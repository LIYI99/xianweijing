/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/include/onvif_priv.h
**
** \brief       onvif nvt
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/


#ifndef _ONVIF_PRIV_H_
#define _ONVIF_PRIV_H_

#include <semaphore.h>

#include "onvif_std.h"
#include "onvif_device.h"
#include "onvif_media.h"
#include "onvif_image.h"
#include "onvif_deviceio.h"
#include "onvif_ptz.h"
#include "onvif_analytics.h"
#include "onvif_event.h"

#include "onvif_adapter.h"

//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
typedef enum
{
	GK_Hello = 0,
	GK_Bye   = 1, 
}ONVIF_HelloBye_E;

typedef enum
{
	XML_DISCOVRY = 0,
	XML_DEVICE,	
	XML_IMAGING,
	XML_MEIDA,
	XML_ALL,
}ONVIF_XmlFileType_E;

typedef enum
{
	NVT_SYS_ERROR = -1, 
	NVT_NO_ERROR = 0,
}GONVIF_NVT_Ret_E;

typedef struct
{
    GK_CHAR *buffer;
    GK_S32   Len;
}ONVIF_Http_package_S;

typedef enum
{
    DISCOVRY_HELLO = 0,
    DISCOVRY_BYE
}DISCOVER_SIGNAL_TYPE;



//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************

/*DeviceMgmt*/
typedef struct gkonvif_Devicemgmt
{
    /* Security */
    GK_U8 Usercount;
    GONVIF_DEVMNG_User_S **User;
    /*network */
    GK_BOOL DHCP;
    GK_BOOL NIC_alive;						/*network interface card is alive or not*/
    GONVIF_DEVMNG_GetNetworkProtocols_Res_S stNetworkProtocolsInf;

    /*time */
    GONVIF_DEVMNG_SetDateTimeType_E enDateTimeType;
    GK_BOOL bDaylightSavings;
    GK_S32  time_zone;
    GONVIF_DEVMNG_GetDeviceInformation_Res_S stDeviceInfo;
}Gkonvif_Devicemgmt;

/*Discovery*/
typedef struct gkonvif_Discovery
{
    GONIVF_DEVMNG_DiscoveryMode_E discoverymode;
    GK_BOOL Is_security;
    GK_CHAR nicName[NIC_NAME_LENTH];
    GK_CHAR hostip[IPV4_STR_LENGTH];    
    GK_CHAR hostmac[7];    
    GK_CHAR *WebServerUrl;					    /*Device Server Address*/
    GK_U32 hostport;
    GK_U8 Scopecount;
    GONVIF_DEVMNG_Scopes_S **Scopes;				/*match info*/
}GKonvif_Discovery;

typedef struct gkIpc_OnvifInf
{   
//discovery
    GKonvif_Discovery discov;
//device managment
    Gkonvif_Devicemgmt devmgmt;
//ptz node
    GK_U8 sizePTZnode;
    GONVIF_PTZ_Node_S *pstPTZnode;
//media and ptz    
  	//media profile  
    GK_U8 ProfileNum;
    GONVIF_MEDIA_Profile_S *gokeIPCameraProfiles;
	//congfiguration
    GK_U8 VideoSourceNum;
    GONVIF_MEDIA_VideoSourceConfiguration_S *gokeIPCameraVideoSource;
    GK_U8 VideoEncodeNum;
    GONVIF_MEDIA_VideoEncoderConfiguration_S *gokeIPCameraVideoEncode;
    GK_U8 AudioSourceNum;
    GONVIF_MEDIA_AudioSourceConfiguration_S *gokeIPCameraAudioSource;
    GK_U8 AudioEncodeNum;
    GONVIF_MEDIA_AudioEncoderConfiguration_S *gokeIPCameraAudioEncode;
    GK_U8 PTZConfigurationNum;
    GONVIF_PTZ_Configuration_S *gokePTZConfiguration;     
    //configuration option
    GK_U8 VideoSourceOptionNum;
    GONVIF_MEDIA_VideoSourceConfigurationOptions_S *gokeVideoSourceConfigurationOption;
    GK_U8 VideoEncodeOptionNum;
    GONVIF_MEDIA_VideoEncoderConfigurationOptions_S *gokeVideoEncoderConfigurationOption; 
    GK_U8 AudioSourceOptionNum;
    GONVIF_MEDIA_AudioSourceConfigurationOptions_S *gokeAudioSourceConfigurationOption;
    GK_U8 AudioEncodeOptionNum;
    GONVIF_MEDIA_AudioEncoderConfigurationOptions_S *gokeAudioEncoderConfigurationOption;
    GK_U8 sizePTZConfigurationOptions;
    GONVIF_PTZ_ConfigurationOptions_S *pstPTZConfigurationOptions;
    GK_U8 VideoAnalyticNum;
    GONVIF_MEDIA_VideoAnalyticsConfiguration_S *gokeIPCameraVideoAnalytic; 
//imaging    
    GONVIF_IMAGE_Options_S *gokeImagingOptions; 
    GONVIF_IMAGE_MoveOptions_S *gokeMoveOptions;
//relay output
    //DeviceIO RelayOutputs
    GK_U8 relayOutputsNum;
    GONVIF_DEVICEIO_RelayOutput_S *pstGokeIPCameraRelayOutputs;
    //GK_U8 RelayOutputsOptionNum;
    //GONVIF_DEVICEIO_RelayOutputsCfg_Options_S *gokeRelayOutputsConfigurationOption;
    
}GkIpc_OnvifInf;

