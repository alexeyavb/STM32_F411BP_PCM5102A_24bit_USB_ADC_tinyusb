static  buffer_t InputBuffer;
static  buffer_t *pInputBuffer = &InputBuffer;
static  buffer_t OutputBuffer;
static  buffer_t *pOutputBuffer = &OutputBuffer;


static void resampletask(void* srcbuff[], void* dstbuff[], uint16_t src_buff_sz, uint16_t dst_buff_sz );
/**
 * @brief resampling test
 * 
*/
/*
// static void resampletask(void* input_buff_ptr, void* output_buff_ptr, int32_t input_buff_sz, int32_t output_buff_sz);

void *persistent_mem_ptr;
void *scratch_mem_ptr;
static src236_static_param_t input_static_param_ptr;
static src236_dynamic_param_t input_dynamic_param_ptr;

buffer_t input_buffer;
buffer_t output_buffer;
static bool lFlgConvertorInitComplete;
static inline void resampletask(void* input_buff_ptr, void* output_buff_ptr, int32_t input_buff_sz, int32_t output_buff_sz){
  int32_t resamplestatus = 0;
  if(!lFlgConvertorInitComplete){
    
    resamplestatus = src236_reset(&persistent_mem_ptr, &scratch_mem_ptr);
    resamplestatus = src236_getParam(&input_static_param_ptr, &persistent_mem_ptr);
    input_static_param_ptr.src_mode = SRC236_RATIO_1_2;

    resamplestatus = src236_setParam(&input_static_param_ptr, &persistent_mem_ptr);
    resamplestatus = src236_setConfig(&input_dynamic_param_ptr, &persistent_mem_ptr);
    // check Errors!!!
    lFlgConvertorInitComplete = true;
  }
  
  resamplestatus = src236_getConfig(&input_dynamic_param_ptr, &persistent_mem_ptr);

  input_buffer.buffer_size = input_buff_sz;
  input_buffer.data_ptr    = input_buff_ptr;
  input_buffer.mode        = 1; //  1 interleaved/0 non-interleaved
  input_buffer.nb_bytes_per_Sample = 3; // 2 = 16b/3 = 24b/4 = 32b
  input_buffer.nb_channels = 2;

  output_buffer.buffer_size = output_buff_sz;
  output_buffer.data_ptr    = output_buff_ptr;
  output_buffer.mode        = 1; //  1 interleaved/0 non-interleaved
  output_buffer.nb_bytes_per_Sample = 3; // 2 = 16b/3 = 24b/4 = 32b
  output_buffer.nb_channels = 2;

  resamplestatus = src236_process(&input_buffer, &output_buffer, &persistent_mem_ptr);
  if(resamplestatus != 0)
    lFlgConvertorInitComplete = false;
    
  
  UNUSED(resamplestatus);
}
*/

/*
* @brief Resample process with libresample
*/
/*Audio module parameters memory allocation*/
/**
 * types
*/
typedef struct {
  uint32_t FileSize;       /*in bytes*/
  uint32_t FileFormat;     /* '3':MP3; 'V':WAV*/
  uint32_t AudioFormat;    /* 'P':PCM'*/
  uint16_t NbrChannels;
  uint32_t SampleRate;
  uint32_t ByteRate;
  uint16_t BitPerSample;
}Audio_InfoTypeDef;

// src236_static_param_t src236_static_param;
// src441_static_param_t src441_static_param;
// uint32_t src_input_frame_size;
/*
typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
  AUDIO_ERROR_INVALID_VALUE,
  AUDIO_ERROR_CODEC,
  AUDIO_ERROR_SRC,
}AUDIO_ErrorTypeDef;
*/


static void *pSrc236PersistentMem;
static void *pSrc441PersistentMem;
static void *pSrcScratchMem;
/*BUFFERs for Audio Module interface*/

static char SRC236_ratio[8][3]={" 2 "," 3 "," 6 ","1_2","1_3","1_6","3_2","2_3"};

static uint8_t SrcTypeSelected;     /* 236, 441, None */
static uint8_t SrcIterations;

static Audio_InfoTypeDef AudioFormatData;

