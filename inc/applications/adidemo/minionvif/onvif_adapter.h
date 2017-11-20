/*!
*****************************************************************************
** \file        subsystem/onvif/src/sdk_onvif.h
**
** \version     $Id$
**
** \brief       onvif adapter
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ONVIF_ADAPTER_H_
#define _ONVIF_ADAPTER_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//#include "onvif_std.h"
#include "onvif_priv.h"

#include "onvif_device.h"
#include "onvif_media.h"
#include "onvif_image.h"
#include "onvif_deviceio.h"
#include "onvif_ptz.h"
#include "onvif_analytics.h"
#include "onvif_event.h"

//#include "sdk_ptz.h"
//#include "sdk_sensor.h"
//#include "sdk_vin.h"
//#include "sdk_out.h"

//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************

typedef struct
{
	GK_S32 streamid;
	GK_S32 encode_type;
	GK_S32 encode_width;
	GK_S32 encode_height;
	GK_S32 framerate;
	GK_S32 bitrate;
	GK_S32 encInterval;
	GK_S32 quality;
	GK_S32 govLength;
	GK_S32 h264Profile;
}ONVIF_MediaVideoEncoderInfor_S;

typedef struct
{
	GK_S32 encode_type;
	GK_S32 bitrate;
    GK_S32 sampleRate;
}ONVIF_MediaAudioEncoderInfor_S;

typedef struct
{
	//GK_U32 blcMode;
	//GK_S32 blcLevel;
	GK_S32 brightness;      //y
	GK_S32 colorSaturation; //y
	GK_S32 contrast;        //y

	//GK_ULONG exposureMode;
	//GK_ULONG aeShutterTimeMin;
	//GK_ULONG aeShutterTimeMax;
	//GK_ULONG aeGainMin;
	//GK_ULONG aeGainMax;
	//GK_ULONG meShutterTime;
	//GK_ULONG meGain;

    //GK_ULONG focusMode;
    //GK_ULONG afNearLimit;
    //GK_ULONG afFarLimit;
    //GK_ULONG mfDefaultSpeed;
	GK_S32 irCutFilter;     //y
    GK_S32 sharpness;       //y

	//GK_ULONG wdrMode;
	//GK_ULONG wdrLevel;

    GK_S32 wbMode;          //y
	//GK_ULONG mwbRGain;
	//GK_ULONG mwbBGain;
	//GK_S32 settingsSave;

}ONVIF_ImagingInfor_S;

typedef struct
{
    GK_FLOAT panPosition;
    GK_FLOAT panSpeed;

    GK_FLOAT tiltPosition;
    GK_FLOAT tiltSpeed;

    GK_FLOAT zoomPosition;
    GK_FLOAT zoomSpeed;
}ONVIF_PTZAbsoluteMove_S;

typedef struct
{
	GK_FLOAT panTranslation;
	GK_FLOAT panSpeed;

	GK_FLOAT tiltTranslation;
	GK_FLOAT tiltSpeed;

	GK_FLOAT zoomTranslation;
	GK_FLOAT zoomSpeed;
}ONVIF_PTZRelativeMove_S;

typedef struct
{
	GK_S32 panSpeed;
	GK_S32 tileSpeed;
	GK_S32 zoomSpeed;
}ONVIF_PTZContinuousMove_S;

typedef struct
{
	GK_BOOL stopPanTilt;
	GK_BOOL stopZoom;
}ONVIF_PTZStop_S;

typedef struct
{
	GK_S32 presetNum;
	GK_CHAR presetName[MAX_NAME_LENGTH];
	GK_CHAR presetToken[MAX_TOKEN_LENGTH];	// unused
}ONVIF_PTZ_Preset_S;

typedef struct
{
	GK_U32 sizePreset;
	ONVIF_PTZ_Preset_S stPreset[MAX_PRESET_NUM];
} ONVIF_PTZ_GetPresets_S;

//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************
typedef struct
{
    GK_S32 (*GetVideoEncoderInfor)(ONVIF_MediaVideoEncoderInfor_S *pstMediaInfor, GK_S32 sizeEncoding);
    GK_S32 (*GetAudioEncoderInfor)(ONVIF_MediaAudioEncoderInfor_S *pstMediaInfor, GK_S32 sizeEncoding);
    GK_S32 (*GetStreamUri)(GK_U8 streamID, GONVIF_MEDIA_StreamType_E enStreamType, GK_CHAR *pszStreamUri, GK_S32 uriLen);
	//video
    GK_S32 (*SetResolution)(GK_S32 streamID, GK_S32 width, GK_S32 height, GK_S32 encodetype);
    GK_S32 (*SetFrameRate)(GK_S32 streamID, GK_S32 value);
    GK_S32 (*SetBitRate)(GK_S32 streamID, GK_S32 value);
    GK_S32 (*SetInterval)(GK_S32 streamID, GK_S32 value);
    GK_S32 (*SetQuality)(GK_S32 streamID, GK_FLOAT value);
    GK_S32 (*SetGovLength)(GK_S32 streamID, GK_S32 value);
    GK_S32 (*Snapshot)(void);
    //audio
    GK_S32 (*SetAudioEncodingType)(GK_S32 value);
    GK_S32 (*SetAudioBitrate)(GK_S32 value);
    GK_S32 (*SetAudioSampleRate)(GK_S32 value);
    //cfg
    GK_S32 (*SynVideoEncoderOptions)(GONVIF_MEDIA_VideoEncoderConfigurationOptions_S *pstStaticCfg, GK_S32 sizeOptions);
    GK_S32 (*SynAudioEncoderOptions)(GONVIF_MEDIA_AudioEncoderConfigurationOptions_S *pstStaticCfg, GK_S32 sizeOptions);
    void   (*SaveCfg)(void);
}GONVIF_ADAPTER_Media_S;

typedef struct
{
    GK_S32 (*GetImagingInfor)(ONVIF_ImagingInfor_S *pstImagingInfor);
    GK_S32 (*SetBrightness)(GK_FLOAT brightness);
    GK_S32 (*SetColorSaturation)(GK_FLOAT saturation);
    GK_S32 (*SetContrast)(GK_FLOAT contrast);
    GK_S32 (*SetIrCutFilter)(GK_S32 mode);
    GK_S32 (*SetSharpness)(GK_FLOAT sharpness);
    GK_S32 (*SetWbMode)(GK_S32 mode);
    void   (*SaveCfg)(void);
#if 0
	/* BLC Mode: 0 off, 1 on. */
    GK_S32 (*ImagingSetBlcMode)(GK_U32 mode);
	/*BLC Level*/
    GK_S32 (*ImagingSetBlcLevel)(GK_S32 level);
	/*Auto exposure mode: 0 disable,1 enable*/
    GK_S32 (*ImagingSetExposureMode)(GK_S32 mode);
	/*Auto exposure shutter time Min*/
    GK_S32 (*ImagingSetAeShutterTimeMin)(GK_S32 shutterMin);
	/*Auto exposure sensor gain max*/
    GK_S32 (*ImagingSetAeShutterTimeMax)(GK_S32 shutterMax);
    /*Auto exposure sensor gain Min*/
    GK_S32 (*ImagingSetAeGainMin)(GK_S32 gainMin);
	/*Auto exposure sensor gain max*/
    GK_S32 (*ImagingSetAeGainMax)(GK_S32 gainMax);
	/*Manual exposure exposuretime*/
    GK_S32 (*ImagingSetMeShutterTime)(GK_S32 shutter);
	/*Manual exposure gain(ISO)*/
    GK_S32 (*ImagingSetMeGain)(GK_S32 gain);
    /*Auto focus mode: enable/disable*/
    GK_S32 (*ImagingSetFocusMode)(GK_S32 mode);
    /*Auto fucus: nearLimit */
    GK_S32 (*ImagingSetAfNearLimit)(GK_S32 nearLimit);
    /*Auto focus: farLimit*/
    GK_S32 (*ImagingSetAfFarLimit)(GK_S32 farLimit);
    /*Manual focus: defaultSpeed*/
    GK_S32 (*ImagingSetMfDefaultSpeed)(GK_S32 defaultSpeed);
    /*Wide dynamic range mode*/
    GK_S32 (*ImagingSetWdrMode)(GK_S32 mode);
    /*Wide dynamic range level*/
    GK_S32 (*ImagingSetWdrLevel)(GK_S32 level);
    /*WhiteBalance rgain*/
	GK_S32 (*ImagingSetMwbRGain)(GK_S32 rGain);
    /*WhiteBalance bgain*/
	GK_S32 (*ImagingSetMwbBGain)(GK_S32 bGain);
    /*ForcePersistenc*/
    GK_S32 (*ImagingSetForcePersistence)(GK_S32 ForcePersistence);