//*****************************************************************************
//*****************************************************************************
//** Global Data
//*****************************************************************************
//*****************************************************************************

extern GkIpc_OnvifInf g_GkIpc_OnvifInf;

//*****************************************************************************
//*****************************************************************************
//** API Functions
//*****************************************************************************
//*****************************************************************************


#ifdef __cplusplus
extern "C" {
#endif

GONVIF_NVT_Ret_E GK_NVT_Env_Init(void);
GONVIF_NVT_Ret_E GK_NVT_Signal_Init(void);
GONVIF_NVT_Ret_E GK_NVT_Device_Init(int type);
GONVIF_NVT_Ret_E GK_NVT_Discover_Init(int type);
GONVIF_NVT_Ret_E GK_NVT_Media_Init(int type);
GONVIF_NVT_Ret_E GK_NVT_Imaging_Init(int type);
GONVIF_NVT_Ret_E GK_NVT_Event_Init(void);
GONVIF_NVT_Ret_E GK_NVT_Ptz_Init(int type);

GONVIF_NVT_Ret_E GK_NVT_DevMng_Register(GOnvif_DEVMNG_CallbackFunc_S *pstNVTDevManageCBFunc);
GONVIF_NVT_Ret_E GK_NVT_Media_Register(GOnvif_MEDIA_CallbackFunc_S *pstNVTMediaCBFunc);
GONVIF_NVT_Ret_E GK_NVT_DeviceIO_Register(GOnvif_DEVICEIO_CallbackFunc_S *pstNVTDeviceIOCBFunc);
GONVIF_NVT_Ret_E GK_NVT_Event_Register(GOnvif_EVENT_CallbackFunction_S *pstNVTEventCBFunc);
GONVIF_NVT_Ret_E GK_NVT_Image_Register(GOnvif_IMAGE_CallbackFunc_S *pstNVTImageCBFunc);
GONVIF_NVT_Ret_E GK_NVT_PTZ_Register(GOnvif_PTZ_CallbackFunc_S *pstNVTPtzCBFunc);

GONVIF_NVT_Ret_E GK_NVT_Analytics_Register(GOnvif_ANALYTICS_CallbackFunc_S *pstNVTAnalyticsCBFunc);
GONVIF_NVT_Ret_E GK_NVT_Start_HttpSoapService(GK_BOOL isFromWeb,GK_S32 webSocket);
GONVIF_NVT_Ret_E GK_NVT_Start_UdpSoapService(void);
GONVIF_NVT_Ret_E GK_NVT_SoapProc_InWeb(GK_S32 clientFd, GK_U32 clientIp, GK_U32 clientPort, const GK_CHAR *pszClientStream, GK_U32 clientSize);

void GK_NVT_SaveXmlConfig(ONVIF_XmlFileType_E xmltype);
void GK_NVT_SendHelloBye(ONVIF_HelloBye_E hbType);

ONVIF_Http_package_S *soap_proc_GetHttpPackage(void);
GK_S32 soap_proc_ReleaseHttpPackage(void);


#ifdef __cplusplus
}
#endif


#endif /* _ONVIF_PRIV_H_ */