static src236_static_param_t src236_static_param;
static src441_static_param_t src441_static_param;
static uint32_t src_input_frame_size;
static uint32_t src_scratch_mem_size;
static uint32_t src_error;
static bool lsrc_memory_allocated;
// static uint32_t src_error;
#define WAV_PACKET_SZE_48K                       (480U*2U*2U) //24bit 48K
#define WAV_PACKET_SZE_96K                       (960U*2U*3U) //24bit 96K
#define WAV_PACKET_SZE                           WAV_PACKET_SZE_48K
#define SRC236_FRAMES                            2
#define PCM_TO_SRC_BUFFER_SZE                    (uint32_t)(SRC236_FRAMES * WAV_PACKET_SZE_96K) /*PCM BUFFER WITH 96K LPCM stream*/
#define SRC_TO_USB_BUFFER_SZE                    (uint32_t)(SRC236_FRAMES * WAV_PACKET_SZE)     /* USB BUFFER*/
/* Output double buffer of 960 stereo samples in 
  16-bits representing 10ms frame at: 
  48kHz as (480 * 2 (half + half) * 2 (stereo) * 2 (bytes x sample)) in byte
  24-bits representing 10ms frame at: 
  48kHz as (480 * 2 (half + half) * 2 (stereo) * 3 (bytes x sample)) in byte
*/


#define SELECT_SRC_NONE     0
#define SELECT_SRC_236      1
#define SELECT_SRC_441      2

static uint8_t PCMToSrcTmpBuffer[PCM_TO_SRC_BUFFER_SZE];                     /* SRC_FRAME_SIZE (240>147) * 2 (bytes x sample) * 2 (stereo) */
static uint8_t SrcToUSBusBuffer[SRC_TO_USB_BUFFER_SZE];

static inline void checksamplerate(void){
  switch(AudioFormatData.SampleRate){
    case 8000:
      src236_static_param.src_mode = SRC236_RATIO_6;
      SrcTypeSelected = SELECT_SRC_236;
      SrcIterations = 1;
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations*6);
      break;
    case 16000:
      src236_static_param.src_mode = SRC236_RATIO_3;
      SrcTypeSelected = SELECT_SRC_236;
      SrcIterations = 1;
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations*3);
      break;
    case 24000:
      src236_static_param.src_mode = SRC236_RATIO_2;
      SrcTypeSelected = SELECT_SRC_236;
      SrcIterations = 1;
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations*2);
      break;
    case 32000:
      src236_static_param.src_mode = SRC236_RATIO_3_2;
      SrcTypeSelected = SELECT_SRC_236;
      SrcIterations = 2;   /* frame size smaller but processing repeated 2 times */
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations*3/2);
      break;
    case 44100:
      /* src441_static_param does not have params to be configured */
      SrcTypeSelected = SELECT_SRC_441;
      SrcIterations = 3;   /* frame size smaller but processing repeated 3 times */
      src_input_frame_size = (SRC_TO_USB_BUFFER_SZE/480)*441/(8*SrcIterations);
      break;
    case 48000:
      SrcTypeSelected = SELECT_SRC_NONE;
      SrcIterations = 2;   /* frame size smaller but processing repeated 2 times considering SRC236 input req.*/
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations); /* half buff, stereo, byte x sample */
      break;
    case 96000:
      src236_static_param.src_mode = SRC236_RATIO_1_2;
      SrcTypeSelected = SELECT_SRC_236;
      SrcIterations = 4;  /* frame size smaller but processing repeated 4 times */
      src_input_frame_size = SRC_TO_USB_BUFFER_SZE/(8*SrcIterations*1/2);
      break;
    default:
      // AudioState = AUDIO_STATE_STOP;
      return;
    }  

    UNUSED(src236_static_param);
    UNUSED(src441_static_param);
    UNUSED(SRC236_ratio);
    UNUSED(pOutputBuffer);
    UNUSED(pInputBuffer);

}

