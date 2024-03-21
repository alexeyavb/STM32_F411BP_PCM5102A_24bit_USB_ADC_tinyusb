/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jerzy Kasenberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "i2s.h"
#include "i2s_ex.h"
#include "i2s_dma.h"
#include "tim.h"
#include "tim_ex.h"
#include "pcm2iis.h"
#include "iis2pcm.h"
#include "memory.h"
// #pragma message("set prerprtocessotr definition for library")
#include "audio_fw_glo.h"
#include "src236_glo.h"
#include "src441_glo.h"


/*
* lresample include
#include "libresample.h"
*/

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTOTYPES
//--------------------------------------------------------------------+

// List of supported sample rates
#if defined(__RX__)
  const uint32_t sample_rates[] = {44100, 48000};
#else
  // const uint32_t sample_rates[] = {44100, 48000, 88200, 96000};
  const uint32_t sample_rates[] = {48000};
  // const uint32_t sample_rates[] = {48000};
  
#endif

uint32_t current_sample_rate  = 48000;

#define N_SAMPLE_RATES  TU_ARRAY_SIZE(sample_rates)

/* Blink pattern
 * - 25 ms   : streaming data
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum
{
  BLINK_STREAMING = 25,
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

enum
{
  VOLUME_CTRL_0_DB = 0,
  VOLUME_CTRL_10_DB = 2560,
  VOLUME_CTRL_20_DB = 5120,
  VOLUME_CTRL_30_DB = 7680,
  VOLUME_CTRL_40_DB = 10240,
  VOLUME_CTRL_50_DB = 12800,
  VOLUME_CTRL_60_DB = 15360,
  VOLUME_CTRL_70_DB = 17920,
  VOLUME_CTRL_80_DB = 20480,
  VOLUME_CTRL_90_DB = 23040,
  VOLUME_CTRL_100_DB = 25600,
  VOLUME_CTRL_SILENCE = 0x8000,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

// Audio controls
// Current states  
int8_t mute[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];       // +1 for master channel 0
int16_t volume[CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX + 1];    // +1 for master channel 0

#define SPK_BUFF_SIZE ((CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ)*1)
// int32_t spk_buf[SPK_BUFF_SIZE];
CFG_TUD_MEM_ALIGN CFG_BOARD_MEM_SPK_SECTION uint16_t spk_buf[SPK_BUFF_SIZE];
// uint16_t CFG_TUD_MEM_ALIGN mic_buf[SPK_BUFF_SIZE];

// Speaker data size received in the last frame
int spk_data_size;
uint16_t samples_rcvd;


// Resolution per format
// const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX,
//                                                                         CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX,
//                                                                         CFG_TUD_AUDIO_FUNC_1_FORMAT_3_RESOLUTION_RX,
//                                                                         };
// Current resolution, update on format change
const uint8_t resolutions_per_format[CFG_TUD_AUDIO_FUNC_1_N_FORMATS] = {CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX};

uint8_t current_resolution;

void led_blinking_task(void);

void audio_task(void);
extern void ST7735_FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

// working with microphone
static uint16_t FillMicrophoneBufferTask(void* pcm_buff, uint16_t mic_sz);
/*------------- MAIN -------------*/
int main(void)
{
  // HAL_Init();
  board_init();
  I2S_BUS_ClockConfig(&hi2s2, 48000, NULL);

  // init device stack on configured roothub port
  tud_init(BOARD_TUD_RHPORT);
  

  if (board_init_after_tusb) {
    board_init_after_tusb();
  }
  extern void MX_CRC_Init(void);  
  extern void MX_GPIO_Init(void);
  extern void MX_DMA_Init(void);
  // extern void MX_TIM4_Init(void);  
  // extern void MX_I2S2_Init(void);
  // extern void MX_I2S3_Init(void);
  extern void MX_SPI5_Init(void);
  extern void MX_NVIC_Init(void);
  
  MX_CRC_Init();
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM4_Init();
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_SPI5_Init();
  MX_NVIC_Init();


  TU_LOG1("Headset running\r\n");
  extern void ST7735_Init(void);
  extern void ST7735_FillScreen(uint16_t color);
  extern void InitTop(void);


  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);SetPWM(0);
  ST7735_Init(); ST7735_FillScreen(0x0000);InitTop();SetPWM(11);
  tud_task(); // TinyUSB device task
  

