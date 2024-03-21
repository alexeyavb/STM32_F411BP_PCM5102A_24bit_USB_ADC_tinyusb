#ifndef _IIS__PCM__DEFS_H__
#define _IIS__PCM__DEFS_H__
#ifdef __cplusplus
extern "C"{
#endif    

#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <math.h>

typedef  union UN32_ {
        uint8_t b[4];
        int32_t s;
} UN32;



typedef union UN24_ {
        uint8_t b[3];
        int32_t s;
} UN24;


#ifndef MIN
#define MIN( _x_, _y_ )  ((_x_ <= _y_) ? _x_ : _y_)
#endif

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

#ifndef CFG_TUSB_MEM_ALIGN2
#define CFG_TUSB_MEM_ALIGN2        __attribute__ ( ( aligned(2) ) )
#endif

#ifndef CFG_TUSB_MEM_ALIGN8
#define CFG_TUSB_MEM_ALIGN8        __attribute__ ( ( aligned(8) ) )
#endif

#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ( ( aligned(4) ) )
#endif

#ifndef CFG_BOARD_MEM_SPK_SECTION
#define CFG_BOARD_MEM_SPK_SECTION __attribute__ (( section(".spk_buff") ))
#endif

#ifndef CFG_BOARD_MEM_MIC_SECTION
#define CFG_BOARD_MEM_MIC_SECTION __attribute__ (( section(".mic_buff") ))
#endif

#ifndef CFG_BOARD_MEM_MIC_CONVERTER_SECTION
#define CFG_BOARD_MEM_MIC_CONVERTER_SECTION __attribute__ (( section(".mic_conv_buff") ))
#endif

#ifndef AUDIO_TOTAL_BUF_SIZE
#define AUDIO_TOTAL_BUF_SIZE ((1024U + 0U)/2)
#endif


#ifndef MIC_TOTAL_DMA_BUF_SIZE

/// @brief Change from max packet size calculation
// #define MIC_TOTAL_DMA_BUF_SIZE (2*120*2*8)                          // 2chanels 240 samples (two channel)) 2x4bytes 48K 24bit 
#define MIC_SAMPLES_ON_FRAME    4U                                     /// 16U Ok with gain
#define MIC_TOTAL_DMA_BUF_SIZE (2*(MIC_SAMPLES_ON_FRAME)*2*8)                          // 2chanels 240 samples (two channel)) 2x4bytes 48K 24bit 
#define MIC_CONVERTER_BUF_SZ ((MIC_TOTAL_DMA_BUF_SIZE/4))             // using 2 buffers Half/Cplt () from previous declaration in 32bit word
//i need cumulative buuffer?
#endif
#ifndef UNUSED
#define UNUSED(x_param) (void)x_param
#endif

// ref : https://www.microchip.com/forums/m932509.aspx    
__always_inline int32_t USBD_AUDIO_Volume_Ctrl(int32_t sample, int32_t shift_3dB){
    int32_t sample_atten = sample;
    int32_t shift_6dB = shift_3dB>>1;

    if (shift_3dB & 1) {
        // shift_3dB is odd, implement 6dB shift and compensate
        shift_6dB++;       
        sample_atten >>= shift_6dB;
        sample_atten += (sample_atten>>1);
    }
    else{
        // shift_3dB is even, implement with 6dB shift
        sample_atten >>= shift_6dB;
    }
    return sample_atten;
}

#define BYTE_0(val)                    (uint8_t)(val & 0xFF)                            // v = 0x12345678  r uint32_t = 0x00000078, r uint_8t = 0x78
#define BYTE_1(val)                    (uint8_t)((val & 0xFF00) >> 8)                   // v = 0x12345678  r uint32_t = 0x00005600, r uint8_t = 0x56
#define BYTE_2(val)                    (uint8_t)((val & 0xFF0000) >> 16)                // v = 0x12345678  r uint32_t = 0x00340000, r uint8_t = 0x34
#define BYTE_3(val)                    (uint8_t)((val & 0xFF000000) >> 24)              // v = 0x12345678  r uint32_t = 0x12000000, r uint8_t = 0x12

#define SRC236_FRAME_SIZE_MAX_1B          (((96000/100)*2)*2*1)                        // = Frq/100*2chanels*(1x4bit) = 3840bytes   // 16bit frame
#define SRC236_FRAME_SIZE_MAX_2B          (((96000/100)*2)*2*2)                        // = Frq/100*2chanels*(2x4bit) = 3840bytes   // 32bit frame
#define SRC236_FRAME_SIZE_MAX_3B          (((96000/100)*2)*2*3)                        // = Frq/100*2chanels*(3x4bit) = 5760bytes   
#define SRC236_FRAME_SIZE_MAX_4B          (((96000/100)*2)*2*4)                        // = Frq/100*2chanels*(3x4bit) = 7680bytes

__always_inline uint32_t brev_classic (uint32_t a)
{
    uint32_t m;
    a = (a >> 16) | (a << 16);                            // swap halfwords
    m = 0x00ff00ff; a = ((a >> 8) & m) | ((a << 8) & ~m); // swap bytes
    m = m^(m << 4); a = ((a >> 4) & m) | ((a << 4) & ~m); // swap nibbles
    m = m^(m << 2); a = ((a >> 2) & m) | ((a << 2) & ~m);
    m = m^(m << 1); a = ((a >> 1) & m) | ((a << 1) & ~m);
    return a;
}
/* Knuth's algorithm from http://www.hackersdelight.org/revisions.pdf. Retrieved 8/19/2015 */
__always_inline uint32_t brev_knuth (uint32_t a)
{
    uint32_t t;
    a = (a << 15) | (a >> 17);
    t = (a ^ (a >> 10)) & 0x003f801f; 
    a = (t + (t << 10)) ^ a;
    t = (a ^ (a >>  4)) & 0x0e038421; 
    a = (t + (t <<  4)) ^ a;
    t = (a ^ (a >>  2)) & 0x22488842; 
    a = (t + (t <<  2)) ^ a;
    return a;
}

__always_inline uint32_t brev_asm( uint32_t x ){
#if defined(__arm__) || defined(__aarch64__)
    __asm__( "rbit %0, %1" : "=r" ( x ) : "r" ( x ) );
    return x;
#endif
    // Flip pairwise
    x = ( ( x & 0x55555555 ) << 1 ) | ( ( x & 0xAAAAAAAA ) >> 1 );
    // Flip pairs
    x = ( ( x & 0x33333333 ) << 2 ) | ( ( x & 0xCCCCCCCC ) >> 2 );
    // Flip nibbles
    x = ( ( x & 0x0F0F0F0F ) << 4 ) | ( ( x & 0xF0F0F0F0 ) >> 4 );

    // Flip bytes. CPUs have an instruction for that, pretty fast one.
#ifdef _MSC_VER
    return _byteswap_ulong( x );
#elif defined(__INTEL_COMPILER)
    return (uint32_t)_bswap( (int)x );
#else
    // Assuming gcc or clang
    return __builtin_bswap32( x );
#endif
}

#ifdef __cplusplus
}
#endif

#endif //_IIS__PCM__DEFS_H__