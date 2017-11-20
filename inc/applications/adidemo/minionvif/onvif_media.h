/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_media.h
**
** \brief       onvif meida
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ONVIF_MEDIA_H_
#define _ONVIF_MEDIA_H_

#include "onvif_std.h"
#include "onvif_ptz.h"

typedef enum
{
    VideoEncoding_JPEG = 0,
    VideoEncoding_MPEG = 1,
    VideoEncoding_H264 = 2
}GONVIF_MEDIA_VideoEncoding_E;

typedef enum 
{
    Mpeg4Profile_SP  = 0,
    Mpeg4Profile_ASP = 1
}GONVIF_MEDIA_Mepg4Profile_E;

typedef enum
{
    H264Profile_BaseLine    = 0,
    H264Profile_Main        = 1,
    H264Profile_Extended    = 2,
    H264Profile_High        = 3
}GONVIF_MEDIA_H264Profile_E;

typedef enum 
{
    AudioEncoding_G711 = 0,
    AudioEncoding_G726 = 1,
    AudioEncoding_AAC  = 2
}GONVIF_MEDIA_AudioEncoding_E;

typedef enum 
{
    StreamType_RTP_Unicast = 0,
    StreamType_RTP_Multicast = 1
}GONVIF_MEDIA_StreamType_E;

typedef enum 
{
    TransportProtocol_UDP = 0,
    TransportProtocol_TCP = 1,
    TransportProtocol_RTSP = 2,
    TransportProtocol_HTTP = 3
}GONVIF_MEDIA_TransportProtocol_E;


//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************
typedef struct 
{
    GK_S32 x;
    GK_S32 y;
    GK_S32 width;
    GK_S32 height;
}GONVIF_MEDIA_IntRectangle_S;

typedef struct 
{
    GK_S32 width;
    GK_S32 height;
}GONVIF_MEDIA_VideoResolution_S;

typedef struct 
{
    GK_S32 frameRateLimit;
    GK_S32 encodingInterval;
    GK_S32 bitrateLimit;
}GONVIF_MEDIA_VideoRateControl_S;

typedef struct
{
    GONVIF_IPType_E enType;
    GK_CHAR *pszIPv4Address;
    GK_CHAR *pszIPv6Address;
}GONVIF_MEDIA_IpAddress_S;


typedef struct 
{
    GK_S32 govLength;
    GONVIF_MEDIA_Mepg4Profile_E enMpeg4Profile;
}GONVIF_MEDIA_Mpeg4Configuration_S;

typedef struct
{
    GK_S32 govLength;
    GONVIF_MEDIA_H264Profile_E enH264Profile;
}GONVIF_MEDIA_H264Configuration_S;


typedef struct 
{
    GONVIF_MEDIA_IpAddress_S stIPAddress;
    GK_S32  port;
    GK_S32  ttl;
    GONVIF_Boolean_E enAutoStart;
}GONVIF_MEDIA_MulticastConfiguration_S;



typedef struct 
{
    GK_CHAR *pszName;
    GK_CHAR *pszValue;
}GONVIF_MEDIA_ItemList_SimpleItem_S;

typedef struct 
{
    GK_CHAR *pszName;
}GONVIF_MEDIA_ItemList_ElementItem_S;

typedef struct 
{
    GK_S32 sizeSimpleItem;
    GONVIF_MEDIA_ItemList_SimpleItem_S *pstSimpleItem;
    GK_S32 sizeElementItem;
    GONVIF_MEDIA_ItemList_ElementItem_S *pstElementItem;
}GONVIF_MEDIA_ItemList_S;

typedef struct 
{
    GONVIF_MEDIA_ItemList_S stParameters;
    GK_CHAR *pszName;
    GK_CHAR *pszType;
}GONVIF_MEDIA_Config_S;

typedef struct 
{
    GK_S32 sizeRule;
    GONVIF_MEDIA_Config_S *pstRule;
}GONVIF_MEDIA_RuleEngineCfg_S;

typedef struct 
{
    GK_S32 sizeAnalyticsModule;
    GONVIF_MEDIA_Config_S *pstAnalyticsModule;
}GONVIF_MEDIA_AnalyticsEngineConfiguration_S;

