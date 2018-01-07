/******************************************************************************
** \file        adi/test/src/video.c
**
** \brief       ADI layer video(video input/output encode) test.
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2013-2014 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "adi_types.h"
#include "adi_sys.h"
#include "adi_vi.h"
#include "adi_venc.h"
#include "adi_vout.h"
#include "adi_isp.h"


//*****************************************************************************
//*****************************************************************************
//** Local Defines
//*****************************************************************************
//*****************************************************************************
#define GK_VIDEO_MODE(W,H,F,PI) (((PI<<31)&0x80000000)|((F<<24)&0x3F000000)| \
    (((W/4)<<12)&0x00FFF000)|((H/2)&0x00000FFF))

#define VI_INPUT_WIDTH        1920
#define VI_INPUT_HEIGHT       1080
#define VI_INPUT_FPS         25

#define VENC_STREAM1_WIDTH    1920
#define VENC_STREAM1_HEIGHT   1080
#define VENC_STREAM2_WIDTH   720
#define VENC_STREAM2_HEIGHT  576
#define VENC_STREAM3_WIDTH    0           //VENC_STREAM1_WIDTH
#define VENC_STREAM3_HEIGHT   0            //VENC_STREAM1_HEIGHT
#define VENC_STREAM4_WIDTH   1920
#define VENC_STREAM4_HEIGHT  1080


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE 
#define FALSE 0
#endif

//handle
GADI_SYS_HandleT viHandle    = NULL;
GADI_SYS_HandleT voHandle    = NULL;
GADI_SYS_HandleT vencHandle  = NULL;
GADI_SYS_HandleT ispHandle  = NULL;
//ctl
unsigned char record_enable = FALSE;
unsigned char runing_enable = FALSE;
unsigned char snphot_enable = FALSE;
unsigned char ispStart = FALSE;
//vi
static GADI_VI_SettingParamsT   viParams = {
    .resoluMode = 0,//GK_VIDEO_MODE(VI_INPUT_WIDTH, VI_INPUT_HEIGHT, VI_INPUT_FPS, 1),
    .frameRate  = VI_INPUT_FPS,
    .mirrorMode = {0, 0},
};
//vo
static GADI_VOUT_SettingParamsT voParams[GADI_VOUT_NUMBER] = {
    {
        .voutChannel = 0,
        .resoluMode  = 13,//GADI_VOUT_RESOLUTION_HDTV,
        .deviceType  = 3,
    },
    {
        .voutChannel = 1,
        .resoluMode = 1,
        .deviceType = 1,
    },
};

static GADI_VENC_StreamFormatT streamFormats[GADI_VENC_STREAM_NUM] = {
    {
        .streamId   = GADI_VENC_STREAM_FIRST,
        .encodeType = GADI_VENC_TYPE_H264,
        .channelId  = GADI_VENC_CHANNEL_1,
        .flipRotate = 0,
        .width      = VENC_STREAM1_WIDTH,
        .height     = VENC_STREAM1_HEIGHT,
        .xOffset    = 0,
        .yOffset    = 0,
        .fps        = VI_INPUT_FPS,
        .keepAspRat = 0,
    },
    {
        .streamId   = GADI_VENC_STREAM_SECOND,
        .encodeType = GADI_VENC_TYPE_OFF,
        .channelId  = GADI_VENC_CHANNEL_2,
        .flipRotate = 0,
        .width      = VENC_STREAM2_WIDTH,
        .height     = VENC_STREAM2_HEIGHT,
        .xOffset    = 0,
        .yOffset    = 0,
        .fps        = VI_INPUT_FPS,
        .keepAspRat = 0,
    },
    {
        .streamId   = GADI_VENC_STREAM_THIRD,
        .encodeType = GADI_VENC_TYPE_OFF,
        .channelId  = GADI_VENC_CHANNEL_1,
        .flipRotate = 0,
        .width      = VENC_STREAM3_WIDTH,
        .height     = VENC_STREAM3_HEIGHT,
        .xOffset    = 0,
        .yOffset    = 0,
        .fps        = GADI_VENC_FPS_3,
        .keepAspRat = 0,
    },
    {
        .streamId   = GADI_VENC_STREAM_FORTH,
        .encodeType = GADI_VENC_TYPE_MJPEG,
        .channelId  = GADI_VENC_CHANNEL_1,
        .flipRotate = 0,
        .width      = VENC_STREAM4_WIDTH,
        .height     = VENC_STREAM4_HEIGHT,
        .xOffset    = 0,
        .yOffset    = 0,
        .fps        = GADI_VENC_FPS_3,
        .keepAspRat = 0,
    },
};

static GADI_VENC_H264ConfigT   h264Confs[GADI_VENC_STREAM_NUM] = {
    {
        .streamId    = GADI_VENC_STREAM_FIRST,
        .gopM        = 1,
        .gopN        = 50,
        .idrInterval = 1,
        .gopModel    = 0,
        .profile     = 0,//main profile
        .brcMode     = 0,//*0: CBR; 1: VBR
        .cbrAvgBps   = 4000000,
        .vbrMinbps   = 1000000,
        .vbrMaxbps   = 4000000,
        .reEncMode   = 0,
    },
    {
        .streamId    = GADI_VENC_STREAM_SECOND,
        .gopM        = 1,
        .gopN        = 50,
        .idrInterval = 1,
        .gopModel    = 0,
        .profile     = 0,//main profile
        .brcMode     = 0,//*0: CBR; 1: VBR
        .cbrAvgBps   = 2000000,
        .vbrMinbps   = 1000000,
        .vbrMaxbps   = 2000000,
        .reEncMode   = 0,
    },
    {
        .streamId    = GADI_VENC_STREAM_THIRD,
        .gopM        = 1,
        .gopN        = 50,
        .idrInterval = 1,
        .gopModel    = 0,
        .profile     = 0,//main profile
        .brcMode     = 0,//*0: CBR; 1: VBR
        .cbrAvgBps   = 4000000,
        .vbrMinbps   = 1000000,
        .vbrMaxbps   = 4000000,
        .reEncMode   = 0,
    },
    {
        .streamId    = GADI_VENC_STREAM_FORTH,
        .gopM        = 1,
        .gopN        = 50,
        .idrInterval = 1,
        .gopModel    = 0,
        .profile     = 0,//main profile
        .brcMode     = 0,//*0: CBR; 1: VBR
        .cbrAvgBps   = 4000000,
        .vbrMinbps   = 1000000,
        .vbrMaxbps   = 4000000,
        .reEncMode   = 0,
    },
};

static GADI_VENC_H264QpConfigT h264QpConfs[GADI_VENC_STREAM_NUM] = {
    {
      .streamId  = GADI_VENC_STREAM_FIRST,
      .qpMinOnI  = 31,
      .qpMaxOnI  = 51,
      .qpMinOnP  = 31,
      .qpMaxOnP  = 51,
      .qpIWeight = 3,
      .qpPWeight = 3,
      .adaptQp = 2,
    },
    {
      .streamId  = GADI_VENC_STREAM_SECOND,
      .qpMinOnI  = 31,
      .qpMaxOnI  = 51,
      .qpMinOnP  = 31,
      .qpMaxOnP  = 51,
      .qpIWeight = 3,
      .qpPWeight = 3,
      .adaptQp = 2,
    },
    {
      .streamId  = GADI_VENC_STREAM_THIRD,
      .qpMinOnI  = 31,
      .qpMaxOnI  = 51,
      .qpMinOnP  = 31,
      .qpMaxOnP  = 51,
      .qpIWeight = 3,
      .qpPWeight = 3,
      .adaptQp = 2,
    },
    {
      .streamId  = GADI_VENC_STREAM_FORTH,
      .qpMinOnI  = 31,
      .qpMaxOnI  = 51,
      .qpMinOnP  = 31,
      .qpMaxOnP  = 51,
      .qpIWeight = 3,
      .qpPWeight = 3,
      .adaptQp = 2,
    },
};
static GADI_VENC_MjpegConfigT  mjpegConfs[GADI_VENC_STREAM_NUM] = {
    {
        .streamId     = GADI_VENC_STREAM_FIRST,
        .chromaFormat = 1,//only support  YUV 420
        .quality      = 50,
    },
    {
        .streamId     = GADI_VENC_STREAM_SECOND,
        .chromaFormat = 1,//only support  YUV 420
        .quality      = 50,
    },
    {
        .streamId     = GADI_VENC_STREAM_THIRD,
        .chromaFormat = 1,//only support  YUV 420
        .quality      = 50,
    },
    {
        .streamId     = GADI_VENC_STREAM_FORTH,
        .chromaFormat = 1,//only support  YUV 420
        .quality      = 50,
    },
};

static GADI_VENC_ChannelsParamsT chansParams ={
    .chan1Type               = GADI_VENC_CHANNEL_TYPE_ENCODE,
    .chan1Width              = VENC_STREAM1_WIDTH,
    .chan1Height             = VENC_STREAM1_HEIGHT,
    .chan1DeintlcForIntlcVin = 0,
    .chan2Type               = GADI_VENC_CHANNEL_TYPE_OFF,
    .chan2Width              = VENC_STREAM2_WIDTH,
    .chan2Height             = VENC_STREAM2_HEIGHT,
    .chan2DeintlcForIntlcVin = 0,
    .chan3Type               = GADI_VENC_CHANNEL_TYPE_OFF,
    .chan3Width              = 0,
    .chan3Height             = 0,
    .chan3DeintlcForIntlcVin = 0,
    .chan4Type               = GADI_VENC_CHANNEL_TYPE_PREVIEW ,
    .chan4Width              =  1280,//VENC_STREAM1_WIDTH,
    .chan4Height             = 720, //VENC_STREAM1_HEIGHT,
    .chan4DeintlcForIntlcVin = 0,
    .intlcScan               = 0,
};



//*****************************************************************************
//*****************************************************************************
//** Local Functions Declaration
//*****************************************************************************
//*****************************************************************************

int gdm_vout_en(int type)
{
    
    GADI_VOUT_SettingParamsT    voutParams;
    voutParams = voParams[0];
    voutParams.deviceType = type;
    
    //err = gadi_vout_get_params(voHandle,&voutParams);
    
    GADI_ERR err;
    err = gadi_vout_set_params(voHandle, &voutParams);
    if(err != GADI_OK){
        GADI_ERROR("set vout fail\n");
    }
    
    return err;

}


GADI_ERR gdm_venc_init(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_venc_init();

    return retVal;
}

GADI_ERR gdm_venc_open(void)
{
    GADI_ERR retVal = GADI_OK;
    GADI_VENC_OpenParamsT vencOpenParams;
	GADI_VENC_DspMapInfoT info;

    gadi_sys_memset(&vencOpenParams, 0, sizeof(GADI_VENC_OpenParamsT));
    vencOpenParams.viHandle   = viHandle;
    vencOpenParams.voutHandle = voHandle;
    vencHandle = gadi_venc_open(&vencOpenParams, &retVal);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_venc_open error\n");
        return retVal;
    }
    retVal = gadi_venc_map_bsb(vencHandle);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_venc_map_bsb fail\n");
    }

    retVal = gadi_venc_map_dsp(vencHandle, &info);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_venc_map_dsp fail\n");
    }


    return retVal;
}

GADI_ERR gdm_venc_setup(void)
{
    
	GADI_U32 enable;
	GADI_U32 cnt;
    GADI_U32 viWidth;
	GADI_U32 viHeight;
    GADI_U8  encodeType;
	GADI_ERR retVal         = GADI_OK;

    /*video input module: ensure exit preview status.*/
    enable = 0;
    retVal = gadi_vi_enable(viHandle, enable);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_enable error\n");
        return retVal;
    }

    /*check encode streams parameters.*/
    retVal = gadi_vi_get_resolution(viHandle, &viWidth, &viHeight);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_get_resolution error\n");
        return retVal;
    }
    
    /*set encode channels parameters.*/
    retVal = gadi_venc_set_channels_params(vencHandle, &chansParams);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("set channel parameters error.\n");
        return retVal;
    }

    for (cnt = 0; cnt < GADI_VENC_STREAM_NUM; cnt++)
    {
        encodeType = streamFormats[cnt].encodeType; 
        if(encodeType != 0)
        {
            if (gadi_venc_set_stream_format(vencHandle, &streamFormats[cnt]) < 0)
            {
                GADI_ERROR("gadi_venc_set_stream_format error, steamId=%d\n",cnt);
                return -1;
            }
            if(encodeType == GADI_VENC_TYPE_H264)
            {
                if(gadi_venc_set_h264_config(vencHandle, &h264Confs[cnt]) < 0)
                {
                    GADI_ERROR("gadi_venc_set_h264_config error, steamId=%d\n",cnt);
                    return -1;
                }
                h264QpConfs[cnt].streamId = cnt;
                h264QpConfs[cnt].qpMaxOnI = 51;
                h264QpConfs[cnt].qpMaxOnP = 51;
                if(gadi_venc_set_h264_qp_config(vencHandle, &h264QpConfs[cnt]) < 0)
                {
                    GADI_ERROR("gadi_venc_set_h264_qp_config error, steamId=%d\n",cnt);
                    return -1;
                }
            }
            else if(encodeType == GADI_VENC_TYPE_MJPEG)
            {
                if(gadi_venc_set_mjpeg_config(vencHandle, &mjpegConfs[cnt]) < 0)
                {
                    GADI_ERROR("gadi_venc_set_mjpeg_config error, steamId=%d\n",cnt);
                    return -1;
                }
            }
        }
    }

    enable = 1;
    if(gadi_vi_enable(viHandle, enable) < 0)
    {
        GADI_ERROR("gadi_vi_enable error.\n");
        return -1;
    }

   return retVal;
}


