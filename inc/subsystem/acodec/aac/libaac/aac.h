/*
****************************************************************************
** \file      /home/acmiler/convert_audio/program/faac/include/aac.h
**
** \version   $Id: aac.h 10575 2016-10-13 08:14:24Z dengbiao $
**
** \brief     videc abstraction layer header file.
**
** \attention THIS SAMPLE CODE IS PROVIDED AS IS. GOFORTUNE SEMICONDUCTOR
**            ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**            OMMISSIONS.
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
****************************************************************************
*/

#ifndef __AAC_H__
#define __AAC_H__
//**************************************************************************
//**************************************************************************
//** Defines and Macros
//**************************************************************************
//**************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define aac_printf(args...)               \
        {                                         \
            printf("[libaac][%s:%d]",             \
            __func__, __LINE__);                  \
            printf(args);                 \
        }

#define AAC_DECODE_MIN_STREAMSIZE 768 /* 6144 bits/channel */
#define MAX_CHANNELS 2 /* make this higher to support files with
                          more channels */

#define AAC_DEFAULT_BUFFER_SIZE (AAC_DECODE_MIN_STREAMSIZE*MAX_CHANNELS)// default decode frame size

#define AAC_NORMAL                0
#define AAC_NULL                  -1
#define AAC_ERROR                 -2
#define AAC_FLOW                  -3
#define AAC_ENCODE_FAIL             -4

//**************************************************************************
//**************************************************************************
//** Enumerated types
//**************************************************************************
//**************************************************************************
/*  samplereate enum  */
typedef enum {
	AAC_SAMPLE_RATE_DEFAULT = 0,
	AAC_SAMPLE_RATE_96000,
	AAC_SAMPLE_RATE_88200,
	AAC_SAMPLE_RATE_64000,
	AAC_SAMPLE_RATE_48000,
	AAC_SAMPLE_RATE_44100,
	AAC_SAMPLE_RATE_32000,
	AAC_SAMPLE_RATE_24000,
	AAC_SAMPLE_RATE_22050,
	AAC_SAMPLE_RATE_16000,
	AAC_SAMPLE_RATE_12000,
	AAC_SAMPLE_RATE_11025,
	AAC_SAMPLE_RATE_8000
} AAC_CONFIG_SampleRateEnumT;

/* library output formats */
typedef enum {
    AAC_FMT_NULL   = 0,
	AAC_FMT_16BIT  = 1,
	AAC_FMT_24BIT  = 2,
	AAC_FMT_32BIT  = 3,
	AAC_FMT_FLOAT  = 4,
	AAC_FMT_FIXED  = 4,
	AAC_FMT_DOUBLE = 5
} AAC_CONFIG_FormatEnumT;

//**************************************************************************
//**************************************************************************
//** Data Structures
//**************************************************************************
//**************************************************************************

typedef void  *AAC_EncodeHandle;

typedef struct {
	long buffer_size;        // buffer max size
    long bytes_into_buffer;  // buffer size of vaild data
    long bytes_consumed;     // buffer size of consumed data
    unsigned char *buffer;   // buffer address
} AAC_BufferT;

typedef struct {
	int channels;      // Pcm channels(mono:1 stereo:2)
	int quantqual;     // Output audio quantaual
	AAC_CONFIG_SampleRateEnumT     samplerates;   // Pcm samplerate
	AAC_CONFIG_FormatEnumT         samplebits;    // Bits of sample point,default 16bit

	unsigned long min_framesize; // [out] size of pcm minimum frame
	unsigned long aac_maxbytes;  // [out] max bytes of aac data

	AAC_BufferT inbuf;  // buffer of fill pcm data
	AAC_BufferT outbuf; // buffer of output aac data

    AAC_EncodeHandle  handle;
} AAC_EncodeInfoT;


typedef void *AAC_DecodeHandle;

typedef struct {
    unsigned long bytesconsumed;
    unsigned long samples;
    unsigned char channels;
    unsigned char error;
    unsigned long samplerate;

    /* SBR: 0: off, 1: on; upsample, 2: on; downsampled, 3: off; upsampled */
    unsigned char sbr;

    /* MPEG-4 ObjectType */
    unsigned char object_type;

    /* AAC header type; MP4 will be signalled as RAW also */
    unsigned char header_type;

    /* multichannel configuration */
    unsigned char num_front_channels;
    unsigned char num_side_channels;
    unsigned char num_back_channels;
    unsigned char num_lfe_channels;
    unsigned char channel_position[64];

    /* PS: 0: off, 1: on */
    unsigned char ps;
} AAC_DecodeFrameInfo;

typedef struct {
	/*  need config  */
	AAC_CONFIG_FormatEnumT      format;        // bits:16bits  24bits  32bit  float
	AAC_CONFIG_SampleRateEnumT  samplerate;    // samplerate:
	                                           //    96000 44100 8000 ...

	/*  return value  */
	unsigned char channels;         //return value; channel num
	unsigned long channel_mask;     //return value; channel mask
	unsigned long real_samplerate;  //return value; real samplate

	/*  need allocate inbuff  */
	AAC_BufferT  inbuf;
	AAC_BufferT  outbuf;

    /*  aac decode parm */
    unsigned char  header_parse;               // 0:off        1:on
    AAC_DecodeHandle          handle;
    AAC_DecodeFrameInfo       info;
} AAC_DecodeInfoT;//aac info;

//**************************************************************************
//**************************************************************************
//** Global Data
//**************************************************************************
//**************************************************************************
//**************************************************************************
//**************************************************************************
//** API Functions
//**************************************************************************
//**************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

extern int  aac_encode_open  (AAC_EncodeInfoT *);
extern int  aac_encoding     (AAC_EncodeInfoT *);
extern int  aac_encode_close (AAC_EncodeInfoT *);

extern int  aac_decode_open  (AAC_DecodeInfoT *);
extern int  aac_decoding     (AAC_DecodeInfoT *);
extern int  aac_decode_close (AAC_DecodeInfoT *);

#ifdef __cplusplus
}
#endif
#endif /* AAC_H */