typedef struct 
{
    GONVIF_Boolean_E enStatus;
    GONVIF_Boolean_E enPosition;
}GONVIF_MEDIA_PTZFilter_S;

typedef struct 
{
    GK_CHAR *pszToken;
    GK_CHAR *pszName;
    GK_CHAR *pszSourceToken;
    GK_S32  useCount;
    GONVIF_MEDIA_IntRectangle_S stBounds;
}GONVIF_MEDIA_VideoSourceConfiguration_S;

typedef struct 
{
    GK_CHAR *pszName;
    GK_CHAR *pszToken;
    GK_CHAR *pszSourceToken;
    GK_S32  useCount;
}GONVIF_MEDIA_AudioSourceConfiguration_S;


typedef struct 
{
    GK_CHAR *pszName;
    GK_S32  useCount;
    GK_CHAR *pszToken;
    GONVIF_MEDIA_VideoEncoding_E enEncoding;
    GONVIF_MEDIA_VideoResolution_S stResolution;
    GK_FLOAT quality;
    GONVIF_MEDIA_VideoRateControl_S  stRateControl;
    GONVIF_MEDIA_Mpeg4Configuration_S stMPEG4;
    GONVIF_MEDIA_H264Configuration_S  stH264;
    GONVIF_MEDIA_MulticastConfiguration_S stMulticast;
    GK_S64 sessionTimeout;
}GONVIF_MEDIA_VideoEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR *pszName;
    GK_CHAR *pszToken;
    GK_S32  useCount;
    GONVIF_MEDIA_AudioEncoding_E enEncoding;
    GK_S32 bitRate;
    GK_S32 sampleRate;
    GONVIF_MEDIA_MulticastConfiguration_S stMulticast;
    GK_S64 sessionTimeout;
}GONVIF_MEDIA_AudioEncoderConfiguration_S;

typedef struct
{
    GK_CHAR *pszName;
    GK_S32  useCount;
    GK_CHAR *pszToken;
    GONVIF_MEDIA_AnalyticsEngineConfiguration_S stAnalyticsEngineConfiguration;
    GONVIF_MEDIA_RuleEngineCfg_S stRuleEngineConfiguration;
}GONVIF_MEDIA_VideoAnalyticsConfiguration_S;

typedef struct 
{
    GK_CHAR *pszName;
    GK_S32  useCount;
    GK_CHAR *pszToken;
    GONVIF_MEDIA_PTZFilter_S  stPTZStatus;
    GONVIF_Boolean_E enAnalytics;
    GONVIF_MEDIA_MulticastConfiguration_S stMulticast;
    GK_S64 sessionTimeout;
}GONVIF_MEDIA_MetadataConfiguration_S;

typedef struct
{
  GONVIF_IntRange_S stXRange;
  GONVIF_IntRange_S stYRange;
  GONVIF_IntRange_S stWidthRange;
  GONVIF_IntRange_S stHeightRange;
}GONVIF_MEDIA_IntrectangleRange_S;

typedef struct
{
    GONVIF_MEDIA_IntrectangleRange_S stBoundsRange;
    GK_S32 sizeVideoSourceTokensAvailable;
    GK_CHAR **pszVideoSourceTokensAvailable;
}GONVIF_MEDIA_VideoSourceConfigurationOptions_S;


typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionsAvailable; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
}GONVIF_MEDIA_JpegOptions_S;

typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionsAvailable; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
    GONVIF_IntRange_S stBitrateRange;
}GONVIF_MEDIA_JpegOptions2_S;

typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionAvailable; 
    GONVIF_IntRange_S stGovLengthRange; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
    GK_S32 sizeMpeg4ProfilesSupported; 
    GONVIF_MEDIA_Mepg4Profile_E *peMpeg4ProfilesSupported; 
}GONVIF_MEDIA_Mpeg4Options_S; 

typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionAvailable; 
    GONVIF_IntRange_S stGovLengthRange; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
    GK_S32 sizeMpeg4ProfilesSupported; 
    GONVIF_MEDIA_Mepg4Profile_E *peMpeg4ProfilesSupported; 
    GONVIF_IntRange_S stBitrateRange;
}GONVIF_MEDIA_Mpeg4Options2_S; 

typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionsAvailable; 
    GONVIF_IntRange_S stGovLengthRange; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
    GK_S32 sizeH264ProfilesSupported; 
    GONVIF_MEDIA_H264Profile_E  *penH264ProfilesSupported;
}GONVIF_MEDIA_H264Options_S;

typedef struct 
{ 
    GK_S32 sizeResolutionsAvailable; 
    GONVIF_MEDIA_VideoResolution_S *pstResolutionsAvailable; 
    GONVIF_IntRange_S stGovLengthRange; 
    GONVIF_IntRange_S stFrameRateRange; 
    GONVIF_IntRange_S stEncodingIntervalRange; 
    GK_S32 sizeH264ProfilesSupported; 
    GONVIF_MEDIA_H264Profile_E  *penH264ProfilesSupported;
    GONVIF_IntRange_S stBitrateRange;    
}GONVIF_MEDIA_H264Options2_S;

typedef struct
{
	GONVIF_MEDIA_JpegOptions2_S stJPEG;
	GONVIF_MEDIA_Mpeg4Options2_S stMPEG4;
	GONVIF_MEDIA_H264Options2_S stH264;
}GONVIF_MEDIA_VideoEncoderOptionsExtension_S;

typedef struct 
{ 
  GONVIF_IntRange_S stQualityRange; 
  GONVIF_MEDIA_JpegOptions_S stJPEG; 
  GONVIF_MEDIA_Mpeg4Options_S stMPEG4; 
  GONVIF_MEDIA_H264Options_S  stH264; 
  GONVIF_MEDIA_VideoEncoderOptionsExtension_S stExtension;
}GONVIF_MEDIA_VideoEncoderConfigurationOptions_S;


typedef struct 
{
    GK_S32 sizeInputTokensAvailable;
    GK_CHAR **pszInputTokensAvailable;
}GONVIF_MEDIA_AudioSourceConfigurationOptions_S;

typedef struct 
{
    GK_S32 sizeItems;
    GK_S32 *pItems;
}GONVIF_MEDIA_Intlist_S;


typedef struct
{
  GONVIF_MEDIA_AudioEncoding_E enEncoding;
  GONVIF_MEDIA_Intlist_S stBitrateList;
  GONVIF_MEDIA_Intlist_S stSampleRateList;
}GONVIF_MEDIA_AudioEncoderConfigurationOption_S;

typedef struct
{
    GK_S32 sizeOptions;
    GONVIF_MEDIA_AudioEncoderConfigurationOption_S *pstOptions;
}GONVIF_MEDIA_AudioEncoderConfigurationOptions_S;

typedef struct 
{
    GK_CHAR *pszName;
    GK_CHAR *pszToken;
    GONVIF_Boolean_E enFixed;
    GONVIF_MEDIA_VideoSourceConfiguration_S    *pstVideoSourceConfiguration;
    GONVIF_MEDIA_AudioSourceConfiguration_S    *pstAudioSourceConfiguration;
    GONVIF_MEDIA_VideoEncoderConfiguration_S   *pstVideoEncoderConfiguration;
    GONVIF_MEDIA_AudioEncoderConfiguration_S   *pstAudioEncoderConfiguration;
    GONVIF_MEDIA_VideoAnalyticsConfiguration_S *pstVideoAnalyticsConfiguration;
    GONVIF_PTZ_Configuration_S 		           *pstPTZConfiguration;
    GONVIF_MEDIA_MetadataConfiguration_S 	   *pstMetadataConfiguration;
}GONVIF_MEDIA_Profile_S;

typedef struct kk
{
    GONVIF_MEDIA_TransportProtocol_E enProtocol;
    struct kk *pstTunnel;
}GONVIF_MEDIA_Transport_S;

typedef struct 
{
    GONVIF_MEDIA_StreamType_E enStream;
    GONVIF_MEDIA_Transport_S stTransport;
}GONVIF_MEDIA_StreamSetup_S;