#if(CFG_TUD_AUDIO_ENABLE_EP_IN)   // Speaker
  AUDIO_MUTE_OFF();
  HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)(spk_audio_buffer_A), (AUDIO_TOTAL_BUF_SIZE)/2);

#endif //CFG_TUD_AUDIO_ENABLE_EP_IN

#if(CFG_TUD_AUDIO_ENABLE_EP_IN)   // Mic
    uint16_t mic_sz = returndmabuffersizeinsamples();
    HAL_I2S_Receive_DMA(&hi2s3, mic_audio_buffer_IA, mic_sz);
    UNUSED(mic_sz);
#endif

#define TUD_TASK_DELAY  1000  
  //uint32_t counter = HAL_GetTick();
  while (1)
  { 
    // if((counter++) > TUD_TASK_DELAY){
    //   counter = 0;
      tud_task(); // TinyUSB device task
    // }    
    audio_task();

    if(lMicRcvHalfFlag){
      tud_audio_write((void*)&mic_tmpbuf_MA,returntmpbuffsizeinbytes());
      lMicRcvHalfFlag = false;
    }

    if(lMicRcvCompFlag){
      tud_audio_write((void*)&mic_tmpbuf_MB,returntmpbuffsizeinbytes());
      lMicRcvCompFlag = false;
    }


    led_blinking_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

// Helper for clock get requests
static bool tud_audio_clock_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);

  if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
  {
    if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      TU_LOG1("Clock get current freq %lu\r\n", current_sample_rate);

      audio_control_cur_4_t curf = { (int32_t) tu_htole32(current_sample_rate) };
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &curf, sizeof(curf));
    }
    else if (request->bRequest == AUDIO_CS_REQ_RANGE)
    {
      audio_control_range_4_n_t(N_SAMPLE_RATES) rangef =
      {
        .wNumSubRanges = tu_htole16(N_SAMPLE_RATES)
      };
      TU_LOG1("Clock get %d freq ranges\r\n", N_SAMPLE_RATES);
      for(uint8_t i = 0; i < N_SAMPLE_RATES; i++)
      {
        rangef.subrange[i].bMin = (int32_t) sample_rates[i];
        rangef.subrange[i].bMax = (int32_t) sample_rates[i];
        rangef.subrange[i].bRes = 0;
        TU_LOG1("Range %d (%d, %d, %d)\r\n", i, (int)rangef.subrange[i].bMin, (int)rangef.subrange[i].bMax, (int)rangef.subrange[i].bRes);
      }

      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &rangef, sizeof(rangef));
    }
  }
  else if (request->bControlSelector == AUDIO_CS_CTRL_CLK_VALID &&
           request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t cur_valid = { .bCur = 1 };
    TU_LOG1("Clock get is valid %u\r\n", cur_valid.bCur);
    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_valid, sizeof(cur_valid));
  }
  TU_LOG1("Clock get request not supported, entity = %u, selector = %u, request = %u\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);
  return false;
}

