#include <memory.h>
#include "i2s.h"
#include "i2s_ex.h"

typedef struct I2S_CLK_CONFIG_ {
        uint32_t N;
        uint32_t R;
        uint32_t I2SDIV;
        uint32_t ODD;
        uint32_t nominal_fdbk;
} I2S_CLK_CONFIG;

#if defined(STM32F411xE) && defined(USE_MCLK_OUT) // Makefile compile flags
// for 32bit I2S frame
#define AUDIO_CLOCKS_AVAILABLE 3
const uint32_t I2SFreq[AUDIO_CLOCKS_AVAILABLE] = {44100, 48000, 96000};
const I2S_CLK_CONFIG I2S_Clk_Config24[AUDIO_CLOCKS_AVAILABLE]  = {
    {271, 2, 6, 0, 0x0B06EAB0},     // 44.1081
    {258, 3, 3, 1, 0x0BFF6DB2},     // 47.9911
    {344, 2, 3, 1, 0x17FEDB64},     // 95.9821
};
#else
// for 32bit I2S frame
#define AUDIO_CLOCKS_AVAILABLE 4
const uint32_t I2SFreq[AUDIO_CLOCKS_AVAILABLE] = {44100, 48000, 96000, 192000};
const I2S_CLK_CONFIG I2S_Clk_Config24[AUDIO_CLOCKS_AVAILABLE]  = {
    {429, 4, 19, 0, 0x0B065E56},    // 44.0995
    {384, 5, 12, 1, 0x0C000000},    // 48.0000
    {424, 3, 11, 1, 0x1800ED70},    // 96.0144
    {258, 3, 3, 1,  0x3001DAE0}     // 192.0288
};
#endif

// @brief This worked direct w/o buffer transmission
// #define MIN_SAMPLES_FOR 16U
// #define RX_BUFFER_SZ ((MIN_SAMPLES_FOR*sizeof(uint16_t)*256U)+2U)


// Nominal buffer sz for bufferized transmission
// #define MIN_SAMPLES_FOR 2U
// #define RX_BUFFER_SZ ((MIN_SAMPLES_FOR*sizeof(uint16_t)))

// static uint16_t rxBuffer[RX_BUFFER_SZ];
// @brief read transmission buffer
// uint16_t tmpBuffer[RX_BUFFER_SZ];
/**
 * @brief additional func for I2SPR set
*/
static inline HAL_StatusTypeDef I2S_Config_I2SPR(I2S_HandleTypeDef *hi2s, uint32_t regVal) {

    uint32_t tickstart = 0U;

    __HAL_RCC_PLLI2S_DISABLE();
    tickstart = HAL_GetTick();

    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  != RESET) {
        if((HAL_GetTick() - tickstart ) > PLLI2S_TIMEOUT_VALUE) {
            return HAL_TIMEOUT;
        }
    }

    // SPI2->I2SPR = regVal;
    hi2s->Instance->I2SPR = regVal;

    __HAL_RCC_PLLI2S_ENABLE();
    tickstart = HAL_GetTick();

    while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLI2SRDY)  == RESET)    {
        if((HAL_GetTick() - tickstart ) > PLLI2S_TIMEOUT_VALUE)      {
            return HAL_TIMEOUT;
        }
    }    
    return HAL_OK; 
}

