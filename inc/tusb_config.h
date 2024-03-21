/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ha Thach (tinyusb.org)
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

#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usb_descriptors.h"

//--------------------------------------------------------------------+
// Board Specific Configuration
//--------------------------------------------------------------------+

// RHPort number used for device can be defined by board.mk, default to port 0
#ifndef BOARD_TUD_RHPORT
#define BOARD_TUD_RHPORT      0
#endif

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUD_MAX_SPEED
// #define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#define BOARD_TUD_MAX_SPEED   OPT_MODE_FULL_SPEED
#endif

//--------------------------------------------------------------------
// Common Configuration
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
// #error CFG_TUSB_MCU must be defined
#define CFG_TUSB_MCU OPT_MCU_STM32F4
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS           OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG        0
#endif

// Enable Device stack
#define CFG_TUD_ENABLED       1

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED     BOARD_TUD_MAX_SPEED

/* USB DMA on some MCUs can only access a specific SRAM region with restriction on alignment.
 * Tinyusb use follows macros to declare transferring memory so that they can be put
 * into those specific section.
 * e.g
 * - CFG_TUSB_MEM SECTION : __attribute__ (( section(".usb_ram") ))
 * - CFG_TUSB_MEM_ALIGN   : __attribute__ ((aligned(4)))
 */
#ifndef CFG_TUSB_MEM_ALIGN
#define CFG_TUSB_MEM_ALIGN        __attribute__ ((aligned(4)))
#endif

#ifndef CFG_TUSB_MEM_SECTION
#define CFG_TUSB_MEM_SECTION     __attribute__ (( section(".usb_ram") ))
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


//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE    64
#endif

//------------- CLASS -------------//
#define CFG_TUD_CDC               0
#define CFG_TUD_MSC               0
#define CFG_TUD_HID               0
#define CFG_TUD_MIDI              0
#define CFG_TUD_AUDIO             1
#define CFG_TUD_VENDOR            0

//--------------------------------------------------------------------
// AUDIO CLASS DRIVER CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUD_AUDIO_FUNC_1_DESC_LEN                                TUD_AUDIO_HEADSET_STEREO_DESC_LEN

// How many formats are used, need to adjust USB descriptor if changed
#define CFG_TUD_AUDIO_FUNC_1_N_FORMATS                               2

// Audio format type I specifications
#if defined(__RX__)
#define CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE                         48000     /*// 16bit/48kHz is the best quality for Renesas RX*/
#else
#define CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE                         48000     /*// 24bit/96kHz is the best quality for full-speed, high-speed is needed beyond this*/
#endif
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX                           2
#define CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX                           2



// 16bit in 16bit slots
//spk conf1
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX                  24
// mic conf1
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX                  24
//spk conf2
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_TX                  24
//mic conf2
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX                  24

/*
// 16bit in 32bit slots
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX                  16
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX          3
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX                  16
*/

/*
// 24bit in 16bit slots
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_TX                  24
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_RESOLUTION_RX                  24
*/
#if defined(__RX__)
// 8bit in 8bit slots
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX          1
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_TX                  8
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX          1
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX                  8
#else

/*
** 32bit in 32bit slots
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_TX                  32
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX          4
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_RESOLUTION_RX                  32
#endif
*/
#endif

// EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense)
#define CFG_TUD_AUDIO_ENABLE_EP_IN                1

#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_IN    TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_IN    TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_TX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)

#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SW_BUF_SZ      (TU_MAX(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_IN, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_IN)*2)
#define CFG_TUD_AUDIO_FUNC_1_EP_IN_SZ_MAX         TU_MAX(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_IN, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_IN) // Maximum EP IN size for all AS alternate settings used

// EP and buffer size - for isochronous EP´s, the buffer and EP size are equal (different sizes would not make sense)
#define CFG_TUD_AUDIO_ENABLE_EP_OUT               1