// Helper for clock set requests
static bool tud_audio_clock_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
  (void)rhport;

  TU_ASSERT(request->bEntityID == UAC2_ENTITY_CLOCK);
  TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

  if (request->bControlSelector == AUDIO_CS_CTRL_SAM_FREQ)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_4_t));

    current_sample_rate = (uint32_t) ((audio_control_cur_4_t const *)buf)->bCur;

    TU_LOG1("Clock set current freq: %ld\r\n", current_sample_rate);

    return true;
  }
  else
  {
    TU_LOG1("Clock set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

// Helper for feature unit get requests
static bool tud_audio_feature_unit_get_request(uint8_t rhport, audio_control_request_t const *request)
{
  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE && request->bRequest == AUDIO_CS_REQ_CUR)
  {
    audio_control_cur_1_t mute1 = { .bCur = mute[request->bChannelNumber] };
    TU_LOG1("Get channel %u mute %d\r\n", request->bChannelNumber, mute1.bCur);
    return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &mute1, sizeof(mute1));
  }
  else if (UAC2_ENTITY_SPK_FEATURE_UNIT && request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
  {
    if (request->bRequest == AUDIO_CS_REQ_RANGE)
    {
      audio_control_range_2_n_t(1) range_vol = {
        .wNumSubRanges = tu_htole16(1),
        .subrange[0] = { .bMin = tu_htole16(-VOLUME_CTRL_50_DB), tu_htole16(VOLUME_CTRL_0_DB), tu_htole16(256) }
      };
      TU_LOG1("Get channel %u volume range (%d, %d, %u) dB\r\n", request->bChannelNumber,
              range_vol.subrange[0].bMin / 256, range_vol.subrange[0].bMax / 256, range_vol.subrange[0].bRes / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &range_vol, sizeof(range_vol));
    }
    else if (request->bRequest == AUDIO_CS_REQ_CUR)
    {
      audio_control_cur_2_t cur_vol = { .bCur = tu_htole16(volume[request->bChannelNumber]) };
      TU_LOG1("Get channel %u volume %d dB\r\n", request->bChannelNumber, cur_vol.bCur / 256);
      return tud_audio_buffer_and_schedule_control_xfer(rhport, (tusb_control_request_t const *)request, &cur_vol, sizeof(cur_vol));
    }
  }
  TU_LOG1("Feature unit get request not supported, entity = %u, selector = %u, request = %u\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);

  return false;
}

// Helper for feature unit set requests
static bool tud_audio_feature_unit_set_request(uint8_t rhport, audio_control_request_t const *request, uint8_t const *buf)
{
  (void)rhport;

  TU_ASSERT(request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT);
  TU_VERIFY(request->bRequest == AUDIO_CS_REQ_CUR);

  if (request->bControlSelector == AUDIO_FU_CTRL_MUTE)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_1_t));

    mute[request->bChannelNumber] = ((audio_control_cur_1_t const *)buf)->bCur;

    TU_LOG1("Set channel %d Mute: %d\r\n", request->bChannelNumber, mute[request->bChannelNumber]);

    return true;
  }
  else if (request->bControlSelector == AUDIO_FU_CTRL_VOLUME)
  {
    TU_VERIFY(request->wLength == sizeof(audio_control_cur_2_t));

    volume[request->bChannelNumber] = ((audio_control_cur_2_t const *)buf)->bCur;

    TU_LOG1("Set channel %d volume: %d dB\r\n", request->bChannelNumber, volume[request->bChannelNumber] / 256);

    return true;
  }
  else
  {
    TU_LOG1("Feature unit set request not supported, entity = %u, selector = %u, request = %u\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
    return false;
  }
}

//--------------------------------------------------------------------+
// Application Callback API Implementations
//--------------------------------------------------------------------+

// Invoked when audio class specific get request received for an entity
bool tud_audio_get_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request)
{
  audio_control_request_t const *request = (audio_control_request_t const *)p_request;

  if (request->bEntityID == UAC2_ENTITY_CLOCK)
    return tud_audio_clock_get_request(rhport, request);
  if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
    return tud_audio_feature_unit_get_request(rhport, request);
  else
  {
    TU_LOG1("Get request not handled, entity = %d, selector = %d, request = %d\r\n",
            request->bEntityID, request->bControlSelector, request->bRequest);
  }
  return false;
}