#endif
}ONVIF_ADAPTER_Imaging_S;

typedef struct
{
    GK_S32 (*ContinuousMove)(ONVIF_PTZContinuousMove_S stContinuousMove);
    GK_S32 (*AbsoluteMove)(ONVIF_PTZAbsoluteMove_S stAbsoluteMove);
    GK_S32 (*RelativeMove)(ONVIF_PTZRelativeMove_S stRelativeMove);
    GK_S32 (*Stop)(ONVIF_PTZStop_S stStop);
    GK_S32 (*GetPresets)(ONVIF_PTZ_GetPresets_S *pstPresets);
    GK_S32 (*SetPreset)(ONVIF_PTZ_Preset_S stPreset);
    GK_S32 (*GotoPreset)(GK_S32 orderNum);
    GK_S32 (*RemovePreset)(GK_S32 orderNum);
}ONVIF_ADAPTER_PTZ_S;


typedef struct
{
    GK_S32  isDHCP;
    GK_CHAR macAddr[MAX_8_LENGTH];
    GK_CHAR ipAddr[MAX_16_LENGTH];
    GK_CHAR maskAddr[MAX_16_LENGTH];
    GK_CHAR gwAddr[MAX_16_LENGTH];
    GK_CHAR dnsAddr1[MAX_16_LENGTH];
    GK_CHAR dnsAddr2[MAX_16_LENGTH];
    GK_S32  mtu;
}ONVIF_DeviceNet_S;