#define CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_OUT    TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_1_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX)
#define CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_OUT    TUD_AUDIO_EP_SIZE(CFG_TUD_AUDIO_FUNC_1_MAX_SAMPLE_RATE, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_N_BYTES_PER_SAMPLE_RX, CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX)

#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SW_BUF_SZ     (TU_MAX(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_OUT, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_OUT)*2)
#define CFG_TUD_AUDIO_FUNC_1_EP_OUT_SZ_MAX        (TU_MAX(CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_OUT, CFG_TUD_AUDIO_FUNC_1_FORMAT_2_EP_SZ_OUT)) // Maximum EP IN size for all AS alternate settings used

// Number of Standard AS Interface Descriptors (4.9.1) defined per audio function - this is required to be able to remember the current alternate settings of these interfaces - We restrict us here to have a constant number for all audio functions (which means this has to be the maximum number of AS interfaces an audio function has and a second audio function with less AS interfaces just wastes a few bytes)
#define CFG_TUD_AUDIO_FUNC_1_N_AS_INT 	          2

// Size of control request buffer
#define CFG_TUD_AUDIO_FUNC_1_CTRL_BUF_SZ	64
// use audio control feedback interrupt(payload 8/64)
#define CFG_TUD_AUDIO_INT_CTR_EPSIZE_IN           64
#define CFG_TUD_AUDIO_INT_CTR_EPSIZE_OUT          64

/// encoder
#define CFG_TUD_AUDIO_ENABLE_ENCODING                 0
#define CFG_TUD_AUDIO_ENABLE_DECODING                 0

#define USE_LINREAR_BUFFER  0 

#if(CFG_TUD_AUDIO_ENABLE_ENCODING)
#define CFG_TUD_AUDIO_ENABLE_TYPE_I_ENCODING          1
#define CFG_TUD_AUDIO_FUNC_1_CHANNEL_PER_FIFO_TX      (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_TX)       // One I2S stream contains two channels, each stream is saved within one support FIFO 
#define CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO        1
#define CFG_TUD_AUDIO_FUNC_1_TX_SUPP_SW_FIFO_SZ        ((BOARD_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED ? 32 : 4) * (CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_OUT / CFG_TUD_AUDIO_FUNC_1_N_TX_SUPP_SW_FIFO))
#else
#endif
/*
#if(USE_LINREAR_BUFFER)
#define USE_LINREAR_BUFFER_TX 1
#endif
#endif
*/

#if(CFG_TUD_AUDIO_ENABLE_DECODING)
#define CFG_TUD_AUDIO_ENABLE_TYPE_I_DECODING          1
#define CFG_TUD_AUDIO_FUNC_1_CHANNEL_PER_FIFO_RX      (CFG_TUD_AUDIO_FUNC_1_N_CHANNELS_RX)         // One I2S stream contains two channels, each stream is saved within one support FIFO 
#define CFG_TUD_AUDIO_FUNC_1_N_RX_SUPP_SW_FIFO        1 
#define CFG_TUD_AUDIO_FUNC_1_RX_SUPP_SW_FIFO_SZ       ((BOARD_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED ? 32 : 4) * (CFG_TUD_AUDIO_FUNC_1_FORMAT_1_EP_SZ_IN / CFG_TUD_AUDIO_FUNC_1_N_RX_SUPP_SW_FIFO))
#else
#if(USE_LINREAR_BUFFER)
#define USE_LINREAR_BUFFER_RX 1
#endif
#endif

//feedback
#define CFG_TUD_AUDIO_ENABLE_FEEDBACK_EP 0
#if (CFG_TUD_AUDIO_ENABLE_FEEDBACK_EP)
#define CFG_TUD_AUDIO_ENABLE_FEEDBACK_FORMAT_CORRECTION 1   /*// AUTO CORRECTION BY SOF FB POINT*/
#endif

//flowcontrol
#define CFG_TUD_AUDIO_EP_IN_FLOW_CONTROL          1
#define CFG_TUD_AUDIO_EP_OUT_FLOW_CONTROL         0
#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */
