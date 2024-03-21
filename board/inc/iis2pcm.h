#ifndef _IIS2PCM_H__
#define _IIS2PCM_H__
#ifdef __cplusplus
extern "C"{
#endif    
#include "iis_pcm_defs.h"
extern bool lFlagRecordReady; // playing buffer fill complete, already a play

extern CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_SECTION uint16_t mic_audio_buffer_IA[MIC_TOTAL_DMA_BUF_SIZE];   // recording DMA buffer A
extern CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_SECTION uint16_t mic_audio_buffer_IB[MIC_TOTAL_DMA_BUF_SIZE];   // recording DMA buffer B

/// @brief change declarations!!! this for test only!!!!

extern CFG_TUSB_MEM_ALIGN8 CFG_BOARD_MEM_MIC_CONVERTER_SECTION int32_t mic_tmpbuf_MA[(MIC_CONVERTER_BUF_SZ)];             //dst_buffer_A for HalfCallback
extern CFG_TUSB_MEM_ALIGN8 CFG_BOARD_MEM_MIC_CONVERTER_SECTION int32_t mic_tmpbuf_MB[(MIC_CONVERTER_BUF_SZ)];             //dst_buffer_B for CpltCallback


// extern CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_CONVERTER_SECTION uint32_t mic_resample_buff[MIC_CONVERTER_BUF_SZ];          // dst_buffer
// extern CFG_BOARD_MEM_MIC_CONVERTER_SECTION CFG_TUSB_MEM_ALIGN uint8_t src_buff[MIC_TOTAL_DMA_BUF_SIZE*2];
// extern CFG_BOARD_MEM_MIC_CONVERTER_SECTION CFG_TUSB_MEM_ALIGN uint8_t dst_buff[MIC_TOTAL_DMA_BUF_SIZE*2]; 


uint16_t returndmabuffersizeinbytes(void);
uint16_t returndmabuffersizeinsamples(void);
uint16_t returntmpbuffsizeinbytes(void);
uint16_t returntmpbuffsizeinsamples(void);

// void movei2stomicrophone(const void* src[], uint8_t dst[], uint16_t nBytes);
// common converter func
// uint16_t convert_3b_i2s24_PCM24(const void * src[], uint8_t dst[], uint16_t nBytes);
// uint16_t bad1_convert_3b_i2s24_PCM24(void* src[], void* dst[], uint16_t nBytes);
// uint16_t bad2_convert_3b_i2s24_PCM24(uint8_t src[], uint8_t dst[], uint32_t length);
// uint16_t Convert_32_to_24_array(const void *src, void *dst, uint32_t length);
uint16_t Convert_32_to_24_array(uint8_t src[], uint8_t dst[], uint32_t length);
// uint16_t Convert_32_to_24_array(uint8_t *src, uint8_t *dst, uint32_t length);
uint16_t convert_3b_i2s24_PCM24(const uint16_t src_buff[], uint16_t startposition, uint16_t src_sz, int32_t dst_buff[], uint16_t dst_pos, uint16_t dst_sz);
#ifdef __cplusplus
}
#endif

#endif