typedef struct
{
    GK_S32  isNTP;
    GK_CHAR ntpAddr[MAX_32_LENGTH];
    GK_S32  port;
}ONVIF_DeviceNTP_S;

typedef enum
{
	Factory_hard = 0,
	Factory_soft = 1,
}ONVIF_DeviceFactory_E;

typedef enum
{
	DEVICES       = 0,
	DISCOVER     = 1,
	IMAGING      = 2,
	MEDIADYNAMIC = 3,
	MEDIASTATIC  = 4,
	PTZ          = 5,
}ONVIF_FILE_TYPE;

typedef struct
{
	//network
    GK_S32 (*GetNetworkInfor)(GK_CHAR *netCardName, ONVIF_DeviceNet_S *netInfor);
    GK_S32 (*SetNetworkInfor)(GK_CHAR *netCardName, ONVIF_DeviceNet_S netInfor);
    GK_S32 (*GetNetworkMac)(GK_CHAR *netCardName, GK_CHAR *macStr);
    GK_S32 (*SetNetworkMac)(GK_CHAR *netCardName, GK_CHAR *macStr);
    GK_S32 (*GetNetworkIP)(GK_CHAR *netCardName, struct sockaddr *addr);
    GK_S32 (*SetNetworkIP)(GK_CHAR *netCardName, GK_CHAR *ipStr);
    GK_S32 (*GetNetworkMask)(GK_CHAR *netCardName, struct sockaddr *addr);
    GK_S32 (*SetNetworkMask)(GK_CHAR *netCardName, GK_CHAR *maskStr);
    GK_S32 (*GetNetworkDNS)(void);
    GK_S32 (*SetNetworkDNS)(char *netCardName, ONVIF_DeviceNet_S netInfor);
    GK_S32 (*GetNetworkNTP)(ONVIF_DeviceNTP_S *ntpInfor);
    GK_S32 (*SetNetworkNTP)(ONVIF_DeviceNTP_S  ntpInfor);
    GK_S32 (*GetNetworkGateway)(GK_CHAR *netCardName, GK_CHAR *gwStr);
    GK_S32 (*SetNetworkGateway)(GK_CHAR *netCardName, GK_CHAR *gwStr);
    GK_S32 (*GetNetworkMTU)(GK_CHAR *netCardName, GK_S32 *mtu);
    GK_S32 (*SetNetworkMTU)(GK_CHAR *netCardName, GK_S32 mtu);
    GK_S32 (*SetNetworkDHCP)(GK_CHAR *netCardName, GK_S32 isDHCP);
    GK_S32 (*GetNetworkWifiNICName)(GK_CHAR *netCardName, GK_S32 nameLen);
    GK_S32 (*GetNetworkIsRunning)(GK_CHAR *netCardName);
	//system
    GK_S32 (*SetSystemFactoryDefault)(ONVIF_DeviceFactory_E enFactory);
    GK_S32 (*GetSystemDeviceInfor)(GK_CHAR *netCardName, GONVIF_DEVMNG_GetDeviceInformation_Res_S *pstADP_GetDeviceInformationRes);
    GK_S32 (*GetSystemDateAddTime)(GK_CHAR *timeBuf, GK_S32 *timeZone);
    GK_S32 (*SetSystemDateAddTime)(GK_CHAR *timeBuf, GK_S32  timeZone);
    GK_S32 (*SetSystemReboot)(GK_S32 delayTime);
	GK_S32 (*SystemGetUsers)(GONVIF_DEVMNG_GetUsers_Res_S *pstUsersCfg);
	GK_S32 (*SystemCreateUsers)(GONVIF_DEVMNG_User_S userInfo);
	GK_S32 (*SystemDeleteUsers)(GK_CHAR *name);
	GK_S32 (*SystemSetUser)(GONVIF_DEVMNG_User_S userInfo);
}ONVIF_ADAPTER_Device_S;


typedef struct
{
	GK_S32 (*OnvifGetFile)(ONVIF_FILE_TYPE types,GK_S8 *buffer,GK_S32 *length);
	GK_S32 (*OnvifSaveFile)(ONVIF_FILE_TYPE types,GK_S8 *file_name);
}ONVIF_XML_FILE;

//*****************************************************************************
//*****************************************************************************
//** Global Data
//*****************************************************************************
//*****************************************************************************



//*****************************************************************************
//*****************************************************************************
//** API Functions
//*****************************************************************************
//*****************************************************************************



#ifdef __cplusplus
extern "C" {
#endif

extern GONVIF_ADAPTER_Media_S   g_stMediaAdapter;
extern ONVIF_ADAPTER_Imaging_S g_stImagingAdapter;
extern ONVIF_ADAPTER_PTZ_S     g_stPTZAdapter;
extern ONVIF_ADAPTER_Device_S  g_stDeviceAdapter;
extern ONVIF_XML_FILE          XmlFileHandle;


#ifdef __cplusplus
}
#endif



#endif /* _ONVIF_ADAPTER_H_ */

