#ifndef _PCM2IIS_H__
#define _PCM2IIS_H__
#ifdef __cplusplus
extern "C"{
#endif    

#include <stdint.h>
#include <stdbool.h>
#include <memory.h>

 // max volume is 0dB, this is to avoid clipping
 #ifndef USBD_AUDIO_VOL_MAX
 #define USBD_AUDIO_VOL_MAX                            0x0000U
 #endif

 // 1dB <=> 0x100, -96dB = 0xA000
 #ifndef USBD_AUDIO_VOL_MIN
 #define USBD_AUDIO_VOL_MIN                            0xA000U
 #endif

 #ifndef USBD_AUDIO_VOL_DEFAULT
 #define USBD_AUDIO_VOL_DEFAULT                        0xA000U
 #endif

 // 3dB step resolution
 #ifndef USBD_AUDIO_VOL_STEP
 #define USBD_AUDIO_VOL_STEP                           0x0300U
 #endif

 // default mute state is on (muted)
#ifndef USBD_AUDIO_MUTE_DEFAULT
#define USBD_AUDIO_MUTE_DEFAULT                         0x00U
#endif

#ifndef AUDIO_SAMPLE_FREQ
#define AUDIO_SAMPLE_FREQ(frq) (uint8_t)(frq), (uint8_t)((frq >> 8)), (uint8_t)((frq >> 16))
#endif

#ifndef AUDIO_PACKET_SZE_24B
 // Max packet size: (freq / 1000 + extra_samples) * channels * bytes_per_sample
 // e.g. (48000 / 1000 + 1) * 2(stereo) * 3(24bit) = 388
#define AUDIO_PACKET_SZE_24B(frq) (uint8_t)(((frq / 1000U + 1) * 2U * 3U) & 0xFFU), \
                                  (uint8_t)((((frq / 1000U + 1) * 2U * 3U) >> 8) & 0xFFU)
#endif

#ifndef AUDIO_FB_DEFAULT
// I2S_Clk_Config24[2].nominal_fdbk (96kHz, 24bit, USE_MCLK_OUT false)
#define AUDIO_FB_DEFAULT 0x1800ED70 
#endif

#ifndef AUDIO_FB_DELTA_MAX
// DbgFeedbackHistory is limited to +/- 1kHz
#define  AUDIO_FB_DELTA_MAX (uint32_t)(1 << 22)
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

#ifndef AUDIO_TOTAL_BUF_SIZE
#define AUDIO_TOTAL_BUF_SIZE 1024U + 256U
#endif
#ifndef UNUSED
#define UNUSED(x_param) (void)x_param
#endif

// playing buffer fill complete, already a play
extern bool lFlagPlayReady;
// playing buffer
extern CFG_TUSB_MEM_ALIGN uint16_t audio_buffer[AUDIO_TOTAL_BUF_SIZE];
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