static inline void init_src_library(void){
  pSrc236PersistentMem = malloc(src236_persistent_mem_size);  /* 972 bytes  */
  pSrc441PersistentMem = malloc(src441_persistent_mem_size);  /* 0x0E8: 232 bytes */ 
  if ((pSrc236PersistentMem == NULL) || (pSrc441PersistentMem == NULL))
  {
    free (pSrc236PersistentMem);
    pSrc236PersistentMem = NULL;
    free (pSrc441PersistentMem);
    pSrc441PersistentMem = NULL;
    while(1);
  }
  if (src236_scratch_mem_size > src441_scratch_mem_size)
  {
    src_scratch_mem_size = src236_scratch_mem_size;    /* 0x784:1924 */
  }
  else
  {
    src_scratch_mem_size = src441_scratch_mem_size;    /* 0xC9C: 3228*/
  }  
  pSrcScratchMem = malloc(src_scratch_mem_size);      /* 3364 bytes*/
  if (pSrcScratchMem == NULL)
  {
    free (pSrcScratchMem);
    pSrcScratchMem = NULL;
    while(1);
  } 
  src236_static_param.src_mode = SRC236_RATIO_1_2;
  
  lsrc_memory_allocated = true;
}

static inline void prepare_src_points(void){
 if (SrcTypeSelected == SELECT_SRC_236)
    {
      /* SRC236 effect reset */
      src_error = src236_reset(pSrc236PersistentMem, pSrcScratchMem);
      if (src_error != SRC236_ERROR_NONE)
      {
        return;
        // return (AUDIO_ERROR_SRC);
      }
      /* SRC236 effect static parameters setting */
      src_error = src236_setParam(&src236_static_param, pSrc236PersistentMem);
      if (src_error != SRC236_ERROR_NONE)
      {
        return ; 
        //  (AUDIO_ERROR_SRC);
      }
    }

    if (SrcTypeSelected == SELECT_SRC_441)
    {
      /* SRC236 effect reset */
      src_error = src441_reset(pSrc441PersistentMem, pSrcScratchMem);
      if (src_error != SRC441_ERROR_NONE)
      {
        return ;
        // (AUDIO_ERROR_SRC);
      }
      /* SRC236 effect static parameters setting */
      src_error = src441_setParam(&src441_static_param, pSrc441PersistentMem);
      if (src_error != SRC441_ERROR_NONE)
      {
        return;
        //  (AUDIO_ERROR_SRC);
      }
    }
}
/*
// MemPool: RAM dynamic memory allocation,
// Allocate SRAM memory MemPool structure
  hAudio.pMemPool = (FWK_MEMPOOL_t*) malloc(sizeof(FWK_MEMPOOL_t)); // 0x10: 16

  if (hAudio.pMemPool == NULL)
  {
    free(hAudio.pMemPool);
    hAudio.pMemPool = NULL;
    while(1);
  }
  // Allocate SRAM memory for MemPool Buffer: at least 3 times Max output audio decoder packet
  hAudio.pMemPool->pBuff = (uint8_t*) malloc(MEMPOOLBUFSZE);    // 0x9000: 36864
  if(hAudio.pMemPool->pBuff == NULL)
  {
    free(hAudio.pMemPool->pBuff);
    hAudio.pMemPool->pBuff = NULL;
    while(1);
  }
  FWK_MEMPOOL_Init(hAudio.pMemPool, MEMPOOLBUFSZE);
 
*/

static inline void initbuffers(void){
  /* Buffers used for audio module interface after MemPool */

    InputBuffer.nb_bytes_per_Sample = 3;            /* 8 bits in 0ne byte 24bit in one sample*/
    InputBuffer.nb_channels = 2;                    // AudioFormatData.NbrChannels;
    InputBuffer.mode = INTERLEAVED;
    InputBuffer.buffer_size = SRC236_FRAMES;                  // in number of samples 3*2*120
    InputBuffer.data_ptr = &PCMToSrcTmpBuffer;

    OutputBuffer.nb_bytes_per_Sample = 3; /* 8 bits in 0ne byte */
    OutputBuffer.nb_channels = 2; //AudioFormatData.NbrChannels;
    OutputBuffer.mode = INTERLEAVED;
    OutputBuffer.buffer_size = SRC236_FRAMES; /* half buff of stereo samples in bytes */  
    OutputBuffer.data_ptr = &SrcToUSBusBuffer;
}