static int env_stream_count  = 0;


GADI_ERR gdm_venc_start_encode_stream(GADI_U32 streamId)
{
    GADI_ERR retVal = GADI_OK;
    GADI_U8  encodeType;

    if (streamId < GADI_VENC_STREAM_NUM) {
        encodeType = streamFormats[streamId].encodeType;
        if(encodeType != 0)
        {
            if(gadi_venc_start_stream(vencHandle, streamId) < 0)
            {
                GADI_ERROR("gadi_venc_start_stream error, streams:%d\n", streamId);

                if(gadi_venc_check_encoder_state(vencHandle) < 0)
                {
                    GADI_ERROR("encoder state error.\n");
                }
                return -1;
            }

            //assert(env_stream_count >= 0);
            /* reset 3a algo */
            if(env_stream_count == 0) {
                if(ispStart) {
                    gadi_isp_reset_3a_static(ispHandle);
                }
            }
#if 0
            if(encodeType == GADI_VENC_TYPE_H264){
                rtsp_create_media_stream(streamId,
                    streams[streamId].streamFormat.width * streams[streamId].streamFormat.height);
            }
#endif

            env_stream_count |= 1 << streamId;
        }
    } else {
        GADI_ERROR("No this video encode stream! streamId[0~4]\n");
    }

    return retVal;
}

