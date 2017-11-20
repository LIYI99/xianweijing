/*!
*****************************************************************************
** \file        subsystem/onvif-no-gsoap/src/onvif_deviceio.h
**
** \brief       onvif deviceio
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/
#ifndef _ONVIF_DEVICEIO_H_
#define _ONVIF_DEVICEIO_H_

#include "onvif_std.h"
#include "onvif_image.h"

/***************************************************************************/

typedef enum
{
    RelayMode_Monostable = 0,
    RelayMode_Bistable = 1
}GONVIF_DEVICEIO_RelayMode_E;

typedef enum
{
    RelayIdleState_closed = 0,
    RelayIdleState_open = 1
}GONVIF_DEVICEIO_RelayIdleState_E;

typedef enum
{
    RelayLogicalState_active = 0,
    RelayLogicalState_inactive = 1
}GONVIF_DEVICEIO_RelayLogicalState_E;


typedef struct
{
	GK_S32 width;
	GK_S32 height;
}GONVIF_DEVICEIO_VideoResolution_S;

typedef struct
{
	GK_CHAR aszToken[MAX_TOKEN_LENGTH];
	GK_FLOAT framerate;
	GONVIF_DEVICEIO_VideoResolution_S stResolution;
	GONVIF_IMAGE_Settings_S stImaging;
}GONVIF_DEVICEIO_VideoSource_S;

typedef struct
{
    GK_CHAR aszToken[MAX_TOKEN_LENGTH];
    GK_S32  channels;
}GONVIF_DEVICEIO_AudioSource_S;

/*Relayoutput*/
typedef struct
{
	GONVIF_DEVICEIO_RelayMode_E enMode;
	GK_S64 delayTime;
	GONVIF_DEVICEIO_RelayIdleState_E enIdleState;
}GONVIF_DEVICEIO_RelayOutputSettings_S;

typedef struct
{
	GK_CHAR aszToken[MAX_TOKEN_LENGTH];
	GONVIF_DEVICEIO_RelayOutputSettings_S stProperties;
}GONVIF_DEVICEIO_RelayOutput_S;

/***************************************************************************/
//GetVideoSources
typedef struct
{
	GK_S32 sizeVideoSources;
	GONVIF_DEVICEIO_VideoSource_S stVideoSources[MAX_VIDEOSOURCE_NUM];
}GONVIF_DEVICEIO_GetVideoSources_Res_S;

//GetAudioSources
typedef struct
{
    GK_S32 sizeAudioSources;
    GONVIF_DEVICEIO_AudioSource_S  stAudioSources[MAX_AUDIOSOURCE_NUM];
}GONVIF_DEVICEIO_GetAudioSources_Res_S;

//GetRelayOutputs
typedef struct
{
    GK_U8 sizeRelayOutputs;
    GONVIF_DEVICEIO_RelayOutput_S stRelayOutputs[MAX_RELAYOUTPUT_NUM];
}GONVIF_DEVICEIO_GetRelayOutputs_Res_S;

//SetRelayOutputSettings
typedef struct
{
    GK_S8 aszRelayOutputToken[MAX_TOKEN_LENGTH];
	GONVIF_DEVICEIO_RelayOutputSettings_S stProperties;
}GONVIF_DEVICEIO_SetRelayOutputSettings_S;

//SetRelayOutputState
typedef struct
{
	char aszRelayOutputToken[MAX_TOKEN_LENGTH];
	GONVIF_DEVICEIO_RelayLogicalState_E enLogicalState;
}GONVIF_DEVICEIO_SetRelayOutputState_S;

/***************************************************************************/
typedef ONVIF_RET (*GONVIF_DEVICEIO_GetVideoSources)(GONVIF_DEVICEIO_GetVideoSources_Res_S *pstIO_GetVideoSourcesRes);
typedef ONVIF_RET (*GONVIF_DEVICEIO_GetAudioSources)(GONVIF_DEVICEIO_GetAudioSources_Res_S *pstIO_GetAudioSourcesRes);
typedef ONVIF_RET (*GONVIF_DEVICEIO_GetRelayOutputs)(GONVIF_DEVICEIO_GetRelayOutputs_Res_S *pstIO_GetRelayOutputsRes);
typedef ONVIF_RET (*GONVIF_DEVICEIO_SetRelayOutputSettings)(GONVIF_DEVICEIO_SetRelayOutputSettings_S *pstIO_SetRelayOutputSettings);
typedef ONVIF_RET (*GONVIF_DEVICEIO_SetRelayOutputState)(GONVIF_DEVICEIO_SetRelayOutputState_S *pstIO_SetRelayOutputState);

typedef struct 
{
    GONVIF_DEVICEIO_GetVideoSources pfnIO_GetVideoSources;
    GONVIF_DEVICEIO_GetAudioSources pfnIO_GetAudioSources;
    GONVIF_DEVICEIO_GetRelayOutputs pfnIO_GetRelayOutputs;
    GONVIF_DEVICEIO_SetRelayOutputSettings pfnIO_SetRelayOutputSettings;
    GONVIF_DEVICEIO_SetRelayOutputState pfnIO_SetRelayOutputState;
}GOnvif_DEVICEIO_CallbackFunc_S;

#ifdef __cplusplus
extern "C" {
#endif





#ifdef __cplusplus
}
#endif



#endif /* _ONVIF_DEVICEIO_H_ */

