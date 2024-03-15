#include "pcm2iis.h"

#ifdef __cplusplus
#extern "C"{
#endif

CFG_TUSB_MEM_ALIGN typedef  union UN32_ {
        uint8_t b[4];
        int32_t s;
} UN32, *PUN32, *LPUN32;
static uint16_t spk_buff_wr_ptr;

bool lFlagPlayReady;

CFG_TUSB_MEM_ALIGN uint16_t audio_buffer[AUDIO_TOTAL_BUF_SIZE];
static CFG_TUSB_MEM_ALIGN uint8_t tmpbuf[(AUDIO_TOTAL_BUF_SIZE)/3];
#ifdef __cplusplus
}
#endif
// ref : https://www.microchip.com/forums/m932509.aspx    
inline static int32_t USBD_AUDIO_Volume_Ctrl(int32_t sample, int32_t shift_3dB){
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

/**
 * @brief convert one 16bit pcm sample to 16bit i2s sample
 * @brief  USB PCM-->I2SPHILLIPS 3bytes/per sample
 * incoming USB audio data buffer : uint8_t array
 * Each 24bit stereo sample is encoded as : L channel 3bytes + R channel 3bytes, LSbyte first
 * b0:lo_L, b1:mid_L, b2:hi_L, b3:lo_R, b4:mid_R, b5:hi_R  
 * volume control is implemented by scaling the data, attenuation resolution is 3dB.
 * 6dB is equivalent to a shift right by 1 bit.
 * outgoing I2S Philips data format is : left-aligned 24bits in 32bit frame, MSbyte first
 * STM32 I2S peripheral uses a 16bit data register
 * => outgoing I2S transmit data buffer : uint16_t array
 * Each I2S stereo sample is encoded as {hi_L:mid_L}, {lo_L:0x00}, {hi_R:mid_R}, {lo_R:0x00}
 * @param src - source buffer
 * @param dst - destination buffer
 * @param nBytes - bytes to convert (in bytes!!!!)
*/
uint16_t convert_3b_pcm24_i2s24(const void* src[], uint16_t dst[], uint16_t nBytes){
    (void)src;
    (void)dst;
    (void)nBytes;    
    memcpy(tmpbuf, ((uint8_t*)src+0), nBytes);      // perfect bitrate!!!
    uint32_t tmpbuf_ptr = 0U;
    uint32_t num_samples = nBytes / 6; // 3bytes per sample

    for (uint32_t i = 0; i < num_samples; i++) {
        UN32 lsample; UN32 rsample;
        lsample.b[0] = tmpbuf[tmpbuf_ptr]; // lsb
        lsample.b[1] = tmpbuf[tmpbuf_ptr+1];
        lsample.b[2] = tmpbuf[tmpbuf_ptr+2]; // msb
        lsample.b[3] = lsample.b[2] & 0x80 ? 0xFF : 0x00; // sign extend to 32bits
        lsample.s = USBD_AUDIO_Volume_Ctrl(lsample.s,USBD_AUDIO_VOL_DEFAULT);

        dst[spk_buff_wr_ptr++] = (((uint16_t)lsample.b[2]) << 8) | (uint16_t)lsample.b[1];
        dst[spk_buff_wr_ptr++] = ((uint16_t)lsample.b[0]) << 8;

        rsample.b[0] = tmpbuf[tmpbuf_ptr+3]; // lsb
        rsample.b[1] = tmpbuf[tmpbuf_ptr+4];
        rsample.b[2] = tmpbuf[tmpbuf_ptr+5]; // msb
        rsample.b[3] = rsample.b[2] & 0x80 ? 0xFF : 0x00; // sign extend to 32bits

        rsample.s = USBD_AUDIO_Volume_Ctrl(rsample.s,USBD_AUDIO_VOL_DEFAULT);

        dst[spk_buff_wr_ptr++] = (((uint16_t)rsample.b[2]) << 8) | (uint16_t)rsample.b[1];
        dst[spk_buff_wr_ptr++] = ((uint16_t)rsample.b[0]) << 8;
        tmpbuf_ptr += 6;
        // Rollover at end of buffer
        if (spk_buff_wr_ptr >= AUDIO_TOTAL_BUF_SIZE) {                
                lFlagPlayReady = true;
                spk_buff_wr_ptr = 0U;
        };
    };
    return num_samples;
}
    

uint16_t convert_2b_pcm16b_i2s16(const void* src[], uint16_t dst[], uint16_t nBytes){
    UNUSED(src);
    UNUSED(dst);
    UNUSED(nBytes);

    return 0;
}

uint16_t convert_3b_pcm16b_i2s16(const void* src[], uint16_t dst[], uint16_t nBytes){
    UNUSED(src);
    UNUSED(dst);
    UNUSED(nBytes);
    return 0;
}
