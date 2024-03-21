#ifndef _I2S_DMA_H__
#define _I2S_DMA_H__
#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
#include <stdbool.h>
extern volatile bool lMicRcvHalfFlag;
extern volatile bool lMicRcvCompFlag;
extern volatile bool lSpkRcvHalfFlag;
extern volatile bool lSpkRcvCompFlag;
// try to move variables to other module
extern volatile bool lMicBufferConvCopyed;
__weak void user_spi3rx_cplt_callback_method(I2S_HandleTypeDef *hi2s);
__weak void user_spi3rx_halfcplt_callback_method(I2S_HandleTypeDef *hi2s);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif