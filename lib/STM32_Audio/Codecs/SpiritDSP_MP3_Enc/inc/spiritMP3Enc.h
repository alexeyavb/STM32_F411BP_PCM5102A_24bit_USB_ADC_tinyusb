/*
  ******************************************************************************
  * @file    spiritMP3Enc.h
  * @author  MCD Application Team
  * @brief   This file is header for spiritMP3Enc MP3 encoder library.
  ******************************************************************************
  * Portions COPYRIGHT 2017 STMicroelectronics
  * Portions SPIRIT Audio Engine Copyright (c) 1995-2009, SPIRIT
  *
  * Licensed under ST Image License SLA0047, the "License"; You may not use this
  * file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/SLA0047
  *
  ******************************************************************************
  */


#ifndef __SPIRITMP3ENC_H__
#define __SPIRITMP3ENC_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct TMP3E_persist_tag TMP3E_persist;
typedef struct TMP3E_scratch_tag TMP3E_scratch;


/**
*   MP3 encoder API error codes
*/
typedef enum
{
    /* Success */
    MP3E_STATUS_OK = 0,

    /* Wrong number of channels */
    MP3E_STATUS_BAD_NUM_CH,

    /* Unsupported sample rate */
    MP3E_STATUS_BAD_SAMPLE_RATE,

    /* Unsupported bitrate */
    MP3E_STATUS_BAD_BITRATE

} E_MP3E_STATUS;

/**
    Initialize MP3 encoder module.
    valid initialization values:
    iCh:
        1 or 2
    uiSampleRateHz:
        48000, 44100, 32000 (MPEG-1)
        24000, 22050, 16000 (MPEG-2)
        12000, 11025,  8000 (MPEG-2.5)
    iBitrateKbps:
        32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 for MPEG-1 sampling rates
        8,  16, 24, 32, 40, 48, 56, 64,  80,  96,  112, 128, 144, 160 for MPEG-2 sampling rates
        8,  16, 24, 32, 40, 48, 56, 64 for MPEG-2.5 sampling rates

    If specified bitrate value not given in the table above, the encoder
    produces free-format stream with specified bitrate (may be not supported by
    some decoders).

*/
E_MP3E_STATUS MP3E__Init(
    TMP3E_persist * enc,            /* encoder object */
    unsigned int sample_rate_hz,    /* Sample rate, Hz */
    int bitrate_kbps,               /* Bitrate, kbps */
    int num_ch,                     /* Number of channels */
    unsigned int cutoff_hz          /* Cutoff frequency in Hz  */
);


/**
         Function  encodes  576  audio  samples  for  1  or two channels. Function
    returns  pointer  to  the  coded  data  and coded data size in bytes. Returned
    buffer  belongs  to  encoder  object,  and  must  not  be  de-allocated by the
    application.
         Function may accept stereo data in interleaved or plain format, by proper
    assignment  of  input  parameters.
         Encoder  internally delays and buffers MP3 frame headers, implementing so
    called  "bit  reservoir".  Up  to  8  frames  can  be buffered in the encoder,
    depending on bitrate (smaller bitrate - more chances that frame get buffered).
    Function  may  not  return coded data immediately after the call. In this case
    function  still  returns valid pointer but coded buffer size will be set to 0,
    indicating that no coded data is produced.
         Note  that  for MPEG1 sampling rates 1 MP3 frame consists of two granules
    (576 samples in each granule), so this function will produce data every second
    call (normally). In case of MPEG2 and MPEG2.5 mode, function normally produces
    data after every call.
         If  left  PCM pointer is NULL, function flushes bit reservoir, and output
    all  frame  headers, which was buffered internally. If MPEG-1 frame has only 1
    granule  encoded before flush, one silent granule would be added automatically
    to  complete  the  frame.  Last flushed frame is padded with zeroes to nominal
    size, so total coded file size would match specified bitrate. Note: This flush
    functionality  affects only delayed headers, it does not flush DSP filters. To
    completely  flush  all the data in the encoder, application should call encode
    function twice with zeroed PCM buffer before flushing bit reservoir.
         Bit-reservoir  flush functionality can be used in the middle of encoding,
    such  effectively  disabling  bit  reservoir,  at  a cost of minor performance
    impact  and  significant quality impact. Disabling bit reservoir may be useful
    for  real-time  communication  application.  Please  note  that  in  this case
    application  should use flush only at the end of frame (i.e. after each second
    granule for MPEG-1 sampling rates).
*/
unsigned char * MP3E__Encode576(
    TMP3E_persist * enc,                /* encoder object */
    TMP3E_scratch * scratch,            /* encoder scratch memory */
    const short   * pcm_left,           /* left (or mono) PCM data (576 samples)*/
    const short   * pcm_right,          /* right PCM data (576 samples) */
    int             pcm_interleave,     /* interleave factor for PCM data */
    unsigned int  * sizeof_coded_data   /* [OUT] size of coded data */
);


/**
*   Return sizeof(TMP3E_persist)
*/
int MP3E__SizeofPersistent(void);


/**
*   Return sizeof(TMP3E_scratch)
*/
int MP3E__SizeofScratch(void);


#ifdef __cplusplus
}
#endif

#endif/*__SPIRITMP3ENC_H__*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