// Invoked when audio class specific set request received for an entity
bool tud_audio_set_req_entity_cb(uint8_t rhport, tusb_control_request_t const *p_request, uint8_t *buf)
{
  audio_control_request_t const *request = (audio_control_request_t const *)p_request;

  if (request->bEntityID == UAC2_ENTITY_SPK_FEATURE_UNIT)
    return tud_audio_feature_unit_set_request(rhport, request, buf);
  if (request->bEntityID == UAC2_ENTITY_CLOCK)
    return tud_audio_clock_set_request(rhport, request, buf);
  TU_LOG1("Set request not handled, entity = %d, selector = %d, request = %d\r\n",
          request->bEntityID, request->bControlSelector, request->bRequest);

  return false;
}

bool tud_audio_set_itf_close_EP_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void)rhport;

  uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
  uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

  if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt == 0)
      blink_interval_ms = BLINK_MOUNTED;

  return true;
}

bool tud_audio_set_itf_cb(uint8_t rhport, tusb_control_request_t const * p_request)
{
  (void)rhport;
  uint8_t const itf = tu_u16_low(tu_le16toh(p_request->wIndex));
  uint8_t const alt = tu_u16_low(tu_le16toh(p_request->wValue));

  TU_LOG2("Set interface %d alt %d\r\n", itf, alt);
  if (ITF_NUM_AUDIO_STREAMING_SPK == itf && alt != 0)
      blink_interval_ms = BLINK_STREAMING;

  // Clear buffer when streaming format is changed
  spk_data_size = 0;
  if(alt != 0)
  {
    current_resolution = resolutions_per_format[alt-1];
  }

  return true;
}

#if(CFG_TUD_AUDIO_ENABLE_EP_IN) // Speaker
bool tud_audio_rx_done_pre_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting)
{
  (void)rhport;
  (void)n_bytes_received;
  (void)func_id;
  (void)ep_out;
  (void)cur_alt_setting;
  spk_data_size = tud_audio_read(spk_buf, n_bytes_received);
  samples_rcvd = convert_3b_pcm24_i2s24((void*) spk_buf, spk_audio_buffer_A, n_bytes_received);
  if(lFlagPlayReady){
    HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)spk_audio_buffer_A, samples_rcvd);
    lFlagPlayReady = false;
  }
  return true;
}

bool tud_audio_rx_done_post_read_cb(uint8_t rhport, uint16_t n_bytes_received, uint8_t func_id, uint8_t ep_out, uint8_t cur_alt_setting){  
  (void)rhport;
  (void)func_id;
  (void)ep_out;
  (void)cur_alt_setting;
  (void)n_bytes_received;
  return true;
}
#endif

#if(CFG_TUD_AUDIO_ENABLE_EP_OUT) // Mic
// static CFG_TUSB_MEM_ALIGN CFG_BOARD_MEM_MIC_CONVERTER_SECTION uint8_t __mic_cnv_buff[MIC_TOTAL_DMA_BUF_SIZE];
bool tud_audio_tx_done_pre_load_cb(uint8_t rhport, uint8_t itf, uint8_t ep_in, uint8_t cur_alt_setting){
  {
    (void)rhport;
    (void)itf;
    (void)ep_in;
    (void)cur_alt_setting;  
  }
  HAL_I2S_Receive_DMA(&hi2s2, mic_audio_buffer_IA, MIC_TOTAL_DMA_BUF_SIZE);
  return true;
}

bool tud_audio_tx_done_post_load_cb(uint8_t rhport, uint16_t n_bytes_copied, uint8_t func_id, uint8_t ep_in, uint8_t cur_alt_setting){
  (void)rhport;
  (void)n_bytes_copied;
  (void)func_id;
  (void)ep_in;
  (void)cur_alt_setting;

  return true;
}
#endif

#if(CFG_TUD_AUDIO_ENABLE_FEEDBACK_EP)
  void tud_audio_feedback_params_cb(uint8_t func_id, uint8_t alt_itf, audio_feedback_params_t* feedback_param)
  {
    (void)func_id;
    (void)alt_itf;
    // Set feedback method to fifo counting
    feedback_param->method = AUDIO_FEEDBACK_METHOD_FREQUENCY_FIXED;
    feedback_param->sample_freq = current_sample_rate;
  }