static inline void resampletask(void* srcbuff[], void* dstbuff[], uint16_t src_buff_sz, uint16_t dst_buff_sz ){
  SrcTypeSelected = SELECT_SRC_236;
  if(!lsrc_memory_allocated){
    init_src_library();
    prepare_src_points();
  }
  initbuffers();

  memset(&PCMToSrcTmpBuffer, 0x00, PCM_TO_SRC_BUFFER_SZE);
  memset(&SrcToUSBusBuffer, 0x00, SRC_TO_USB_BUFFER_SZE);
  memcpy(&PCMToSrcTmpBuffer, srcbuff, 720);
  
  int32_t resamplestatus = src236_process(pInputBuffer, pOutputBuffer, pSrc236PersistentMem);
  if(resamplestatus == 0){
    memset(&PCMToSrcTmpBuffer, 0x00, PCM_TO_SRC_BUFFER_SZE);    
  }
  UNUSED(resamplestatus);
  UNUSED(srcbuff);
  UNUSED(dstbuff);
  UNUSED(src_buff_sz);
  UNUSED(dst_buff_sz);
}

  // When new data arrived, copy data from speaker buffer, to microphone buffer
  // and send it over
  // Only support speaker & headphone both have the same resolution
  // If one is 16bit another is 24bit be care of LOUD noise !
  /*
  if (spk_data_size)
  {
    if (current_resolution == 16)
    {
      int16_t *src = (int16_t*)spk_buf;
      int16_t *limit = (int16_t*)spk_buf + spk_data_size / 2;
      int16_t *dst = (int16_t*)mic_buf;
      while (src < limit)
      {
        // Combine two channels into one
        int32_t left = *src++;
        int32_t right = *src++;
        *dst++ = (int16_t) ((left >> 1) + (right >> 1));
      }
      // tud_audio_write((uint8_t *)mic_buf, (uint16_t) (spk_data_size / 2));
      spk_data_size = 0;
    }
    else if (current_resolution == 24)
    {
      int32_t *src = spk_buf;
      int32_t *limit = spk_buf + spk_data_size / 4;
      int32_t *dst = mic_buf;
      while (src < limit)
      {
        // Combine two channels into one
        int32_t left = *src++;
        int32_t right = *src++;
        *dst++ = (int32_t) ((uint32_t) ((left >> 1) + (right >> 1)) & 0xffffff00ul);
      }
      // tud_audio_write((uint8_t *)(mic_buf), (uint16_t) (spk_data_size / 2));
      spk_data_size = 0;
    }    
  }
  */
/**
 * @brief convert one 16bit i2s sample to 16bit pcm sample
 * @brief  I2SPHILLIPS-->USB PCM 24bit 3bytes/per sample
 * incoming I2S audio data buffer : uint16_t array
 * incoming I2S Philips data format is : left-aligned 24bits in 32bit frame, MSbyte first
 * STM32 I2S peripheral uses a 16bit data register
 * => incomiing I2S receved data buffer : uint16_t array
 * Each I2S stereo sample is encoded as {hi_L:mid_L}, {lo_L:0x00}, {hi_R:mid_R}, {lo_R:0x00}
 * In USB PCM Each 24bit stereo sample is encoded as : L channel 3bytes + R channel 3bytes, LSbyte first
 * b0:lo_L, b1:mid_L, b2:hi_L, b3:lo_R, b4:mid_R, b5:hi_R
 * => outgoing USB PCM transfer data buffer : uint16_t array
 * mirroring:
 *          0     1   2     3    4     5   6     7
 * input  hi_L,mid_L,0x00,lo_L,hi_R,mid_R,0x00,lo_R as 2 16bit word (2 uint16_t or 4 uint8_t  8 working bits, +2 0x0 bits =>8bits total) MSbyte first LSByteLast as one 24bit frame encoded to 32 bit word
 * input 3 and 6 byte valued as 0x00 or 0x03 (sign?) eqal it 3 byte in LChanel 4bit and R Chanel 4bit
 *          0     1    2    3     4    5
 * output lo_L,mid_L,hi_L,lo_R,mid_R,hi_R   LSByte first, MSByte Last 2x3bytes (6 workings bits => 6bits total)
 * 
 * volume control is implemented by scaling the data, attenuation resolution is 3dB.
 * 6dB is equivalent to a shift right by 1 bit.
 * @param src - source buffer
 * @param dst - destination buffer
 * @param nBytes - bytes to convert in input buffer (in bytes!!!!)
*/

