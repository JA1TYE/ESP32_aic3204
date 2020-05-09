#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_err.h"
#include "codec.h"
#include "aic3204.h"

esp_err_t aic3204_write_reg(uint8_t addr,uint8_t data){
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AIC3204_ADDR | SUCODEC_I2C_WRITE, true);
    i2c_master_write_byte(cmd, addr, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(SUCODEC_I2C_PORT, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

uint8_t aic3204_read_reg(uint8_t addr){
    uint8_t value;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AIC3204_ADDR | SUCODEC_I2C_WRITE, true);
    i2c_master_write_byte(cmd, addr, true);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AIC3204_ADDR | SUCODEC_I2C_READ, true);
    i2c_master_read_byte(cmd, &value, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(SUCODEC_I2C_PORT, cmd, 50 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return value;
}

esp_err_t aic3204_set_headphone_mute(aic3204_channel_t ch,bool value){
    uint8_t tmp;
    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    if(ch == AIC3204_L){
        //Read the HPL gain
        tmp = aic3204_read_reg(0x10);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the HPL gain
        aic3204_write_reg(0x10,tmp);
    }
    else if(ch == AIC3204_R){
        //Read the HPR gain
        tmp = aic3204_read_reg(0x11);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the HPR gain
        aic3204_write_reg(0x11,tmp);
    }
    else{
        //Read the HPL gain
        tmp = aic3204_read_reg(0x10);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the HPL gain
        aic3204_write_reg(0x10,tmp);
        //Read the HPR gain
        tmp = aic3204_read_reg(0x11);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the HPR gain
        aic3204_write_reg(0x11,tmp);
    }
    return ESP_OK;
}

esp_err_t aic3204_set_line_out_mute(aic3204_channel_t ch,bool value){
    uint8_t tmp;
    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    if(ch == AIC3204_L){
        //Read the LOL gain
        tmp = aic3204_read_reg(0x12);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the LOL gain
        aic3204_write_reg(0x12,tmp);
    }
    else if(ch == AIC3204_R){
        //Read the LOR gain
        tmp = aic3204_read_reg(0x13);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the LOR gain
        aic3204_write_reg(0x13,tmp);
    }
    else{
        //Read the LOL gain
        tmp = aic3204_read_reg(0x12);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the LOL gain
        aic3204_write_reg(0x12,tmp);
        //Read the LOR gain
        tmp = aic3204_read_reg(0x13);
        if(value == true){
            tmp |= 0x40;
        }
        else{
            tmp &= 0x3f;
        }
        //Set the LOR gain
        aic3204_write_reg(0x13,tmp);
    }
    return ESP_OK;
}

esp_err_t aic3204_set_headphone_volume(aic3204_channel_t ch,float level){
    if(level > AIC3204_OUT_GAIN_MAX){
        return ESP_ERR_INVALID_ARG;
    }
    if(level < AIC3204_OUT_GAIN_MIN){
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t value;
    uint8_t tmp;
    //Register setting value is expressed by 2's complement, 6-bit
    //e.g. : -6db = 0b11 1010
    value = ((int8_t)level) & 0x3f;
    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    if(ch == AIC3204_L){
        //Read the HPL gain
        tmp = aic3204_read_reg(0x10);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the HPL gain
        aic3204_write_reg(0x10,value);
    }
    else if(ch == AIC3204_R){
        //Read the HPR gain
        tmp = aic3204_read_reg(0x11);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the HPR gain
        aic3204_write_reg(0x11,value);
    }
    else{
        //Read the HPL gain
        tmp = aic3204_read_reg(0x10);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the HPL gain
        aic3204_write_reg(0x10,value);
        //Read the HPR gain
        tmp = aic3204_read_reg(0x11);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the HPR gain
        aic3204_write_reg(0x11,value);
    }
    return ESP_OK;
}

esp_err_t aic3204_set_line_out_volume(aic3204_channel_t ch,float level){
    if(level > AIC3204_OUT_GAIN_MAX){
        return ESP_ERR_INVALID_ARG;
    }
    if(level < AIC3204_OUT_GAIN_MIN){
        return ESP_ERR_INVALID_ARG;
    }
    uint8_t value;
    uint8_t tmp;
    //Register setting value is expressed by 2's complement, 6-bit
    //e.g. : -6db = 0b11 1010
    value = ((int8_t)level) & 0x3f;
    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    if(ch == AIC3204_L){
        //Read the LOL gain
        tmp = aic3204_read_reg(0x12);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the LOL gain
        aic3204_write_reg(0x12,value);
    }
    else if(ch == AIC3204_R){
        //Read the LOR gain
        tmp = aic3204_read_reg(0x13);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the LOR gain
        aic3204_write_reg(0x13,value);
    }
    else{
        //Read the LOL gain
        tmp = aic3204_read_reg(0x12);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the LOL gain
        aic3204_write_reg(0x12,value);
        //Read the LOR gain
        tmp = aic3204_read_reg(0x13);
        if(tmp & 0x40){
            value |= 0x40;
        }
        else{
            value &= 0x3f;
        }
        //Set the LOR gain
        aic3204_write_reg(0x13,value);
    }
    return ESP_OK;
}

esp_err_t aic3204_set_dac_digital_volume(aic3204_channel_t ch,float level){
    uint8_t value;
    if(level > AIC3204_DAC_VOL_MAX){
        return ESP_ERR_INVALID_ARG;
    }
    else if(level < AIC3204_DAC_VOL_MIN){
        return ESP_ERR_INVALID_ARG;
    }

    level = level * 2.0;
    value = (int8_t)level;

    //Select Page 0
    aic3204_write_reg(0x00,0x00);
    if(ch == AIC3204_L){
        //Set the LOL gain
        aic3204_write_reg(0x41,value);
    }
    else if(ch == AIC3204_R){
        //Set the LOR gain
        aic3204_write_reg(0x42,value);
    }
    else{
        aic3204_write_reg(0x41,value);        
        aic3204_write_reg(0x42,value);
    }
    return ESP_OK;
}

esp_err_t aic3204_set_input_volume(aic3204_channel_t ch,float level){
    int8_t value;
    if(level > AIC3204_IN_GAIN_MAX){
        return ESP_ERR_INVALID_ARG;
    }
    else if(level < AIC3204_IN_GAIN_MIN){
        return ESP_ERR_INVALID_ARG;
    }

    level = level * 2.0;
    value = (int8_t)level;
    value = value & 0x7f;

    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    if(ch == AIC3204_L){
        //Set the LOL gain
        aic3204_write_reg(0x3B,level);
    }
    else if(ch == AIC3204_R){
        //Set the LOR gain
        aic3204_write_reg(0x3C,level);
    }
    else{
        aic3204_write_reg(0x3B,level);        
        aic3204_write_reg(0x3C,level);
    }
    return ESP_OK;
}

esp_err_t aic3204_reset(void){
    esp_err_t ret;
    ret = gpio_set_level((gpio_num_t)SUCODEC_CODEC_RESET,0);
    if(ret != ESP_OK)return ret;
    vTaskDelay(100/portTICK_RATE_MS);
    ret = gpio_set_level((gpio_num_t)SUCODEC_CODEC_RESET,1);
    if(ret != ESP_OK)return ret;
    vTaskDelay(10/portTICK_RATE_MS);
    return ret;
}

esp_err_t aic3204_init(void){
    aic3204_reset();
    //Page 0
    aic3204_write_reg(0x00,0x00);
    //Software Reset
    aic3204_write_reg(0x01,0x01);
    vTaskDelay(10/portTICK_RATE_MS);
    /*---Clock Settings---*/
    //Select CODEC_CLKIN = MCLK
    aic3204_write_reg(0x19,0x00);
    //Set CLKOUT_M Divider Active
    aic3204_write_reg(0x1a,0x81);
    //Power up the NDAC divider with value 1
    aic3204_write_reg(0x0b,0x81);
    //Power up the MDAC divider with value 2
    aic3204_write_reg(0x0c,0x82);
    // Power up NADC divider with value 1
    aic3204_write_reg(0x12,0x81);
    // Power up MADC divider with value 2
    aic3204_write_reg(0x13,0x82);
    //Program the OSR of DAC to 128
    aic3204_write_reg(0x0d,0x00);
    aic3204_write_reg(0x0e,0x80);
    // Program OSR for ADC to 128
    aic3204_write_reg(0x14,0x80);

    /*---Digital Section---*/
    //Set I2S, 16bit Data Word, BCLK and WCLK is input
    //aic3204_write_reg(0x1b,0x00);
    //Set I2S, 32bit Data Word, BCLK and WCLK is input
    aic3204_write_reg(0x3b,0x00);
    //Set the DAC Mode to PRB_P8
    aic3204_write_reg(0x3c,0x08);
    // Select ADC PRB_R1
    aic3204_write_reg(0x3d,0x01);
    
    /*---Power and Analog---*/
    //Select Page 1
    aic3204_write_reg(0x00,0x01);
    //Disable Internal Crude AVdd in presence of external AVdd supply or before
    //powering up internal AVdd LDO
    aic3204_write_reg(0x01,0x08);
    //Enable Master Analog Power Control
    aic3204_write_reg(0x02,0x01);
    //Set the REF charging time to 40ms
    aic3204_write_reg(0x7b,0x01);
    //HP soft stepping settings for optimal pop performance at power up
    //Rpop used is 6k with N = 6 and soft step = 20usec. This should work with 47uF coupling
    //capacitor. Can try N=5,6 or 7 time constants as well. Trade-off delay vs “pop” sound.
    aic3204_write_reg(0x14,0x25);
    //Set the Input Common Mode to 0.9V and Output Common Mode for Headphone to
    //Input Common Mode
    aic3204_write_reg(0x0a,0x00);

    /*---Analog Routing---*/
    //Route Left DAC to HPL
    aic3204_write_reg(0x0c,0x08);
    //Route Right DAC to HPR
    aic3204_write_reg(0x0d,0x08);
    //Route Left DAC to LOL
    aic3204_write_reg(0x0e,0x08);
    //Route Right DAC to LOR
    aic3204_write_reg(0x0f,0x08);
    //Set the DAC PTM mode to PTM_P3/4
    aic3204_write_reg(0x03,0x00);
    aic3204_write_reg(0x04,0x00);
    // Select ADC PTM_R4
    aic3204_write_reg(0x3d,0x00);
    //Set the HPL gain to -6dB
    aic3204_write_reg(0x10,0x3a);
    //Set the HPR gain to -6dB
    aic3204_write_reg(0x11,0x3a);
    //Set the LOL gain to -6dB
    aic3204_write_reg(0x12,0x3a);
    //Set the LOR gain to -6dB
    aic3204_write_reg(0x13,0x3a);
    //Power up HPL and HPR, LOL and LOR drivers
    aic3204_write_reg(0x09,0x3c);
    //Wait for 2.5 sec for soft stepping to take effect
    //Else read Page 1, Register 63d, D(7:6). When = “11” soft-stepping is complete
    // Set MicPGA startup delay to 3.1ms
    aic3204_write_reg(0x47,0x32);
    // Set the REF charging time to 40ms
    aic3204_write_reg(0x7b,0x01);
    // Route IN1L to LEFT_P with 20K input impedance
    aic3204_write_reg(0x34,0x80);
    // Route Common Mode to LEFT_M with impedance of 20K
    aic3204_write_reg(0x36,0x80);
    // Route IN1R to RIGHT_P with input impedance of 20K
    aic3204_write_reg(0x37,0x80);
    // Route Common Mode to RIGHT_M with impedance of 20K
    aic3204_write_reg(0x39,0x80);
    // Unmute Left MICPGA, Gain selection of 6dB to make channel gain 0dB
    // Register of 6dB with input impedance of 20K => Channel Gain of 0dB
    aic3204_write_reg(0x3b,0x0c);
    // Unmute Right MICPGA, Gain selection of 6dB to make channel gain 0dB
    // Register of 6dB with input impedance of 20K => Channel Gain of 0dB
    aic3204_write_reg(0x3c,0x0c);
    //Select Page 0
    aic3204_write_reg(0x00,0x00);
    //Power up the Left and Right DAC Channels with route the Left Audio digital data to
    //Left Channel DAC and Right Audio digital data to Right Channel DAC
    aic3204_write_reg(0x3f,0xd6);
    // Power up Left and Right ADC Channels
    aic3204_write_reg(0x51,0xc0);
    //Unmute the DAC digital volume control and enable syncronized control (use L-ch)
    aic3204_write_reg(0x40,0x00);
    // Unmute Left and Right ADC Digital Volume Control.
    aic3204_write_reg(0x52,0x00);

    return ESP_OK;
}
