#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "esp_system.h"
#include "esp_err.h"
#include "aic3204.h"
#include "codec.h"

esp_err_t sucodec_i2c_init(void){ 
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SUCODEC_I2C_SDA_NUM;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = SUCODEC_I2C_SCL_NUM;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = SUCODEC_I2C_FREQ_HZ;
    i2c_param_config(SUCODEC_I2C_PORT, &conf);
    return i2c_driver_install(SUCODEC_I2C_PORT, conf.mode,
                                0,//I2C Rx Buffer disable
                                0,//I2C Tx Buffer disable
                                0);//Interrupt allocation disable
}

esp_err_t sucodec_i2s_init(void){
    esp_err_t ret;

    //Configure I2S_NUM_0 as Full-Duplex mode
    i2s_config_t i2s_config;

    i2s_config.mode = (i2s_mode_t)(I2S_MODE_TX | I2S_MODE_RX | I2S_MODE_MASTER);
    i2s_config.sample_rate = SUCODEC_I2S_SAMPLE_RATE_HZ;
    i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
    i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;//2-channels
    i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    i2s_config.dma_buf_count = 8;
    i2s_config.dma_buf_len = 64;//Unit is [samples], not [bytes]! 
    i2s_config.use_apll = true;
    i2s_config.fixed_mclk = 0;
    i2s_config.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;//Interrupt level 1

    i2s_pin_config_t pin_config;

    pin_config.bck_io_num = SUCODEC_I2S_BCLK_NUM;
    pin_config.ws_io_num = SUCODEC_I2S_WCLK_NUM;
    pin_config.data_out_num = SUCODEC_I2S_DOUT_NUM;
    pin_config.data_in_num = SUCODEC_I2S_DIN_NUM;
    ret = i2s_driver_install(SUCODEC_I2S_PORT, &i2s_config, 0, NULL);
    if(ret != ESP_OK)return ret;

    ret = i2s_set_pin(SUCODEC_I2S_PORT, &pin_config);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);
    WRITE_PERI_REG(PIN_CTRL, READ_PERI_REG(PIN_CTRL) & 0xFFFFFFF0);
    return ret;
}

esp_err_t sucodec_gpio_init(void){
    esp_err_t ret;

    //Configure SUCODEC_CODEC_RESET as Output
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1 << SUCODEC_CODEC_RESET);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret = gpio_config(&io_conf);
    if(ret != ESP_OK)return ret;

    //Configure SUCODEC_AMP_SHUTDOWN as Output
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1 << SUCODEC_AMP_SHUTDOWN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret = gpio_config(&io_conf);
    if(ret != ESP_OK)return ret;

    //Configure SUCODEC_HPDET as Input
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = ((uint64_t)1 << SUCODEC_HP_DETECT);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    ret = gpio_config(&io_conf);
    if(ret != ESP_OK)return ret;

    ret = gpio_set_level((gpio_num_t)SUCODEC_CODEC_RESET,0);
    if(ret != ESP_OK)return ret;
    
    ret = gpio_set_level((gpio_num_t)SUCODEC_AMP_SHUTDOWN,0);
    if(ret != ESP_OK)return ret;

    return ret;
}

esp_err_t sucodec_set_amp_mute(bool value){
    esp_err_t ret = ESP_OK;
    if(value == true){
        ret = gpio_set_level((gpio_num_t)SUCODEC_AMP_SHUTDOWN,0);
    }
    else{
        ret = gpio_set_level((gpio_num_t)SUCODEC_AMP_SHUTDOWN,1);
    }
    return ret;
}

bool sucodec_is_hp_detected(void){
    if(gpio_get_level((gpio_num_t)SUCODEC_HP_DETECT) == 0){
        return true;
    }
    return false;
}

esp_err_t sucodec_aic3204_init(void){
    return aic3204_init();
}

esp_err_t sucodec_init(void){
    esp_err_t ret;
    ret = sucodec_gpio_init();
    if(ret != ESP_OK)return ret;
    ret = sucodec_i2c_init();
    if(ret != ESP_OK)return ret;
    ret = sucodec_i2s_init();
    if(ret != ESP_OK)return ret;
    ret = sucodec_aic3204_init();    
    return ret;
}