GADI_ERR gdm_venc_stop_encode_stream(GADI_U32 streamId)
{
    GADI_ERR   retVal =  GADI_OK;
    GADI_U8  encodeType;

    if (streamId < GADI_VENC_STREAM_NUM) {
        encodeType = streamFormats[streamId].encodeType;
        if(encodeType != 0)
        {
            if(gadi_venc_stop_stream(vencHandle, streamId) < 0)
            {
                GADI_ERROR("gadi_venc_stop_stream error, streams:%d\n", streamId);
                return -1;
            }
            GADI_INFO("stop stream[%d]\n",streamId);
            env_stream_count &= ~(1 << streamId);
        }
    }

    //assert(env_stream_count >= 0);

    return retVal;
}

GADI_ERR gdm_venc_close(void)
{
    GADI_ERR retVal = GADI_OK;

    if(env_stream_count != 0){
        int i;
        for(i = 0; i < GADI_VENC_STREAM_NUM; i++){
            gdm_venc_stop_encode_stream(i);
        }
    }

    retVal = gadi_venc_close(vencHandle);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_venc_close error\n");
        return retVal;
    }


    return retVal;
}

GADI_ERR gdm_venc_exit(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_venc_exit();

    return retVal;
}

GADI_ERR gdm_vi_init(void)
{
    
    GADI_ERR retVal = GADI_OK;
    retVal = gadi_vi_init();
    return retVal;
}

GADI_ERR gdm_vi_open(void)
{
    GADI_ERR retVal = GADI_OK;
    viHandle = gadi_vi_open(&retVal);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_open error\n");
        return retVal;
    }

    return retVal;
}

GADI_ERR gdm_vi_setup(void)
{
    GADI_ERR retVal = GADI_OK;

    /*video input module: set video input frame rate and resolution.*/
    if (viParams.resoluMode != 0) {
        viParams.resoluMode = GK_VIDEO_MODE(viParams.resoluMode/10000,
                                            viParams.resoluMode%10000,
                                            viParams.frameRate,
                                            1);
    }

    retVal = gadi_vi_set_params(viHandle, &viParams);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_set_params error\n");
        return retVal;
    }

    retVal = gadi_vi_set_mirror_mode(viHandle, &viParams.mirrorMode);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_set_mirror_mode error\n");
        return retVal;
    }

    return retVal;
}

GADI_ERR gdm_vi_close(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_vi_close(viHandle);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vi_close error\n");
        return retVal;
    }

    return retVal;
}

GADI_ERR gdm_vi_exit(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_vi_exit();

    return retVal;
}


GADI_ERR gdm_vout_init(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_vout_init();

    return retVal;
}

GADI_ERR gdm_vout_exit(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_vout_exit();

    return retVal;
}