/**
  * @brief  Clock Config, assumes input to PLL I2S Block = 1MHz
  * @param
  * @param  AudioFreq: Audio frequency used to play the audio stream.
  * @note   This API is called by BSP_AUDIO_OUT_Init() and BSP_AUDIO_OUT_SetFrequency()
  *         Being __weak it can be overwritten by the application
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
void I2S_BUS_ClockConfig(I2S_HandleTypeDef *hi2s, uint32_t AudioFreq, void *Params) {
  RCC_PeriphCLKInitTypeDef RCC_ExCLKInitStruct;
  int index = 0, freqindex = -1; 
    
  for(index = 0; index < AUDIO_CLOCKS_AVAILABLE; index++)  {
          if (I2SFreq[index] == AudioFreq) {
                  freqindex = index;
                  break;
                  }
          }
  uint32_t N, R, I2SDIV, ODD, I2S_PR;
#ifdef STM32F411xE
  uint32_t MCKOE;
#ifdef USE_MCLK_OUT
    MCKOE = 1;
#else
    MCKOE = 0;
#endif
#endif
    // PLLI2S_VCO = f(VCO clock) = f(PLLI2S clock input)  (PLLI2SN/PLLM)
    // I2SCLK = f(PLLI2S clock output) = f(VCO clock) / PLLI2SR
  
  HAL_RCCEx_GetPeriphCLKConfig(&RCC_ExCLKInitStruct);
  if (freqindex != -1)  {
    N = I2S_Clk_Config24[freqindex].N;
    R = I2S_Clk_Config24[freqindex].R;
    I2SDIV = I2S_Clk_Config24[freqindex].I2SDIV;
    ODD = I2S_Clk_Config24[freqindex].ODD;
        
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
#ifdef STM32F411xE
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SM = 25;
#endif
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = N;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SR = R;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
#ifdef STM32F411xE
    I2S_PR = (MCKOE<<9) | (ODD<<8) | I2SDIV;
#else
    I2S_PR = (ODD<<8) | I2SDIV;
#endif
    I2S_Config_I2SPR(hi2s, I2S_PR);
    }
  else { // Default PLL I2S configuration for 96000 Hz 24bit  
    N = I2S_Clk_Config24[2].N;
    R = I2S_Clk_Config24[2].R;
    I2SDIV = I2S_Clk_Config24[2].I2SDIV;
    ODD = I2S_Clk_Config24[2].ODD;
    
    RCC_ExCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
#ifdef STM32F411xE
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SM = 25;
#endif
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SN = N;
    RCC_ExCLKInitStruct.PLLI2S.PLLI2SR = R;
    HAL_RCCEx_PeriphCLKConfig(&RCC_ExCLKInitStruct);
#ifdef STM32F411xE
    I2S_PR = (MCKOE<<9) | (ODD<<8) | I2SDIV;
#else
    I2S_PR = (ODD<<8) | I2SDIV;
#endif
    I2S_Config_I2SPR(hi2s, I2S_PR);
  }
  UNUSED(Params);
}
/*
Direct buffer moving
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s) {  

    if(hi2s->Instance == SPI3){

        uint32_t int_left, int_right;
        float float_left, float_right;

        for(uint16_t i=0;i<RX_BUFFER_SZ;i+=4){
            {
                int_left = (int32_t)(
                    ((uint32_t)rxBuffer[i] << 8) | (uint16_t)(rxBuffer[i+1] >> 8)
                );
                if(int_left & (1<<23)){ int_left|=0xFF000000;}; float_left = (int32_t) int_left;
                // float_left *= 2; // ((int32_t)float_left) >>1;
                int_left = (int32_t)(float_left);
                
                tmpBuffer[i] = (uint32_t)int_left>>8;
                tmpBuffer[i+1] = (uint16_t)int_left>>8;            
            }
            {
                int_right = (int32_t)(
                    ((uint32_t)rxBuffer[i+2] << 8) | ((uint16_t)rxBuffer[i+3] >> 8)
                );
                if(int_right & (1<<23)) {int_right|=0xFF000000;}; float_right = (int32_t) int_right;
                // float_right *= 2; // ((int32_t)float_right)>>1;
                int_right = (int32_t)(float_right);
                
                tmpBuffer[i+2] = (uint32_t)int_right>>8;
                tmpBuffer[i+3] = (uint16_t)int_right>>8;
            }
        }
        // HAL_I2S_Transmit_DMA(&hi2s2, tmpBuffer, RX_BUFFER_SZ);
        HAL_I2S_Transmit_DMA(&hi2s2, tmpBuffer, RX_BUFFER_SZ);
    }
}
*/

// void StartRezerved3Task(void *argument){
//     AUDIO_MUTE_OFF();    
//     ////
//     // @brief Direct data moving
//     // HAL_I2S_Receive_DMA(&hi2s3, tmpBuffer, RX_BUFFER_SZ);
//     // @brief over read buffer
//     // HAL_I2S_Receive_DMA(&hi2s3, rxBuffer, RX_BUFFER_SZ);
//     for(;;)
//     {    
//         osDelay(1);
//     }
// }
