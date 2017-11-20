   /*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_image.h
**
** \brief       onvif image
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/


#ifndef _ONVIF_IMAGE_H_
#define _ONVIF_IMAGE_H_

#include "onvif_std.h"

/******************************************************/
/*capability*/
typedef struct
{
    GONVIF_Boolean_E enImageStabilization;
}GONVIF_IMAGE_Capabilities_S;

/*****************************************************/
//backlight compensation
typedef enum
{
    BacklightCompensationMode_OFF = 0,
    BacklightCompensationMode_ON  = 1
}GONVIF_IMAGE_BackLightCompensation_Mode_E;

typedef struct
{
	GONVIF_IMAGE_BackLightCompensation_Mode_E enMode;
	GK_FLOAT level;
}GONVIF_IMAGE_BackLightCompensation_S;
//exposure
typedef enum
{
    ExposureMode_AUTO   = 0,
    ExposureMode_MANUAL = 1
}GONVIF_IMAGE_Exposure_Mode_E;
/*

typedef enum
{
    ExposurePriority_LowNoise  = 0,
    ExposurePriority_FrameRate = 1
}GONVIF_IMAGE_ExposurePriority_E;

typedef struct
{
	GK_FLOAT bottom;
	GK_FLOAT top;
	GK_FLOAT right;
	GK_FLOAT left;
}GONVIF_IMAGE_ExposureRectangle_S;
*/
typedef struct
{
	GONVIF_IMAGE_Exposure_Mode_E enMode;
	//GONVIF_IMAGE_ExposurePriority_E pePriority;
	//GONVIF_IMAGE_ExposureRectangle_S pstWindow;
	GK_FLOAT minExposureTime;
	GK_FLOAT maxExposureTime;
	GK_FLOAT minGain;
	GK_FLOAT maxGain;
	//GK_FLOAT MinIris;
	//GK_FLOAT MaxIris;
	GK_FLOAT exposureTime;
	GK_FLOAT gain;
	//GK_FLOAT iris;
}GONVIF_IMAGE_Exposure_S;
//focus
typedef enum
{
    AutoFocusMode_AUTO = 0,
    AutoFocusMode_MANUAL = 1
}GONVIF_IMAGE_Focus_Mode_E;

typedef struct
{
	GONVIF_IMAGE_Focus_Mode_E enAutoFocusMode;
	GK_FLOAT defaultSpeed;
	GK_FLOAT nearLimit;
	GK_FLOAT farLimit;
}GONVIF_IMAGE_FocusConfiguration_S;
//IrCutFilter
typedef enum
{
    IrCutFilterMode_ON = 0,
    IrCutFilterMode_OFF = 1,
    IrCutFilterMode_AUTO = 2
}GONVIF_IMAGE_IrCutFilter_Mode_E;
//WideDynamicRange
typedef enum
{
    WideDynamicMode_OFF = 0,
    WideDynamicMode_ON = 1
}GONVIF_IMAGE_WideDynamicRange_Mode_E;

typedef struct
{
	GONVIF_IMAGE_WideDynamicRange_Mode_E enMode;
	GK_FLOAT level;
}GONVIF_IMAGE_WideDynamicRange_S;
//WhiteBalance
typedef enum
{
    WhiteBalanceMode_AUTO = 0,
    WhiteBalanceMode_MANUAL = 1
}GONVIF_IMAGE_WhiteBalance_Mode_E;

typedef struct
{
	GONVIF_IMAGE_WhiteBalance_Mode_E enMode;
	GK_FLOAT crGain;
	GK_FLOAT cbGain;
}GONVIF_IMAGE_WhiteBalance_S;
//focus move
typedef struct
{
	GK_FLOAT *position;
	GK_FLOAT *speed;
}GONVIF_IMAGE_Focus_Absolute_S;

typedef struct
{
	GK_FLOAT *distance;
	GK_FLOAT *speed;
}GONVIF_IMAGE_Focus_Relative_S;

typedef struct
{
	GK_FLOAT *speed;
}GONVIF_IMAGE_Focus_Continuous_S;

typedef struct
{
	GONVIF_IMAGE_Focus_Absolute_S *stAbsolute;
	GONVIF_IMAGE_Focus_Relative_S *stRelative;
	GONVIF_IMAGE_Focus_Continuous_S *stContinuous;
}GONVIF_IMAGE_Focus_Move_S;
//

