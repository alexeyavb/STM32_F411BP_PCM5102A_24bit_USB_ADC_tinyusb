#include "i2s.h"
#include "i2s_dma.h"

volatile bool lMicRcvHalfFlag;
volatile bool lMicRcvCompFlag;
volatile bool lSpkRcvHalfFlag;
volatile bool lSpkRcvCompFlag;
volatile bool lMicBufferConvCopyed;

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
    lSpkRcvHalfFlag = true;
  }
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);
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
    lSpkRcvCompFlag = true;
  }
  /* Prevent unused argument(s) compilation warning */  
  UNUSED(hi2s);
}

/**
  * @brief  Rx Transfer half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI3){
    lMicRcvHalfFlag = true;  
    user_spi3rx_halfcplt_callback_method(hi2s);
  }
  UNUSED(hi2s);
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
  if(hi2s->Instance == SPI3){
    lMicRcvCompFlag = true;
    user_spi3rx_cplt_callback_method(hi2s);
  }
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hi2s);
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
    // it`s spk
  }
  if(hi2s->Instance == SPI3){
    // it`s mic
  }
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hi2s);
}

/*
 * Configure the DMA Stream for I2S TX to work in double-buffer mode
 */

#define SINE_LENGTH 0
//buffers
uint8_t waveform1[1024];
uint8_t waveform2[1024];
void  DMA_DBM_Setup(I2S_HandleTypeDef haudio_i2s)
{
	static DMA_Stream_TypeDef* DMA_Stream = DMA1_Stream4;
	// Disable the DMA stream first
	DMA_Stream->CR &=  ~DMA_SxCR_EN;
	// Wait until the stream is disabled
	while(DMA_Stream->CR & DMA_SxCR_EN);
 
	// Assign I2S2 DR to DMA stream
	DMA_Stream->PAR = (uint32_t)&haudio_i2s.Instance->DR;
 
	// Configure source audio buffers used in DBM
	DMA_Stream->M0AR = (uint32_t)waveform1;
	DMA_Stream->M1AR = (uint32_t)waveform2;
 
	// Set the length/size of the DATA stream in
	DMA_Stream->NDTR = SINE_LENGTH;
 
	DMA_Stream->CR |= (0 << DMA_SxCR_CHSEL_Pos); // DMA channel selection
	DMA_Stream->CR &= ~DMA_SxCR_MBURST; // Set single memory burst
	DMA_Stream->CR &= ~DMA_SxCR_PBURST; // Set single peripheral burst
	DMA_Stream->CR &= ~DMA_SxCR_CT; 	// Set memory to peripheral mode
	DMA_Stream->CR &= ~DMA_SxCR_PL_1; 	// Set DMA priority to high
	DMA_Stream->CR &= ~DMA_SxCR_PINCOS; // Set peripheral increment offset size to 32-bit
	DMA_Stream->CR |= DMA_SxCR_MINC; 	// Enable memory increment mode
	DMA_Stream->CR &= ~(DMA_SxCR_PINC);  // Disable peripheral increment mode
	DMA_Stream->CR |= DMA_SxCR_MSIZE_0; // Set memory data size to 16-bit
	DMA_Stream->CR |= DMA_SxCR_PSIZE_0; // Set peripheral data size to 16-bit
	DMA_Stream->CR |= DMA_SxCR_CIRC; 	// Enable circular mode
	DMA_Stream->CR |= (DMA_SxCR_DBM);  // Enable double buffer mode
	DMA_Stream->CR |= (DMA_SxCR_TCIE | DMA_SxCR_HTIE);  // Enable TC and HC interrupts
 
	/* Enable DMA stream */
	DMA_Stream->CR |= DMA_SxCR_EN;
}

__weak void user_spi3rx_cplt_callback_method(I2S_HandleTypeDef *hi2s){  
  UNUSED(hi2s);
  return;
}
__weak void user_spi3rx_halfcplt_callback_method(I2S_HandleTypeDef *hi2s){
  UNUSED(hi2s);
  return;
}