GADI_ERR gdm_vout_open(void)
{
    GADI_ERR retVal = GADI_OK;
    GADI_ERROR("test ovHandle:%p\n",voHandle);
    voHandle = gadi_vout_open(&retVal);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vo_open error\n");
        return retVal;
    }
    GADI_ERROR("test ovHandle:%p\n",voHandle);
    
    //set fb channle
    GADI_VOUT_SelectFbParamsT   fbPar;
    
    fbPar.voutChannel = 0;
    fbPar.fbChannel = 0;
    gadi_vout_select_fb(voHandle,&fbPar);

    
    return retVal;
}


GADI_ERR gdm_vout_close(void)
{
    GADI_ERR retVal = GADI_OK;


    retVal = gadi_vout_close(voHandle);
    if(retVal != GADI_OK)
    {
        GADI_ERROR("gadi_vo_close error\n");
        return retVal;
    }

    return retVal;
}


GADI_ERR gdm_vout_setup(void)
{
    GADI_S32 i;
    GADI_ERR retVal = GADI_OK;

    /*video out module: set video output resolution and output device.*/
    for(i = GADI_VOUT_A; i < GADI_VOUT_NUMBER; i++) {
        voParams[i].voutChannel = i;
        if(i == 0){
          //  voParams[i].resoluMode = GADI_VOUT_RESOLUTION_HDTV;
            //voParams[i].deviceType = GADI_VOUT_DEVICE_I80;
        }
        retVal = gadi_vout_set_params(voHandle, &voParams[i]);
        printf("gmd channnel:%d voParams.resoluMode:%d voParams.devicetype:%d\n",
                i,voParams[i].resoluMode,voParams[i].deviceType);
        if(retVal != GADI_OK)
        {
            GADI_ERROR("gadi_vo_set_params error retVal:%d voHandle:%p\n",retVal,
                    voHandle);
            return retVal;
        }
    }
    
    printf("return gmd channnel:%d voParams.resoluMode:%d voParams.devicetype:%d\n",
                i,voParams[i].resoluMode,voParams[i].deviceType);

    return retVal;
}


