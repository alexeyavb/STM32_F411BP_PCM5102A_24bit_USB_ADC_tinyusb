#include "iis2pcm.h"
// playing buffer fill complete, already a play
bool lFlagRecordReady;
// static uint16_t mic_buff_rd_ptr;
// recording double buffer received data from DMA
CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_SECTION uint16_t mic_audio_buffer_IA[MIC_TOTAL_DMA_BUF_SIZE] = {0};      // recording DMA buffer A
CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_SECTION uint16_t mic_audio_buffer_IB[MIC_TOTAL_DMA_BUF_SIZE] = {0};      // recording DMA buffer B

/// @brief change declarations!!! this for test only!!!!
CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_CONVERTER_SECTION int32_t mic_tmpbuf_MA[(MIC_CONVERTER_BUF_SZ)] = {0};   //dst_buffer_A for HalfCallback
CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_CONVERTER_SECTION int32_t mic_tmpbuf_MB[(MIC_CONVERTER_BUF_SZ)] = {0};   //dst_buffer_B for CpltCallback
//// Set as 3byte shift

// converter tmp buffers
// CFG_BOARD_MEM_MIC_CONVERTER_SECTION CFG_TUSB_MEM_ALIGN uint8_t src_buff[MIC_TOTAL_DMA_BUF_SIZE*2];
// CFG_BOARD_MEM_MIC_CONVERTER_SECTION CFG_TUSB_MEM_ALIGN uint8_t dst_buff[MIC_TOTAL_DMA_BUF_SIZE*2]; 

uint16_t returndmabuffersizeinbytes(void){
    return (uint16_t) MIC_TOTAL_DMA_BUF_SIZE * sizeof(mic_audio_buffer_IA[0]);
}

uint16_t returndmabuffersizeinsamples(void){
    return (uint16_t) MIC_TOTAL_DMA_BUF_SIZE;
}

uint16_t returntmpbuffsizeinbytes(void){
    return (uint16_t) (MIC_CONVERTER_BUF_SZ * sizeof(mic_tmpbuf_MA[0]));
}

uint16_t returntmpbuffsizeinsamples(void){
    return (uint16_t) (MIC_CONVERTER_BUF_SZ);
}

/**
 * @brief move 16bit buffer to 8bit for i2s to pcmn convert
*/
void mic_test(const void* src[], uint8_t dst[], uint16_t nBytes){    
    UNUSED(src);
    UNUSED(dst);
    UNUSED(nBytes);
}

/*
**
* @brief:
!!! i think, it`s a WORKING SAMPLE !!! FROM STACKOVERFGLOW!!!
Refer to the video [#13] FIR Filters - Audio DSP On STM32 (24 Bit / 48 kHz)

It also has source code and is very useful.

Change the process of retrieving value for signals of L/R channel.

void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
    lSample = (int)(rxBuf[0] << 16) | rxBuf[1];
    rSample = (int)(rxBuf[2] << 16) | rxBuf[3];
}

void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
   lSample = (int)(rxBuf[4] << 16) | rxBuf[5];
   rSample = (int)(rxBuf[6] << 16) | rxBuf[7];
}
And collect data via a timer, when reached 64 bytes (best performance for CDC), and transmit it.

And the final result.
1k sine wave
https://i.stack.imgur.com/j63sP.jpg

* @param
* @note worked with gain
*/
uint16_t convert_3b_i2s24_PCM24(const uint16_t src_buff[], uint16_t startposition, uint16_t src_sz, int32_t dst_buff[], uint16_t dst_pos, uint16_t dst_sz){    
    // lSample = (int)(rxBuf[0] << 16) | rxBuf[1];
    // rSample = (int)(rxBuf[2] << 16) | rxBuf[3];

    int32_t lSample = 0U;
    int32_t rSample  = 0x00U;
    dst_pos = 0;
    
    for(uint16_t i = startposition; i<src_sz; i+=4){
        lSample = (int)(src_buff[i]) << 16 | (int16_t)(src_buff[i+1]) << 8;
        rSample = (int)(src_buff[i+2]) << 16 | (uint16_t)(src_buff[i+3]) << 8;
        dst_buff[dst_pos++] = rSample; // lSample ; //brev_asm(lSample);
        dst_buff[dst_pos++] = lSample ; //brev_asm(rSample);
    }
    UNUSED(src_buff);
    UNUSED(startposition);
    UNUSED(src_sz);
    UNUSED(dst_buff);
    UNUSED(dst_pos);
    UNUSED(dst_sz);
    UNUSED(rSample);
    UNUSED(lSample);
    return dst_pos;
}

uint16_t p_convert_3b_i2s24_PCM24(const uint16_t src_buff[], uint16_t startposition, uint16_t src_sz, int32_t dst_buff[], uint16_t dst_pos, uint16_t dst_sz){    
    // lSample = (int)(rxBuf[0] << 16) | rxBuf[1];
    // rSample = (int)(rxBuf[2] << 16) | rxBuf[3];
    //   static CFG_TUSB_MEM_ALIGN2 CFG_BOARD_MEM_MIC_SECTION uint8_t mic_audio_buffer_L[MIC_TOTAL_DMA_BUF_SIZE*2] = {0};      // temporary DMA buffer A
    static CFG_TUSB_MEM_ALIGN2 CFG_BOARD_MEM_MIC_SECTION uint8_t mic_audio_buffer_L[MIC_TOTAL_DMA_BUF_SIZE*2] = {0};      // temporary DMA buffer A

    uint16_t src_bytes = returndmabuffersizeinbytes();

    memcpy(mic_audio_buffer_L, (uint8_t*)&src_buff+0, src_bytes);
    uint64_t lSample = 0U;
    uint64_t rSample  = 0x00U;
    dst_pos = 0;
    uint32_t bs_src_sz = ((MIC_TOTAL_DMA_BUF_SIZE*2)*sizeof(mic_audio_buffer_L[0]));
    //* Each I2S stereo sample is encoded as {hi_L:mid_L}, {lo_L:0x00}, {hi_R:mid_R}, {lo_R:0x00}
    // need:
    // * Each 24bit stereo sample is encoded as : L channel 3bytes + R channel 3bytes, LSbyte first
    // * b0:lo_L, b1:mid_L, b2:hi_L, b3:lo_R, b4:mid_R, b5:hi_R       
    uint16_t Sample_LL_ML;
    uint16_t Sample_HL_LR;
    uint16_t Sample_MR_HR;
    for(uint32_t i = startposition; i<bs_src_sz; i+=8){
        Sample_LL_ML = (uint16_t)(
            brev_asm(mic_audio_buffer_L[i+2]) | brev_asm((mic_audio_buffer_L[i+1]))
        );
        Sample_HL_LR = (uint16_t)(
            brev_asm(mic_audio_buffer_L[i] | brev_asm(mic_audio_buffer_L[i+6]))
        );
        Sample_MR_HR = (uint16_t)(
            brev_asm(mic_audio_buffer_L[i+4] | brev_asm(mic_audio_buffer_L[i+5]))
        );
        dst_buff[dst_pos++] = Sample_LL_ML ; 
        dst_buff[dst_pos++] = Sample_HL_LR ;
        dst_buff[dst_pos++] = Sample_MR_HR ;
    }
    UNUSED(src_buff);
    UNUSED(startposition);
    UNUSED(src_sz);
    UNUSED(dst_buff);
    UNUSED(dst_pos);
    UNUSED(dst_sz);
    UNUSED(rSample);
    UNUSED(lSample);
    return dst_pos;
}