#endif

#if (CFG_TUD_AUDIO_ENABLE_EP_OUT) && (CFG_TUD_AUDIO_ENABLE_FEEDBACK_EP)
void tud_audio_fb_done_cb(uint8_t func_id){
  UNUSED(func_id)
  return;
}
void tud_audio_feedback_interval_isr(uint8_t func_id, uint32_t frame_number, uint8_t interval_shift){
  UNUSED(func_id) ;
  UNUSED(frame_number);
  UNUSED(interval_shift);
  return ;
}
#endif

#if(CFG_TUD_AUDIO_INT_CTR_EPSIZE_IN)
bool tud_audio_int_ctr_done_cb(uint8_t rhport, uint16_t n_bytes_copied){
  UNUSED(rhport);
  UNUSED(n_bytes_copied);
  return true;
}
#endif
//--------------------------------------------------------------------+
// AUDIO Task
//--------------------------------------------------------------------+

#define MIC_SAMPLES_INTERVAL 1000U
static uint16_t mic_samples_rcvd;
static bool lcur;
void audio_task(void)
{    
  if(lMicRcvCompFlag){
    if(mic_samples_rcvd >= MIC_SAMPLES_INTERVAL){
      mic_samples_rcvd = 0;
      lcur = !lcur;
      ST7735_FillRectangleFast(00,20,10,10,(lcur) ? 0xF0A1 : 0x000F);
    }
    mic_samples_rcvd++;
  }
return;
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;
  
  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms) return;
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state;
}

//--------------------------------------------------------------------+
// extend buffers to .tmp memory section


//--------------------------------------------------------------------+
// HALF DMA BUFFER Callback
void user_spi3rx_halfcplt_callback_method(I2S_HandleTypeDef *hi2s){
  uint32_t resampletaskticks = HAL_GetTick();
  // if(!lMicRcvHalfFlag){      
    {
      uint16_t bytesconverted = 0u;
      uint16_t common_position = 0u;
      uint16_t halfbufferSz = (returndmabuffersizeinsamples())/2;
      uint16_t dstBufferSz = (returntmpbuffsizeinsamples());
      bytesconverted = convert_3b_i2s24_PCM24(mic_audio_buffer_IA, common_position, halfbufferSz, mic_tmpbuf_MA, common_position, dstBufferSz);
      lMicRcvHalfFlag  = true;    
      UNUSED(bytesconverted);
    }
  resampletaskticks = HAL_GetTick()-resampletaskticks;    
  UNUSED(hi2s);
  UNUSED(resampletaskticks);
  return;
}
//--------------------------------------------------------------------+
// FULL DMA BUFFER Callback

void user_spi3rx_cplt_callback_method(I2S_HandleTypeDef *hi2s){
  uint32_t resampletaskticks = HAL_GetTick();
  // if(!lMicRcvCompFlag){
  {
    uint16_t bytesconverted = 0u;
    uint16_t common_position = (returndmabuffersizeinsamples())/2;
    uint16_t bufferSz = (returndmabuffersizeinsamples());
    uint16_t dstBufferSz = (returntmpbuffsizeinsamples());    
    bytesconverted = convert_3b_i2s24_PCM24(mic_audio_buffer_IA, common_position, bufferSz, mic_tmpbuf_MB, 0, dstBufferSz);
    lMicRcvCompFlag = true;   
    UNUSED(bytesconverted); 
  }
  resampletaskticks = HAL_GetTick()-resampletaskticks;    
  UNUSED(hi2s);
  UNUSED(resampletaskticks);
  return;
}


static inline uint16_t FillMicrophoneBufferTask(void* pcm_buff, uint16_t mic_sz){
  uint16_t stat = tud_audio_write(pcm_buff, mic_sz);

  if(!(stat == 0))
    return -1;

  UNUSED(pcm_buff);
  UNUSED(mic_sz);
  return stat;
}