GADI_ERR gdm_vout_setup_after(void)
{
    GADI_S32 i;
    GADI_ERR retVal = GADI_OK;
    GADI_VOUT_I80_CmdParaT   i80CmdParams;

    /*video out module: set video output resolution and output device.*/
    for(i = GADI_VOUT_A; i < GADI_VOUT_NUMBER; i++) {
        if(voParams[i].deviceType == GADI_VOUT_DEVICE_I80)
        {
            i80CmdParams.voId       = i;
            i80CmdParams.cmdParaNum = 1;//command total
            i80CmdParams.rdParaNum  = 0; //read command total
            i80CmdParams.cmdPara[0] = LCD_WRITE_CMD(0x2c);//I80 write pixel
            retVal =  gadi_vout_set_i80_params(voHandle, &i80CmdParams);
            printf("test++++++++ set I80 re:%d\n",retVal);

            if(retVal != GADI_OK)
            {
               GADI_ERROR("gadi_vout_set_i80_params error\n");
               return retVal;
            }
        }
    }

    return retVal;
}

 int gdm_isp_init(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_isp_init();
    if (retVal != GADI_OK)
    {
        printf("gadi_isp_init() failed! ret = %d\n", retVal);
    }

    return retVal;
}

 int gdm_isp_open(void)
{
    GADI_ERR                retVal = GADI_OK;
    GADI_ISP_OpenParamsT    openParams;
    
    memset((void*)&openParams,0x0,sizeof(GADI_ISP_OpenParamsT));

    openParams.denoiseMode = GADI_ISP_VPS_MERGE_MODE;

    ispHandle = gadi_isp_open(&openParams, &retVal);
    if (ispHandle == NULL || retVal != GADI_OK)
    {
        printf("gadi_isp_open() failed! ret = %d\n", retVal);
    }
    
    


    return retVal;
}


 int gdm_isp_stop(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_isp_stop(ispHandle);
    if (retVal != GADI_OK)
    {
        printf("gadi_isp_stop() failed! ret = %d\n", retVal);
    }
    ispStart = FALSE;

    return retVal;
}

 int gdm_isp_close(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_isp_close(ispHandle);
    if (retVal != GADI_OK)
    {
        printf("gadi_isp_close() failed! ret = %d\n", retVal);
    }

    ispHandle = NULL;

    return retVal;
}

 int gdm_isp_exit(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_isp_exit();
    if (retVal != GADI_OK)
    {
        printf("gadi_isp_exit() failed! ret = %d\n", retVal);
    }

    return retVal;
}