typedef struct 
{
    GK_CHAR aszUri[MAX_URI_LENGTH];
    GONVIF_Boolean_E enInvalidAfterConnect;
    GONVIF_Boolean_E enInvalidAfterReboot;
    GK_S64  timeout;
}GONVIF_MEDIA_MediaUri_S;

typedef struct
{
    GK_S32 maximumNumberOfProfiles;    
}GONVIF_MEDIA_ProfileCapabilities_S;

typedef struct
{
	GONVIF_Boolean_E enRTPMulticast;
	GONVIF_Boolean_E enRTP_USCORETCP;
	GONVIF_Boolean_E enRTP_USCORERTSP_USCORETCP;
	GONVIF_Boolean_E enNonAggregateControl;
	GONVIF_Boolean_E enNoRTSPStreaming;
}GONVIF_MEDIA_StreamingCapabilities_S;

typedef struct 
{
    GONVIF_MEDIA_ProfileCapabilities_S stProfileCapabilities;
	GONVIF_MEDIA_StreamingCapabilities_S stStreamingCapabilities;
	GONVIF_Boolean_E enSnapshotUri;
	GONVIF_Boolean_E enRotation;
	GONVIF_Boolean_E enVideoSourceMode;
	GONVIF_Boolean_E enOSD;
}GONVIF_MEDIA_Capabilities_S;


typedef struct
{
    GONVIF_MEDIA_Capabilities_S stCapabilities;
}GONVIF_MEDIA_GetServiceCapabilities_Res_S;

typedef struct 
{
    GK_CHAR aszName[MAX_NAME_LENGTH];
    GK_CHAR aszToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_CreateProfile_S;

typedef struct
{
    GONVIF_MEDIA_Profile_S stProfile;    
}GONVIF_MEDIA_CreateProfile_Res_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetProfile_S;

typedef struct
{
    GONVIF_MEDIA_Profile_S stProfile;
}GONVIF_MEDIA_GetProfile_Res_S;

typedef struct
{
    GK_S32 sizeProfiles;
    GONVIF_MEDIA_Profile_S stProfiles[MAX_PROFILE_NUM];
}GONVIF_MEDIA_GetProfiles_Res_S;

typedef struct 
{
    GK_CHAR  aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_DeleteProfile_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_AddVideoSourceConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_AddVideoEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_AddAudioSourceConfiguration_S;

typedef struct 
{
   GK_CHAR aszProfileToken[ MAX_TOKEN_LENGTH];
   GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_AddAudioEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_AddPTZConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_RemoveVideoSourceConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_RemoveVideoEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR  aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_RemoveAudioSourceConfiguration_S;

typedef struct
{
    GK_CHAR  aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_RemoveAudioEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_RemovePTZConfiguration_S;

typedef struct 
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_VideoSourceConfiguration_S stConfigurations[MAX_VIDEOSOURCE_NUM];
}GONVIF_MEDIA_GetVideoSourceConfigurations_Res_S;

typedef struct 
{
    GK_CHAR  aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetVideoSourceConfiguration_S;

typedef struct
{
    GONVIF_MEDIA_VideoSourceConfiguration_S stConfiguration;
}GONVIF_MEDIA_GetVideoSourceConfiguration_Res_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations_S;

typedef struct
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_VideoSourceConfiguration_S stConfigurations[MAX_VIDEOSOURCE_NUM];
}GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations_Res_S;

typedef struct
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetVideoSourceConfigurationOptions_S;

typedef struct
{
    GONVIF_MEDIA_VideoSourceConfigurationOptions_S stOptions;
}GONVIF_MEDIA_GetVideoSourceConfigurationOptions_Res_S;

typedef struct 
{
    GONVIF_MEDIA_VideoSourceConfiguration_S stVideoSourceConfiguration;
    GONVIF_Boolean_E enForcePersistence;
}GONVIF_MEDIA_SetVideoSourceConfiguration_S;

typedef struct 
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_VideoEncoderConfiguration_S stConfigurations[MAX_VIDEOENCODE_NUM];
}GONVIF_MEDIA_GetVideoEncoderConfigurations_Res_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetVideoEncoderConfiguration_S;

typedef struct
{
    GONVIF_MEDIA_VideoEncoderConfiguration_S stConfiguration;
}GONVIF_MEDIA_GetVideoEncoderConfiguration_Res_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations_S;

typedef struct 
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_VideoEncoderConfiguration_S stConfigurations[MAX_VIDEOENCODE_NUM];
}GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations_Res_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetVideoEncoderConfigurationOptions_S;

typedef struct
{
    GONVIF_MEDIA_VideoEncoderConfigurationOptions_S stOptions;
}GONVIF_MEDIA_GetVideoEncoderConfigurationOptions_Res_S;

typedef struct
{
    GONVIF_MEDIA_VideoEncoderConfiguration_S stConfiguration;
    GONVIF_Boolean_E enForcePersistence;
}GONVIF_MEDIA_SetVideoEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances_S;

typedef struct 
{
    GK_S32 totalNumber;
    GK_S32 JPEG;
    GK_S32 H264;
    GK_S32 MPEG4;
}GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances_Res_S;

typedef struct
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_AudioSourceConfiguration_S stConfigurations[MAX_AUDIOSOURCE_NUM];
}GONVIF_MEDIA_GetAudioSourceConfigurations_Res_S;

