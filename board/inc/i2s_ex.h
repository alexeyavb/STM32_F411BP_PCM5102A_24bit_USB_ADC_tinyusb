#ifndef _I2S_EX_H__
#define _I2S_EX_H__
#ifdef __cplusplus
extern "C"{
#endif
#ifndef I2S2_MUTE_CTRL_Pin
#define I2S2_MUTE_CTRL_Pin GPIO_PIN_2
#define I2S2_MUTE_CTRL_GPIO_Port GPIOB
#endif

#define AUDIO_MUTE_PIN                                          I2S2_MUTE_CTRL_Pin
#define AUDIO_MUTE_PORT                                         I2S2_MUTE_CTRL_GPIO_Port
#define AUDIO_MUTE_PORT_ENABLE()                                __HAL_RCC_GPIOB_CLK_ENABLE()
#define AUDIO_MUTE_ON()                                         HAL_GPIO_WritePin(AUDIO_MUTE_PORT, AUDIO_MUTE_PIN, GPIO_PIN_RESET)
#define AUDIO_MUTE_OFF()                                        HAL_GPIO_WritePin(AUDIO_MUTE_PORT, AUDIO_MUTE_PIN, GPIO_PIN_SET)

void I2S_BUS_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq, void *Params);

#ifdef __cplusplus
}
#endif

#endif // _I2S_EX_H__