static void isp_nighttoday(GADI_U32 value)
{
    GADI_ISP_AeAttrT    aeAttr;
    GADI_ISP_ContrastAttrT contrastAttr;
    GADI_ISP_SensorModelEnumT sensorModel;

	gadi_isp_get_sensor_model(ispHandle,&sensorModel);

    gadi_isp_set_day_night_mode(1);

    gadi_isp_set_meter_mode(2);

    if(sensorModel == GADI_ISP_SENSOR_IMX122)   //imx222
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = 25;
        aeAttr.gainMax        = 36;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 96;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
//add by WRD
    else if(sensorModel == GADI_ISP_SENSOR_GC0328)   //gc0328
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_25;
        aeAttr.gainMax        = 36;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 72;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
    else if(sensorModel == GADI_ISP_SENSOR_JXH65)   //jxh65
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_25;
        aeAttr.gainMax        = 48;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 64;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
     else if(sensorModel == GADI_ISP_SENSOR_SC1235)   //SC1235
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_25;
        aeAttr.gainMax        = 48;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 64;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
    else if(sensorModel == GADI_ISP_SENSOR_JXK02)   //JXK02
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_15;
        aeAttr.gainMax        = 36;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 64;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
    else if(sensorModel == GADI_ISP_SENSOR_JXK03)   //JXK03
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_15;
        aeAttr.gainMax        = 36;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 64;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
     else if(sensorModel == GADI_ISP_SENSOR_SC3035)   //SC3035
    {
        gadi_isp_get_ae_attr(ispHandle, &aeAttr);
        aeAttr.shutterTimeMin = 8000;//1024;
        aeAttr.shutterTimeMax = GADI_VI_FPS_20;
        aeAttr.gainMax        = 42;
        aeAttr.gainMin        = 1;
        contrastAttr.enableAuto     = 1;
        contrastAttr.manualStrength = 128;
        contrastAttr.autoStrength   = 64;//96;
        gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
        gadi_isp_set_ae_attr(ispHandle, &aeAttr);
    }
    else//others
    {
         gadi_isp_get_ae_attr(ispHandle, &aeAttr);
         aeAttr.shutterTimeMin = 8000;//1024;
         aeAttr.shutterTimeMax = GADI_VI_FPS_25;
         aeAttr.gainMax        = 33;
         aeAttr.gainMin        = 1;
         contrastAttr.enableAuto     = 1;
         contrastAttr.manualStrength = 128;
         contrastAttr.autoStrength   = 64;//96;
         gadi_isp_set_contrast_attr(ispHandle, &contrastAttr);
         gadi_isp_set_ae_attr(ispHandle, &aeAttr);
     }
//end
}


 int gdm_isp_start(void)
{
    GADI_ERR retVal = GADI_OK;

    retVal = gadi_isp_load_param(ispHandle, NULL);
    if (retVal != GADI_OK)
    {
        if (GADI_ISP_ERR_FEATURE_NOT_SUPPORTED != retVal)
            printf("gadi_isp_load_param() failed! ret = %d\n", retVal);
        else {
            printf("Current sensor no support ISP function.\n");
            gdm_isp_close();
            gdm_isp_exit();
            return retVal;
        }
    }

    retVal = gadi_isp_start(ispHandle);
    if (retVal != GADI_OK)
    {
        printf("gadi_isp_start() failed! ret = %d\n", retVal);
    }
    
    isp_nighttoday(0);

    ispStart = TRUE;
    return retVal;
}

char *record_file_name = "./test.264";

void* gdm_read_encode_streams(void *arg)
{
    unsigned char encodingState = FALSE;
    int fd = -1;
    int chn, i;
    GADI_CHN_AttrT  chnAttr;
    GADI_VENC_StreamT stream;
    FILE *fp = NULL;
    
    char* buf[128];
    record_enable = 0;

    FILE *jfp = NULL;

    


    if(record_enable){
        /* open a new file to write */
        remove(record_file_name);
        if ((fp = fopen(record_file_name,"wr+")) == -1){
            printf("open %s failed.\n", record_file_name);
            return NULL;
        }
    }

    while(runing_enable)
    {
        for(i = 0; i < GADI_VENC_STREAM_NUM; i ++){
            chn = i;
            if (gadi_venc_query(vencHandle, chn, &chnAttr) < 0){
                printf("gadi_venc_query chn:%d", chn);
                continue;
            }
            if((chnAttr.state == GADI_VENC_STREAM_STATE_ENCODING) ||
               (chnAttr.state == GADI_VENC_STREAM_STATE_STOPPING)){
                encodingState = TRUE;
                break;
            }
        }

        if( encodingState){
            /*BLOCK read.*/
            if (gadi_venc_get_stream(vencHandle, 0XFF, &stream)<0){
                printf("gadi_venc_get_stream chn:%d failed.", stream.stream_id);
            }
            else{
                /*stream end, stream size & stream addr is invalid.*/
                if(stream.stream_end == 1){
                    printf("gadi_venc_get_stream chn:%d stream end.", stream.stream_id);
                }
                else{
                    if(record_enable){
                        /*record encode stream 0.*/
                        if(stream.stream_id == GADI_VENC_STREAM_FIRST)
                        {
                            if (fwrite(stream.addr,1, stream.size,fp) != stream.size){
                                printf("record: write file error, len=%d\n",stream.size);
                            }
                        }else if(stream.pic_type == 5){
                            
                                        
                            sprintf(buf,"./test_%d.jpeg",stream.frame_num%10);
                            printf("filename:%s\n",buf);
                            jfp  = fopen(buf,"wr+");

                            if(jfp){
                                fwrite(stream.addr,1,stream.size,jfp);
                                fclose(jfp);
                            }else{
                                printf("open file fail \n");
                            }
                        }
                    }
                    else{
                        ;     
                    }
                }
            }
        }else{
            printf("--not in encode status.\n");
            usleep(30000);
        }
    }

    close(fd);
    runing_enable = FALSE;
    return NULL;
}