/******************************************************/
/*settings*/
typedef struct
{
	GONVIF_IMAGE_BackLightCompensation_S stBacklightCompensation;
	GK_FLOAT brightness;
	GK_FLOAT colorSaturation;
	GK_FLOAT contrast;
	GONVIF_IMAGE_Exposure_S stExposure;
	GONVIF_IMAGE_FocusConfiguration_S stFocus;
	GONVIF_IMAGE_IrCutFilter_Mode_E enIrCutFilter;
	GK_FLOAT sharpness;
	GONVIF_IMAGE_WideDynamicRange_S stWideDynamicRange;
	GONVIF_IMAGE_WhiteBalance_S stWhiteBalance;
}GONVIF_IMAGE_Settings_S;

typedef struct
{
	GONVIF_IMAGE_Settings_S stImageSettings;
}GONVIF_IMAGE_GetSettings_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
	GONVIF_IMAGE_Settings_S stImageSettings;
	GONVIF_Boolean_E enForcePersistence;
}GONVIF_IMAGE_SetSettings_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
}GONVIF_IMAGE_GetSettings_Token_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
}GONVIF_IMAGE_GetOptions_Token_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
}GONVIF_IMAGE_Stop_Token_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
}GONVIF_IMAGE_GetStatus_Token_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
}GONVIF_IMAGE_GetMoveOptions_Token_S;

typedef struct
{
	GK_CHAR aszVideoSourceToken[MAX_TOKEN_LENGTH];
	GONVIF_IMAGE_Focus_Move_S stFocus;
}GONVIF_IMAGE_Move_S;


/*****************************************************/
//backlight compensation options
typedef struct
{
	GK_S32 sizeMode;
	GONVIF_IMAGE_BackLightCompensation_Mode_E *penMode;
	GONVIF_FloatRange_S stLevel;
}GONVIF_IMAGE_BackLightCompensation_Options_S;
//exposure options
typedef struct
{
	GK_S32 sizeMode;
	GONVIF_IMAGE_Exposure_Mode_E *penMode;
	GONVIF_FloatRange_S stMinExposureTime;
	GONVIF_FloatRange_S stMaxExposureTime;
	GONVIF_FloatRange_S stMinGain;
	GONVIF_FloatRange_S stMaxGain;
	//GONVIF_FloatRange_S stMinIris;
	//GONVIF_FloatRange_S stMaxIris;
	GONVIF_FloatRange_S stExposureTime;
	GONVIF_FloatRange_S stGain;
	//GONVIF_FloatRange_S stIris;
}GONVIF_IMAGE_Exposure_Option_S;
//focus options
typedef struct
{
	GK_S32 sizeAutoFocusModes;
	GONVIF_IMAGE_Focus_Mode_E *penAutoFocusModes;
	GONVIF_FloatRange_S stDefaultSpeed;
	GONVIF_FloatRange_S stNearLimit;
	GONVIF_FloatRange_S stFarLimit;
}GONVIF_IMAGE_Focus_Options_S;
//wide dynamic range options
typedef struct
{
	GK_S32 sizeMode;
	GONVIF_IMAGE_WideDynamicRange_Mode_E *penMode;
	GONVIF_FloatRange_S stLevel;
}GONVIF_IMAGE_WideDynamicRange_Options_S;
//white balance options
typedef struct
{
	GK_S32 sizeMode;
	GONVIF_IMAGE_WhiteBalance_Mode_E *penMode;
	GONVIF_FloatRange_S stYrGain;
	GONVIF_FloatRange_S stYbGain;
}GONVIF_IMAGE_WhiteBalance_Options_S;
//move option
typedef struct
{
	GONVIF_FloatRange_S stPosition;
	GONVIF_FloatRange_S stSpeed;
}GONVIF_IMAGE_FocusAbsoluteOptions_S;

typedef struct
{
	GONVIF_FloatRange_S stDistance;
	GONVIF_FloatRange_S stSpeed;
}GONVIF_IMAGE_FocusRelativeOptions_S;

typedef struct
{
	GONVIF_FloatRange_S stSpeed;
}GONVIF_IMAGE_FocusContinuousOptions_S;
/*****************************************************/
/*options*/
typedef struct
{
	GONVIF_IMAGE_BackLightCompensation_Options_S stBacklightCompensation;
	GONVIF_FloatRange_S stBrightness;
	GONVIF_FloatRange_S stColorSaturation;
	GONVIF_FloatRange_S stContrast;
	GONVIF_IMAGE_Exposure_Option_S stExposure;
	GONVIF_IMAGE_Focus_Options_S stFocus;
	GK_S32 sizeIrCutFilterModes;
	GONVIF_IMAGE_IrCutFilter_Mode_E *penIrCutFilterModes;
	GONVIF_FloatRange_S stSharpness;
	GONVIF_IMAGE_WideDynamicRange_Options_S stWideDynamicRange;
	GONVIF_IMAGE_WhiteBalance_Options_S stWhiteBalance;
}GONVIF_IMAGE_Options_S;

