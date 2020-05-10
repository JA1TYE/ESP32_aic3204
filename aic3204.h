#ifndef _AIC3204_H_
#define _AIC3204_H_

#include "freertos/FreeRTOS.h"
#include "esp_system.h"

#ifdef __cplusplus
extern "C"{
#endif

//I2C Slave addr.
#define AIC3204_ADDR 0x30

#define AIC3204_OUT_GAIN_MAX    29.0
#define AIC3204_OUT_GAIN_MIN    -6.0
#define AIC3204_IN_GAIN_MAX     20.0
#define AIC3204_IN_GAIN_MIN     -12.0
#define AIC3204_DAC_VOL_MAX     24.0
#define AIC3204_DAC_VOL_MIN     -63.5

typedef enum {
    AIC3204_L,
    AIC3204_R,
    AIC3204_BOTH
}aic3204_channel_t;

typedef enum {
    AIC3204_IN1_L,
    AIC3204_IN1_R,
    AIC3204_IN2_L,
    AIC3204_IN2_R,
    AIC3204_IN3_L,
    AIC3204_IN3_R,
    AIC3204_IN1_BOTH,
    AIC3204_IN2_BOTH,
    AIC3204_IN3_BOTH
}aic3204_adc_input_t;

typedef enum {
    AIC3204_L_P = 1,
    AIC3204_L_N = 2,
    AIC3204_R_P = 4,
    AIC3204_R_N = 8
}aic3204_adc_routing_t;

typedef enum {
    AIC3204_IN_0db = 1,
    AIC3204_IN_6db = 2,
    AIC3204_IN_12db = 3,
    AIC3204_IN_NC = 0
}aic3204_adc_gain_t;

const int AIC3204_IN1_L_VALID_MASK = AIC3204_L_P | AIC3204_R_N;
const int AIC3204_IN1_R_VALID_MASK = AIC3204_L_P | AIC3204_R_P;
const int AIC3204_IN2_L_VALID_MASK = AIC3204_L_P | AIC3204_R_P;
const int AIC3204_IN2_R_VALID_MASK = AIC3204_L_N | AIC3204_R_P;
const int AIC3204_IN3_L_VALID_MASK = AIC3204_L_P | AIC3204_R_N;
const int AIC3204_IN3_R_VALID_MASK = AIC3204_L_N | AIC3204_R_P;

esp_err_t aic3204_init(void);
esp_err_t aic3204_set_headphone_mute(aic3204_channel_t ch,bool value);
esp_err_t aic3204_set_line_out_mute(aic3204_channel_t ch,bool value);
esp_err_t aic3204_set_headphone_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_line_out_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_dac_digital_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_input_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_adc_routing(aic3204_adc_input_t in_terminal,int routing_mask,aic3204_adc_gain_t gain);

#ifdef __cplusplus
}
#endif

#endif