typedef struct
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetAudioSourceConfiguration_S;

typedef struct
{
    GONVIF_MEDIA_AudioSourceConfiguration_S stConfiguration;
}GONVIF_MEDIA_GetAudioSourceConfiguration_Res_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations_S;

typedef struct
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_AudioSourceConfiguration_S stConfigurations[MAX_AUDIOSOURCE_NUM];
}GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations_Res_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetAudioSourceConfigurationOptions_S;

typedef struct
{
    GONVIF_MEDIA_AudioSourceConfigurationOptions_S stOptions;
}GONVIF_MEDIA_GetAudioSourceConfigurationOptions_Res_S;

typedef struct 
{
    GONVIF_MEDIA_AudioSourceConfiguration_S stConfiguration;
    GONVIF_Boolean_E enForcePersistence;
}GONVIF_MEDIA_SetAudioSourceConfiguration_S;

typedef struct 
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_AudioEncoderConfiguration_S stConfigurations[MAX_AUDIOENCODE_NUM];
}GONVIF_MEDIA_GetAudioEncoderConfigurations_Res_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetAudioEncoderConfiguration_S;

typedef struct
{
    GONVIF_MEDIA_AudioEncoderConfiguration_S stConfiguration;
}GONVIF_MEDIA_GetAudioEncoderConfiguration_Res_S;

typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations_S;

typedef struct 
{
    GK_S32 sizeConfigurations;
    GONVIF_MEDIA_AudioEncoderConfiguration_S stConfigurations[MAX_AUDIOENCODE_NUM];
}GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations_Res_S;