void sample_kill_handle(int signo)
{
    runing_enable = FALSE;
    /*wait for exit read encode stream thread.*/
    sleep(1);

    gdm_isp_stop();
    //gdm_stop_encode_streams();
    gdm_isp_close();
    //gdm_video_close();
    exit(1);
}


 int sample_exit(void)
{
    int retVal = GADI_OK;

    retVal = gadi_vi_exit();
    retVal = gadi_vout_exit();
    retVal = gadi_venc_exit();
    retVal = gadi_sys_exit();
    retVal = gdm_isp_exit();

    return retVal;
}


static int voutchannel = GADI_VOUT_A;
static int fb_select(void)
{
    GADI_VOUT_SelectFbParamsT fbPar;

    fbPar.voutChannel = voutchannel;
    fbPar.fbChannel   = 0;
    return gadi_vout_select_fb(voHandle, &fbPar);
}


int  gk_device_init(void* data)
{

    GADI_ERR   err      = GADI_OK;
    GADI_U32   streamId = 0;
    
    err = gadi_sys_set_log_level(2); //ok
    /* system module init.*/
    err |= gadi_sys_init(); //ok 
    err = gadi_sys_load_firmware(); //ok
    
    err = gdm_vi_init(); // ok
    err |= gdm_vout_init(); //ok 
    err |= gdm_venc_init(); //ok 
    err |= gdm_vi_open();  //ok
    err |= gdm_vout_open(); //ok
    err |= gdm_venc_open(); //ok
    if (err != GADI_OK) {
        printf("Applicantions initialize failed!\n");
        exit(-1);
    }

    gdm_vi_setup(); //ok
    gdm_vout_setup(); //ok
    
    gdm_venc_setup();
    gdm_vout_setup_after();
    
    for (streamId = 0; streamId < GADI_VENC_STREAM_NUM; streamId++)
    {
        err =  gdm_venc_start_encode_stream(streamId);
        if(err != 0)
        {
            printf("start stream[%d]\n",streamId);
        }
    }
    gadi_venc_print_params(vencHandle);
     
    err = gdm_isp_init();
    if(err < 0)
    {
        printf("gdm init ips faild\n");
    }
    err = gdm_isp_open();
    if(err < 0){
        printf("gdm_isp_open failed.\n");
        return -1;
    }
  
    err = gdm_isp_start();
    if((err < 0) && (GADI_ISP_ERR_FEATURE_NOT_SUPPORTED != err)){
        printf("gdm_isp_start failed.\n");
        return -1;
    }
    
    fb_select();

    return 0;

    /*creat get bit stream thread.*/
}

static pthread_t        videoThreadId;
static pthread_attr_t   vattr;

int start_read_venc_thread(void)
{
    
    runing_enable = TRUE;
    record_enable = FALSE;
    int retVal = 0;
    if (pthread_attr_init(&vattr) < 0)
    {
        return -1;
    }
    retVal = pthread_create(&videoThreadId, NULL, gdm_read_encode_streams, 0);
    if(retVal != 0){
        return -1;
    }
    //if(videoThreadId != 0){
      //  pthread_join(videoThreadId,NULL);
   // }

    return 0;

}

void start_recod(void* filename){
    record_enable = TRUE;
}

void stop_recod(void){
    record_enable = FALSE;
}

void snphot_jepg(void* filename){
    snphot_enable = TRUE;
}

void*   fb_init(void){
    
    return NULL;

}




