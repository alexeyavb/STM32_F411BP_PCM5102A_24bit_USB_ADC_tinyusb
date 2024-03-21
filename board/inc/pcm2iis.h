#ifndef _PCM2IIS_H__
#define _PCM2IIS_H__
#ifdef __cplusplus
extern "C"{
#endif    
#include "iis_pcm_defs.h"

// playing buffer fill complete, already a play
extern bool lFlagPlayReady;
// playing buffer
extern CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_SPK_SECTION  uint16_t spk_audio_buffer_A[AUDIO_TOTAL_BUF_SIZE];
extern CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_SPK_SECTION  uint16_t spk_audio_buffer_B[AUDIO_TOTAL_BUF_SIZE];

// recording buffer
// convert 3b pcm to 3b i2s 24bit
uint16_t convert_3b_pcm24_i2s24(const void* src[], uint16_t dst[], uint16_t nBytes);
// convert 2b pcm to 2b i2s 16bit
uint16_t convert_2b_pcm16b_i2s16(const void* src[], uint16_t dst[], uint16_t nBytes);
// convert 3b pcm to 3b i2s 16bit
uint16_t convert_3b_pcm16b_i2s16(const void* src[], uint16_t dst[], uint16_t nBytes);

#ifdef __cplusplus
}
#endif
#endif