typedef struct 
{
    GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetAudioEncoderConfigurationOptions_S;

typedef struct
{
    GONVIF_MEDIA_AudioEncoderConfigurationOptions_S stOptions;
}GONVIF_MEDIA_GetAudioEncoderConfigurationOptions_Res_S;

typedef struct 
{
    GONVIF_MEDIA_AudioEncoderConfiguration_S stConfiguration;
    GONVIF_Boolean_E enForcePersistence;
}GONVIF_MEDIA_SetAudioEncoderConfiguration_S;

typedef struct 
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEIDA_GetSnapShotUri_S;

typedef struct
{
    GONVIF_MEDIA_MediaUri_S stMediaUri;
}GONVIF_MEIDA_GetSnapShotUri_Res_S;

typedef struct 
{
    GONVIF_MEDIA_StreamSetup_S  stStreamSetup;
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_MEDIA_GetStreamUri_S;

typedef struct
{
    GONVIF_MEDIA_MediaUri_S stMediaUri;
}GONVIF_MEDIA_GetStreamUri_Res_S;



typedef ONVIF_RET (*GONVIF_MEDIA_GetServiceCapabilities)(GONVIF_MEDIA_GetServiceCapabilities_Res_S *pstMD_GetServiceCapabilitiesRes);
typedef ONVIF_RET (*GONVIF_MEDIA_CreateProfile)(GONVIF_MEDIA_CreateProfile_S *pstMD_CreateProfile, GONVIF_MEDIA_CreateProfile_Res_S *pstMD_CreateProfileRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetProfile)(GONVIF_MEDIA_GetProfile_S *pstMD_GetProfile, GONVIF_MEDIA_GetProfile_Res_S *pstMD_GetProfileRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetProfiles)(GONVIF_MEDIA_GetProfiles_Res_S *pstMD_GetProfilesRes);
typedef ONVIF_RET (*GONVIF_MEDIA_DeleteProfile)(GONVIF_MEDIA_DeleteProfile_S *pstMD_DeleteProfile);

typedef ONVIF_RET (*GONVIF_MEDIA_AddVideoSourceConfiguration)(GONVIF_MEDIA_AddVideoSourceConfiguration_S *pstMD_AddVideoSourceConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_AddVideoEncoderConfiguration)(GONVIF_MEDIA_AddVideoEncoderConfiguration_S *pstMD_AddVideoEncoderConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_AddAudioSourceConfiguration)(GONVIF_MEDIA_AddAudioSourceConfiguration_S *pstMD_AddAudioSourceConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_AddAudioEncoderConfiguration)(GONVIF_MEDIA_AddAudioEncoderConfiguration_S *pstMD_AddAudioEncoderConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_AddPTZConfiguration)(GONVIF_MEDIA_AddPTZConfiguration_S *pstMD_AddPTZConfiguration);

typedef ONVIF_RET (*GONVIF_MEDIA_RemoveVideoSourceConfiguration)(GONVIF_MEDIA_RemoveVideoSourceConfiguration_S *pstMD_RemoveVideoSourceConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_RemoveVideoEncoderConfiguration)(GONVIF_MEDIA_RemoveVideoEncoderConfiguration_S *pstMD_RemoveVideoEncoderConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_RemoveAudioSourceConfiguration)(GONVIF_MEDIA_RemoveAudioSourceConfiguration_S *pstMD_RemoveAudioSourceConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_RemoveAudioEncoderConfiguration)(GONVIF_MEDIA_RemoveAudioEncoderConfiguration_S *pstMD_RemoveAudioEncoderConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_RemovePTZConfiguration)(GONVIF_MEDIA_RemovePTZConfiguration_S *pstMD_RemovePTZConfiguration);


typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoSourceConfigurations)(GONVIF_MEDIA_GetVideoSourceConfigurations_Res_S *pstMD_GetVideoSourceConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoSourceConfiguration)(GONVIF_MEDIA_GetVideoSourceConfiguration_S *pstMD_GetVideoSourceConfiguration, GONVIF_MEDIA_GetVideoSourceConfiguration_Res_S *pstMD_GetVideoSourceConfigurationRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations)(GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations_S *pstMD_GetCompatibleVideoSourceConfigurations, GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations_Res_S *pstMD_GetCompatibleVideoSourceConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoSourceConfigurationOptions)(GONVIF_MEDIA_GetVideoSourceConfigurationOptions_S *pstMD_GetVideoSourceConfigurationOptions, GONVIF_MEDIA_GetVideoSourceConfigurationOptions_Res_S *pstMD_GetVideoSourceConfigurationOptionsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_SetVideoSourceConfiguration)(GONVIF_MEDIA_SetVideoSourceConfiguration_S *pstMD_SetVideoSourceConfiguration);

typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoEncoderConfigurations)(GONVIF_MEDIA_GetVideoEncoderConfigurations_Res_S *pstMD_GetVideoEncoderConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoEncoderConfiguration)(GONVIF_MEDIA_GetVideoEncoderConfiguration_S *pstMD_GetVideoEncoderConfiguration, GONVIF_MEDIA_GetVideoEncoderConfiguration_Res_S *pstMD_GetVideoEncoderConfigurationRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations)(GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations_S *pstMD_GetCompatibleVideoEncoderConfigurations, GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations_Res_S *pstMD_GetCompatibleVideoEncoderConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetVideoEncoderConfigurationOptions)(GONVIF_MEDIA_GetVideoEncoderConfigurationOptions_S *pstMD_GetVideoEncoderConfigurationOptions, GONVIF_MEDIA_GetVideoEncoderConfigurationOptions_Res_S *pstMD_GetVideoEncoderConfigurationOptionsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_SetVideoEncoderConfiguration)(GONVIF_MEDIA_SetVideoEncoderConfiguration_S *pstMD_etVideoEncoderConfiguration);
typedef ONVIF_RET (*GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances)(GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances_S *pstMD_GetGuaranteedNumberOfVideoEncoderInstances, GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances_Res_S *pstMD_GetGuaranteedNumberOfVideoEncoderInstancesRes);

typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioSourceConfigurations)(GONVIF_MEDIA_GetAudioSourceConfigurations_Res_S *pstMD_GetAudioSourceConfigurationRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioSourceConfiguration)(GONVIF_MEDIA_GetAudioSourceConfiguration_S *pstMD_GetAudioSourceConfiguration, GONVIF_MEDIA_GetAudioSourceConfiguration_Res_S *pstMD_GetAudioSourceConfigurationRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations)(GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations_S *pstMD_GetCompatibleAudioSourceConfigurations, GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations_Res_S *pstMD_GetCompatibleAudioSourceConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioSourceConfigurationOptions)(GONVIF_MEDIA_GetAudioSourceConfigurationOptions_S *pstMD_GetAudioSourceConfigurationOptions, GONVIF_MEDIA_GetAudioSourceConfigurationOptions_Res_S *pstMD_GetAudioSourceConfigurationOptionsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_SetAudioSourceConfiguration)(GONVIF_MEDIA_SetAudioSourceConfiguration_S *pstMD_SetAudioSourceConfiguration);

typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioEncoderConfigurations)(GONVIF_MEDIA_GetAudioEncoderConfigurations_Res_S *pstMD_GetAudioEncoderConfigurationsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioEncoderConfiguration)(GONVIF_MEDIA_GetAudioEncoderConfiguration_S *pstMD_GetAudioEncoderConfiguration, GONVIF_MEDIA_GetAudioEncoderConfiguration_Res_S *pstMD_GetAudioEncoderConfigurationRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations)(GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations_S *pstGetCompAECfgsReq,GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations_Res_S *pstAECfgsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetAudioEncoderConfigurationOptions)(GONVIF_MEDIA_GetAudioEncoderConfigurationOptions_S *pstMD_GetAudioEncoderConfigurationOptions, GONVIF_MEDIA_GetAudioEncoderConfigurationOptions_Res_S *pstMD_GetAudioEncoderConfigurationOptionsRes);
typedef ONVIF_RET (*GONVIF_MEDIA_SetAudioEncoderConfiguration)(GONVIF_MEDIA_SetAudioEncoderConfiguration_S *pstMD_SetAudioEncoderConfiguration);

typedef ONVIF_RET (*GONVIF_MEDIA_GetStreamUri)(GONVIF_MEDIA_GetStreamUri_S *pstMD_GetStreamUri, GONVIF_MEDIA_GetStreamUri_Res_S *pstMMD_GetStreamUriRes);
typedef ONVIF_RET (*GONVIF_MEDIA_GetSnapshotUri)(GONVIF_MEIDA_GetSnapShotUri_S *pstMD_GetSnapShotUri, GONVIF_MEIDA_GetSnapShotUri_Res_S *pstMD_GetSnapShotUriRes);


