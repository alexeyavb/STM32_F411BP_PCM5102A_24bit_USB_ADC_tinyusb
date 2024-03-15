#include "i2s.h"
#include "tusb.h"

// #define TMP_DMA_BUFFER_SZ 192
// static uint16_t mic_tmp_dma_buffer[TMP_DMA_BUFFER_SZ];
// extern void ST7735_FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
/**
  * @brief  Tx Transfer Half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI2){
    // it`s speaker
//    TU_LOG1("Speaker");
//    ST7735_FillRectangleFast(00,20,10,10,0xABF);
  }
  if(hi2s->Instance == SPI3){
    // it`s mic
//    TU_LOG1("Microphone");
//    ST7735_FillRectangleFast(00,30,10,10,0xBFB);
  }

  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Tx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI2){
    // it`s speaker
//    ST7735_FillRectangleFast(10,20,10,10,0xCBF);
//    TU_LOG1("Speaker");

  }
  if(hi2s->Instance == SPI3){
    // it`s mic
//    ST7735_FillRectangleFast(10,30,10,10,0xDFB);
//    TU_LOG1("Microphone");
  }
  /* Prevent unused argument(s) compilation warning */  
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI2){
    // it`s speaker
//    ST7735_FillRectangleFast(20,10,10,10,0xEBF);
//    TU_LOG1("Speaker");
    
  }
  if(hi2s->Instance == SPI3){
    // it`s mic
//    ST7735_FillRectangleFast(30,20,10,10,0xFBF);
//    TU_LOG1("Microphone");    
  }
    
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxHalfCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI2){
    // it`s speaker
//    ST7735_FillRectangleFast(40,10,10,10,0xADF);
//    TU_LOG1("Speaker");
  }
  if(hi2s->Instance == SPI3){
    // it`s mic
//    ST7735_FillRectangleFast(40,20,10,10,0xAEF);
//    TU_LOG1("Microphone");
  }
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  I2S error callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
if(hi2s->Instance == SPI2){
    // it`s speaker
//    TU_LOG1("Speaker");
//    ST7735_FillRectangleFast(50,30,10,10,0xFF0);
  }
  if(hi2s->Instance == SPI3){
    // it`s mic
//    ST7735_FillRectangleFast(50,40,10,10,0xFF0);
//    TU_LOG1("Microphone");
  }
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_I2S_ErrorCallback could be implemented in the user file
   */
}
