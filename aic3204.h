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

esp_err_t aic3204_init(void);
esp_err_t aic3204_set_headphone_mute(aic3204_channel_t ch,bool value);
esp_err_t aic3204_set_line_out_mute(aic3204_channel_t ch,bool value);
esp_err_t aic3204_set_headphone_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_line_out_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_dac_digital_volume(aic3204_channel_t ch,float level);
esp_err_t aic3204_set_input_volume(aic3204_channel_t ch,float level);

#ifdef __cplusplus
}
#endif

#endif
