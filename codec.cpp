#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/i2s_std.h"
#include "esp_system.h"
#include "esp_err.h"
#include "aic3204.h"
#include "codec.h"

static i2s_chan_handle_t                tx_chan;        // I2S tx channel handler
static i2s_chan_handle_t                rx_chan;        // I2S rx channel handler

esp_err_t sucodec_i2c_init(void){ 
    i2c_config_t conf;
    memset(&conf,0,sizeof(i2c_config_t));
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SUCODEC_I2C_SDA_NUM;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = SUCODEC_I2C_SCL_NUM;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = SUCODEC_I2C_FREQ_HZ;
    conf.clk_flags = 0;
    i2c_param_config(SUCODEC_I2C_PORT, &conf);
    return i2c_driver_install(SUCODEC_I2C_PORT, conf.mode,
                                0,//I2C Rx Buffer disable
                                0,//I2C Tx Buffer disable
                                0);//Interrupt allocation disable
}

esp_err_t sucodec_i2s_init(void){
    esp_err_t ret;

    //Configure I2S_NUM_0 as Full-Duplex mode
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(SUCODEC_I2S_PORT,I2S_ROLE_MASTER);
    ret = i2s_new_channel(&chan_cfg,&tx_chan,&rx_chan);
    if(ret != ESP_OK)return ret;

    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(SUCODEC_I2S_SAMPLE_RATE_HZ),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = SUCODEC_I2S_MCLK_NUM,
            .bclk = SUCODEC_I2S_BCLK_NUM,
            .ws   = SUCODEC_I2S_WCLK_NUM,
            .dout = SUCODEC_I2S_DOUT_NUM,
            .din  = SUCODEC_I2S_DIN_NUM,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    //Use APLL
    std_cfg.clk_cfg.clk_src = I2S_CLK_SRC_APLL;

    ret = i2s_channel_init_std_mode(tx_chan, &std_cfg);
    if(ret != ESP_OK)return ret;    
    ret = i2s_channel_init_std_mode(rx_chan, &std_cfg);
    if(ret != ESP_OK)return ret;

    i2s_channel_enable(tx_chan);
    if(ret != ESP_OK)return ret;
    i2s_channel_enable(rx_chan);
    if(ret != ESP_OK)return ret;

    return ret;
}

esp_err_t sucodec_gpio_init(void){
    esp_err_t ret;

    //Configure SUCODEC_CODEC_RESET as Output
    gpio_config_t io_conf;
    memset(&io_conf,0,sizeof(gpio_config_t));
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

esp_err_t sucodec_write(const void *src,size_t size,size_t *bytes_written,uint32_t timeout_ms){
    return i2s_channel_write(tx_chan,src,size,bytes_written,timeout_ms);
}

esp_err_t sucodec_read(void *dest,size_t size,size_t *bytes_read,uint32_t timeout_ms){
    return i2s_channel_read(rx_chan,dest,size,bytes_read,timeout_ms);
} 