// #pragma message ("Testing this")
// #pragma warning ("WARNING")
// #pragma message("move this buffer to dedicated memory area")
// static CFG_TUSB_MEM_ALIGN uint8_t __mic_tmpbuf[(MIC_TOTAL_DMA_BUF_SIZE)];

/* 
 * @brief mirroring:
 *          0     1   2     3    4     5   6     7
 * input  hi_L,mid_L,0x00,lo_L,hi_R,mid_R,0x00,lo_R as 2 16bit word (6 working bits, +2 0x0 bits =>8bits total) MSbyte first
 *          0     1    2    3     4    5
 * output lo_L,mid_L,hi_L,lo_R,mid_R,hi_R   LSByte first, MSByte Last 2x3bytes (6 workings bits => 6bits total)
 */


/*
Example call
void REC_ProcessDMA(void)
{
    static uint16_t i;

    if (current_buf == 0)
    {
        if (i < SAMPLES_BUF_SIZE)
        {
            samples_buf_0[i] = REC_ProcessSample((uint16_t *)i2s_samples);
            i++;
        }
        else
        {
            i = 0;
            current_buf = 1;
            samples_ready = 1;
            last_buf_avg = samples_avg / SAMPLES_BUF_SIZE;
            samples_avg = 0;
        }
        
    }
    else
    {
        if (i < SAMPLES_BUF_SIZE)
        {
            samples_buf_1[i] = REC_ProcessSample((uint16_t*)i2s_samples);
            i++;
        }
        else
        {
            i = 0;
            current_buf = 0;
            samples_ready = 1;
            last_buf_avg = samples_avg / SAMPLES_BUF_SIZE;
            samples_avg = 0; 
        }
        
    }
}



/*
uint16_t bad2_convert_3b_i2s24_PCM24(uint8_t src[], uint8_t dst[], uint32_t length){
    int32_t int_left, int_right;
    float float_left, float_right;
    uint16_t dst_ptr = 0;
    for(uint16_t i = 0; i< length; i+=4){  
        int_left = (int32_t) (((uint32_t)src[i+0] << 8) | (uint32_t)src[i+1] >> 8);
        if(int_left & (1<<23)) int_left|= 0xFF000000;
        float_left = ((float) int_left); // преобразуем во float32_t

        int_right = (int32_t) (((uint32_t)src[i+2] << 8) | (uint32_t)src[i+3] >> 8);
        if(int_right & (1<<23)) int_right|= 0xFF000000;
        float_right = ((float) int_right); // преобразуем во float32_t        
        // *(&dst+dst_ptr++) = (void*)int_left;
        // *(&dst+dst_ptr++) = (void*)int_right; 
        dst[dst_ptr]    = BYTE_0(int_left) ;
        dst[++dst_ptr]  = BYTE_1(int_left) ;
        dst[++dst_ptr]  = BYTE_0(int_right) ;
        dst[++dst_ptr]  = BYTE_1(int_right) ;

    }
    UNUSED(float_left);
    UNUSED(float_right);
    return ((uint16_t)(dst_ptr));

}
// Process 24 bit sample to 16 bit
int16_t REC_ProcessSample(uint16_t *samples)
{
#define gain 0x01U    
    int32_t full_sample = (((int32_t)samples[0] << 16) | samples[1]);
    uint8_t sign = samples[0] >> 15;
    
    full_sample = full_sample << gain;
    int16_t sample = (sign << 15) ;

    int16_t temp_sample = 0x7FFF & (full_sample >> 16);

//     samples_avg += abs(sample | temp_sample);

    return sample | temp_sample;

}
*/

