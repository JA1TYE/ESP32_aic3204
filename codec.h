#ifndef _CODEC_H_
#define _CODEC_H_

#include "freertos/FreeRTOS.h"
#include "esp_system.h"

#ifdef __cplusplus
extern "C"{
#endif

#define SUCODEC_I2C_PORT I2C_NUM_0
#define SUCODEC_I2C_WRITE 0
#define SUCODEC_I2C_READ 1

#define SUCODEC_I2C_SCL_NUM GPIO_NUM_14
#define SUCODEC_I2C_SDA_NUM GPIO_NUM_27
#define SUCODEC_I2C_FREQ_HZ 400000

#define SUCODEC_I2S_DOUT_NUM    5
#define SUCODEC_I2S_DIN_NUM     18
#define SUCODEC_I2S_WCLK_NUM    19
#define SUCODEC_I2S_BCLK_NUM    21

#define SUCODEC_CODEC_RESET     22
#define SUCODEC_AMP_SHUTDOWN    4
#define SUCODEC_HP_DETECT       35

#define SUCODEC_I2S_PORT I2S_NUM_0

#define SUCODEC_I2S_SAMPLE_RATE_HZ 48000

esp_err_t sucodec_init(void);
esp_err_t sucodec_i2s_init(void);
esp_err_t sucodec_i2c_init(void);
esp_err_t sucodec_gpio_init(void);
esp_err_t sucodec_set_amp_mute(bool value);
bool      sucodec_is_hp_detected(void);

#ifdef __cplusplus
}
#endif

#endif