typedef struct
{
    GONVIF_MEDIA_GetServiceCapabilities pfnMD_GetServiceCapabilities;
    GONVIF_MEDIA_CreateProfile pfnMD_CreateProfile;
    GONVIF_MEDIA_GetProfile    pfnMD_GetProfile;
    GONVIF_MEDIA_GetProfiles   pfnMD_GetProfiles;
    GONVIF_MEDIA_DeleteProfile pfnMD_DeleteProfile;
    
    GONVIF_MEDIA_AddVideoSourceConfiguration  pfnMD_AddVideoSourceConfiguration;    
    GONVIF_MEDIA_AddVideoEncoderConfiguration pfnMD_AddVideoEncoderConfiguration;
    GONVIF_MEDIA_AddAudioSourceConfiguration  pfnMD_AddAudioSourceConfiguration;
    GONVIF_MEDIA_AddAudioEncoderConfiguration pfnMD_AddAudioEncoderConfiguration;
    GONVIF_MEDIA_AddPTZConfiguration          pfnMD_AddPTZConfiguration;
    
    GONVIF_MEDIA_RemoveVideoSourceConfiguration  pfnMD_RemoveVideoSourceConfiguration;
    GONVIF_MEDIA_RemoveVideoEncoderConfiguration pfnMD_RemoveVideoEncoderConfiguration;
    GONVIF_MEDIA_RemoveAudioSourceConfiguration  pfnMD_RemoveAudioSourceConfiguration;
    GONVIF_MEDIA_RemoveAudioEncoderConfiguration pfnMD_RemoveAudioEncoderConfiguration;
    GONVIF_MEDIA_RemovePTZConfiguration          pfnMD_RemovePTZConfiguration;
    
    GONVIF_MEDIA_GetVideoSourceConfigurations           pfnMD_GetVideoSourceConfigurations;
    GONVIF_MEDIA_GetVideoSourceConfiguration            pfnMD_GetVideoSourceConfiguration;
    GONVIF_MEDIA_GetCompatibleVideoSourceConfigurations pfnMD_GetCompatibleVideoSourceConfigurations;
    GONVIF_MEDIA_GetVideoSourceConfigurationOptions     pfnMD_GetVideoSourceConfigurationOptions;
    GONVIF_MEDIA_SetVideoSourceConfiguration            pfnMD_SetVideoSourceConfiguration;
    
    GONVIF_MEDIA_GetVideoEncoderConfigurations              pfnMD_GetVideoEncoderConfigurations;
    GONVIF_MEDIA_GetVideoEncoderConfiguration               pfnMD_GetVideoEncoderConfiguration;
    GONVIF_MEDIA_GetCompatibleVideoEncoderConfigurations    pfnMD_GetCompatibleVideoEncoderConfigurations;
    GONVIF_MEDIA_GetVideoEncoderConfigurationOptions        pfnMD_GetVideoEncoderConfigurationOptions;
    GONVIF_MEDIA_SetVideoEncoderConfiguration               pfnMD_SetVideoEncoderConfiguration;
    GONVIF_MEDIA_GetGuaranteedNumberOfVideoEncoderInstances pfnMD_GetGuaranteedNumberOfVideoEncoderInstances;
    
    GONVIF_MEDIA_GetAudioSourceConfigurations           pfnMD_GetAudioSourceConfigurations;
    GONVIF_MEDIA_GetAudioSourceConfiguration            pfnMD_GetAudioSourceConfiguration;
    GONVIF_MEDIA_GetCompatibleAudioSourceConfigurations pfnMD_GetCompatibleAudioSourceConfigurations;
    GONVIF_MEDIA_GetAudioSourceConfigurationOptions     pfnMD_GetAudioSourceConfigurationOptions;
    GONVIF_MEDIA_SetAudioSourceConfiguration            pfnMD_SetAudioSourceConfiguration;

    GONVIF_MEDIA_GetAudioEncoderConfigurations           pfnMD_GetAudioEncoderConfigurations;
    GONVIF_MEDIA_GetAudioEncoderConfiguration            pfnMD_GetAudioEncoderConfiguration;
    GONVIF_MEDIA_GetCompatibleAudioEncoderConfigurations pfnMD_GetCompatibleAudioEncoderConfigurations;
    GONVIF_MEDIA_GetAudioEncoderConfigurationOptions     pfnMD_GetAudioEncoderConfigurationOptions;
    GONVIF_MEDIA_SetAudioEncoderConfiguration            pfnMD_SetAudioEncoderConfiguration;
    
    GONVIF_MEDIA_GetSnapshotUri pfnMD_GetSnapshotUri;
    GONVIF_MEDIA_GetStreamUri   pfnMD_GetStreamUri;
}GOnvif_MEDIA_CallbackFunc_S;


#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif 