typedef struct
{
    GONVIF_IMAGE_Options_S stGetImageOptions;
}GONVIF_IMAGE_GetOptions_S;

typedef struct
{
	GONVIF_IMAGE_FocusAbsoluteOptions_S stAbsolute;
	GONVIF_IMAGE_FocusRelativeOptions_S stRelative;
	GONVIF_IMAGE_FocusContinuousOptions_S stContinuous;
}GONVIF_IMAGE_MoveOptions_S;

typedef struct
{
	GONVIF_IMAGE_MoveOptions_S stMoveOptions;
}GONVIF_IMAGE_GetMoveOptions_S;

/*****************************************************/
/*image status*/
typedef enum
{
    MoveStatus_IDLE = 0,
    MoveStatus_MOVING = 1,
    MoveStatus_UNKNOWN = 2
}GONVIF_IMAGE_MoveStatus_E;

typedef struct
{
	GK_FLOAT position;
	GONVIF_IMAGE_MoveStatus_E enMoveStatus;
	GK_CHAR aszError[MAX_ERROR_LENGTH];
}GONVIF_IMAGE_FocusStatus_S;

typedef struct
{
	GONVIF_IMAGE_FocusStatus_S stFocusStatus;
}GONVIF_IMAGE_Status_S;

typedef struct
{
	GONVIF_IMAGE_Status_S stImageStatus;
}GONVIF_IMAGE_GetStatus_S;

/******************************************************************************/
//GetServiceCapabilities
typedef struct
{
	GONVIF_IMAGE_Capabilities_S stCapabilities;
}GONVIF_IMAGING_GetServiceCapabilities_Res_S;

//GetImagingSettings

//SetImagingSettings

//GetOptions

//Move

//Stop

//GetStatus

//GetMoveOptions


/******************************************************************************/
typedef ONVIF_RET (*GONVIF_IMAGE_GetServiceCapabilities)(GONVIF_IMAGING_GetServiceCapabilities_Res_S *pstIMG_GetServiceCapabilitiesRes);
typedef ONVIF_RET (*GONVIF_IMAGE_GetImagingSettings)(GONVIF_IMAGE_GetSettings_Token_S *pstGetImagingSettingsReq, GONVIF_IMAGE_GetSettings_S *pstGetImagingSettingsRes);
typedef ONVIF_RET (*GONVIF_IMAGE_SetImagingSettings)(GONVIF_IMAGE_SetSettings_S *pstSetImagingSettingsReq);
typedef ONVIF_RET (*GONVIF_IMAGE_GetOptions)(GONVIF_IMAGE_GetOptions_Token_S *pstGetImageOptionsReq, GONVIF_IMAGE_GetOptions_S *pstGetImageOptionsRes);
typedef ONVIF_RET (*GONVIF_IMAGE_Move)(GONVIF_IMAGE_Move_S *pstMoveReq);
typedef ONVIF_RET (*GONVIF_IMAGE_Stop)(GONVIF_IMAGE_Stop_Token_S *pstStopReq);
typedef ONVIF_RET (*GONVIF_IMAGE_GetStatus)(GONVIF_IMAGE_GetStatus_Token_S *pstGetStatusReq, GONVIF_IMAGE_GetStatus_S *pstGetStatusRes);
typedef ONVIF_RET (*GONVIF_IMAGE_GetMoveOptions)(GONVIF_IMAGE_GetMoveOptions_Token_S *pstGetMoveOptionsReq, GONVIF_IMAGE_GetMoveOptions_S *pstGetMoveOptionsRes);



typedef struct 
{
    GONVIF_IMAGE_GetServiceCapabilities pfnIMG_GetServiceCapabilities;
    GONVIF_IMAGE_GetImagingSettings pfnGetImagingSettings;
    GONVIF_IMAGE_SetImagingSettings pfnSetImagingSettings;
    GONVIF_IMAGE_GetOptions pfnGetOptions;
    GONVIF_IMAGE_Move pfnMove;
    GONVIF_IMAGE_Stop pfnPTZ_Stop;
    GONVIF_IMAGE_GetStatus pfnGetStatus;
    GONVIF_IMAGE_GetMoveOptions pfnGetMoveOptions;
}GOnvif_IMAGE_CallbackFunc_S;



#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif  /* _ONVIF_IMAGE_H_ */

