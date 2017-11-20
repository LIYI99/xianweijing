   /*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_ptz.h
**
** \brief       onvif ptz
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/


#ifndef _ONVIF_PTZ_H_
#define _ONVIF_PTZ_H_

#include "onvif_std.h"

#define MAX_AB_PT_SPACE     2
#define MAX_AB_ZO_SPACE     2
#define MAX_RE_PT_SPACE     2
#define MAX_RE_ZO_SPACE     2
#define MAX_CO_PT_SPACE     2
#define MAX_CO_ZO_SPACE     2
#define MAX_PT_SPEED_SPACE  2
#define MAX_ZO_SPEED_SPACE  2


/******************************************************/
//capabilities
typedef struct
{
	GONVIF_Boolean_E enEFlip;
	GONVIF_Boolean_E enReverse;
	GONVIF_Boolean_E enGetCompatibleConfigurations;
}GONVIF_PTZ_Capabilities_S;

//node
typedef struct
{
	GK_CHAR aszURI[MAX_URI_LENGTH];
	GONVIF_FloatRange_S stXRange;
	GONVIF_FloatRange_S stYRange;
}GONVIF_PTZ_Space2DDescription_S;

typedef struct
{
	GK_CHAR aszURI[MAX_URI_LENGTH];
	GONVIF_FloatRange_S stXRange;
}GONVIF_PTZ_Space1DDescription_S;

typedef struct
{
    //range for absolute space
	GK_S32 sizeAbsolutePanTiltPositionSpace;
	GONVIF_PTZ_Space2DDescription_S stAbsolutePanTiltPositionSpace[MAX_AB_PT_SPACE];//abstraction of coordinate systems
	GK_S32 sizeAbsoluteZoomPositionSpace;
	GONVIF_PTZ_Space1DDescription_S stAbsoluteZoomPositionSpace[MAX_AB_ZO_SPACE];//abstraction of coordinate systems
	//range for relative space
	GK_S32 sizeRelativePanTiltTranslationSpace;
	GONVIF_PTZ_Space2DDescription_S stRelativePanTiltTranslationSpace[MAX_RE_PT_SPACE];//abstraction of coordinate systems
	GK_S32 sizeRelativeZoomTranslationSpace;
	GONVIF_PTZ_Space1DDescription_S stRelativeZoomTranslationSpace[MAX_RE_ZO_SPACE];//abstraction of coordinate systems
	//range for continuous space
	GK_S32 sizeContinuousPanTiltVelocitySpace;
	GONVIF_PTZ_Space2DDescription_S stContinuousPanTiltVelocitySpace[MAX_CO_PT_SPACE];//abstraction of coordinate systems
	GK_S32 sizeContinuousZoomVelocitySpace;
	GONVIF_PTZ_Space1DDescription_S stContinuousZoomVelocitySpace[MAX_CO_ZO_SPACE];//abstraction of coordinate systems
	//range for speed
	GK_S32 sizePanTiltSpeedSpace;
	GONVIF_PTZ_Space1DDescription_S stPanTiltSpeedSpace[MAX_PT_SPEED_SPACE];
	GK_S32 sizeZoomSpeedSpace;
	GONVIF_PTZ_Space1DDescription_S stZoomSpeedSpace[MAX_ZO_SPEED_SPACE];
}GONVIF_PTZ_Spaces_S;

typedef enum
{
    PTZPresetTourOperation_Start    = 0,
    PTZPresetTourOperation_Stop     = 1,
    PTZPresetTourOperation_Pause    = 2,
    PTZPresetTourOperation_Extended = 3
}GONVIF_PTZ_PresetToutOperation_E;

typedef struct
{
	GK_S32 maximumNumberOfPresetTours;
	GK_S32 sizePTZPresetTourOperation;
	GONVIF_PTZ_PresetToutOperation_E enPTZPresetTourOperation[MAX_PRESET_TOUR_OPERATION];
}GONVIF_PTZ_PresetTourSupported_S;

typedef struct
{
	GONVIF_PTZ_PresetTourSupported_S stSupportedPresetTour;
}GONVIF_PTZ_NodeExtension_S;

typedef struct
{
    GK_CHAR token[MAX_TOKEN_LENGTH];
	GK_CHAR name[MAX_NAME_LENGTH];
	GONVIF_PTZ_Spaces_S stSupportedPTZSpaces;
    GK_S32 maximumNumberOfPresets;
    GONVIF_Boolean_E enHomeSupported;
    //GK_S32 SizeAuxiliaryCommands;
    //GK_CHAR **AuxiliaryCommands;
	GONVIF_PTZ_NodeExtension_S stExtension;
    GONVIF_Boolean_E enFixedHomePosition;
}GONVIF_PTZ_Node_S;

//configurations
typedef struct
{
    GK_FLOAT x;
    GK_FLOAT y;
    GK_CHAR aszSpace[MAX_SPACE_LENGTH];
}GONVIF_PTZ_Vector2D_S;

typedef struct
{
    GK_FLOAT x;
    GK_CHAR aszSpace[MAX_SPACE_LENGTH];
}GONVIF_PTZ_Vector1D_S;

typedef struct
{
	GONVIF_PTZ_Vector2D_S stPanTilt;
	GONVIF_PTZ_Vector1D_S stZoom;
}GONVIF_PTZ_Speed_S;

typedef struct
{
	GONVIF_PTZ_Space2DDescription_S stRange;
}GONVIF_PTZ_PanTiltLimits_S;

typedef struct
{
	GONVIF_PTZ_Space1DDescription_S stRange;
}GONVIF_PTZ_ZoomLimits_S;

typedef struct
{
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_S32  useCount;
	GK_CHAR token[MAX_TOKEN_LENGTH];
	GK_CHAR nodeToken[MAX_TOKEN_LENGTH];
	GK_CHAR defaultAbsolutePantTiltPositionSpace[MAX_URI_LENGTH];
	GK_CHAR defaultAbsoluteZoomPositionSpace[MAX_URI_LENGTH];
	GK_CHAR defaultRelativePanTiltTranslationSpace[MAX_URI_LENGTH];
	GK_CHAR defaultRelativeZoomTranslationSpace[MAX_URI_LENGTH];
	GK_CHAR defaultContinuousPanTiltVelocitySpace[MAX_URI_LENGTH];
	GK_CHAR defaultContinuousZoomVelocitySpace[MAX_URI_LENGTH];
	GONVIF_PTZ_Speed_S stDefaultPTZSpeed;        //related to absolute or relative movement
	GK_S64 defaultPTZTimeout;                   //related to continuous movement
	GONVIF_PTZ_PanTiltLimits_S stPanTiltLimits;
	GONVIF_PTZ_ZoomLimits_S stZoomLimits;
}GONVIF_PTZ_Configuration_S;

typedef enum
{
    ReverseMode_OFF = 0,
    ReverseMode_ON = 1,
    ReverseMode_AUTO = 2,
    ReverseMode_EXTENDED = 3
}GONVIF_PTZ_ReverseMode_E;

typedef enum
{
    EFlipMode_OFF = 0,
    EFlipMode_ON = 1,
    EFlipMode_EXTENDED = 2
}GONVIF_PTZ_EFlipMode_E;

typedef struct
{
	GK_S32 sizeMode;
	GONVIF_PTZ_EFlipMode_E enMode[MAX_NUM_EFLIP_MODE];
}GONVIF_PTZ_EFlipOptions_S;

typedef struct
{
	GK_S32 sizeMode;
	GONVIF_PTZ_ReverseMode_E enMode[MAX_NUM_REVERSE_MODE];
}GONVIF_PTZ_ReverseOptions_S;

typedef struct
{
	GONVIF_PTZ_EFlipOptions_S stEFlip;
	GONVIF_PTZ_ReverseOptions_S stReverse;
}GONVIF_PTZ_PTControlDirectionOptions_S;


typedef struct
{
	GK_S64 min;
	GK_S64 max;
}GONVIF_PTZ_DurationRange_S;

typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_GetStatus_Token_S;

typedef enum
{
    PTZ_MoveStatus_IDLE = 0,
    PTZ_MoveStatus_MOVING = 1,
    PTZ_MoveStatus_UNKNOWN = 2
}GONVIF_PTZ_MoveStatus_E;


typedef struct
{
    GONVIF_PTZ_Vector2D_S stPanTilt;
    GONVIF_PTZ_Vector1D_S stZoom;
}GONVIF_PTZ_Vector_S;

//status
typedef struct
{
    GONVIF_PTZ_MoveStatus_E enPanTilt;
    GONVIF_PTZ_MoveStatus_E enZoom;
}GONVIF_PTZ_MoveStatus_S;

typedef struct
{
    GONVIF_PTZ_Vector_S  stPosition;
    GONVIF_PTZ_MoveStatus_S stMoveStatus;
    GK_CHAR aszError[MAX_ERROR_LENGTH];
    GK_CHAR aszUtcTime[MAX_TIME_LEN];
}GONVIF_PTZ_GetStatus_S;

typedef struct
{
    GK_CHAR aszName[MAX_NAME_LENGTH];
    GONVIF_PTZ_Vector_S  stPTZPosition;	//absolute position
    GK_CHAR aszToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_Preset_S;

typedef struct
{
	GONVIF_PTZ_Spaces_S stSpaces;
	GONVIF_PTZ_DurationRange_S stPTZTimeout;
	GONVIF_PTZ_PTControlDirectionOptions_S stPTControlDirection;
}GONVIF_PTZ_ConfigurationOptions_S;


/******************************************************************************/
//GetServiceCapabilities
typedef struct
{
	GONVIF_PTZ_Capabilities_S stCapabilities;
}GONVIF_PTZ_GetServiceCapabilities_Res_S;

//GetNodes
typedef struct
{
	GK_S32 sizePTZNode;
	GONVIF_PTZ_Node_S stPTZNode[MAX_PTZ_NODE_NUM];
}GONVIF_PTZ_GetNodes_Res_S;

//GetNode
typedef struct
{
    GK_CHAR aszNodeToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_GetNode_S;

typedef struct
{
	GONVIF_PTZ_Node_S stPTZNode;
}GONVIF_PTZ_GetNode_Res_S;

//GetConfigurations
typedef struct
{
	GK_S32 sizePTZConfiguration;
	GONVIF_PTZ_Configuration_S stPTZConfiguration[MAX_PTZ_CONFIGURATION_NUM];
}GONVIF_PTZ_GetConfigutations_Res_S;

//GetConfiguration
typedef struct
{
    GK_CHAR aszPTZConfiguationToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_GetConfiguration_S;

typedef struct
{
	GONVIF_PTZ_Configuration_S stPTZConfiguration;
}GONVIF_PTZ_GetConfiguration_Res_S;

//GetConfigurationOptions
typedef struct
{
	GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_GetConfigurationOptions_S;

typedef struct
{
    GONVIF_PTZ_ConfigurationOptions_S stPTZConfigurationOptions;
}GONVIF_PTZ_GetConfigurationOptions_Res_S;

//SetConfiguration
typedef struct
{
	GONVIF_PTZ_Configuration_S stPTZConfiguration;
	GONVIF_Boolean_E enForcePersistence;
}GONVIF_PTZ_SetConfiguration_S;

//ContinuousMove
typedef struct
{
	GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
	GONVIF_PTZ_Speed_S stVelocity;
	GK_S64 timeout;
}GONVIF_PTZ_ContinuousMove_S;

//Stop
typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GONVIF_Boolean_E enPanTilt;
    GONVIF_Boolean_E enZoom;
}GONVIF_PTZ_Stop_S;

//GetPresets
typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_GetPresets_S;

typedef struct
{
    GK_S32 sizePreset;
    GONVIF_PTZ_Preset_S  stPreset[MAX_PRESET_NUM];
}GONVIF_PTZ_GetPresets_Res_S;

//SetPreset
typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszPresetName[MAX_NAME_LENGTH];
    GK_CHAR aszPresetToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_SetPreset_S;

typedef struct
{
    GK_CHAR aszPresetToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_SetPreset_Res_S;

//GotoPreset
typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszPresetToken[MAX_TOKEN_LENGTH];
    GONVIF_PTZ_Speed_S stSpeed;
}GONVIF_PTZ_GotoPreset_S;

//RemovePreset
typedef struct
{
    GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
    GK_CHAR aszPresetToken[MAX_TOKEN_LENGTH];
}GONVIF_PTZ_RemovePreset_S;

//AbsoluteMove
typedef struct
{
	GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
	GONVIF_PTZ_Vector_S stPosition;
	GONVIF_PTZ_Speed_S stSpeed;
}GONVIF_PTZ_AbsoluteMove_S;

//RelativeMove
typedef struct
{
	GK_CHAR aszProfileToken[MAX_TOKEN_LENGTH];
	GONVIF_PTZ_Vector_S stTranslation;
	GONVIF_PTZ_Speed_S stSpeed;
}GONVIF_PTZ_RelativeMove_S;

//GetStatus

/******************************************************************************/

typedef ONVIF_RET (*GONVIF_PTZ_GetServiceCapabilities)(GONVIF_PTZ_GetServiceCapabilities_Res_S *pstPTZ_GetServiceCapabilitiesRes);
typedef ONVIF_RET (*GONVIF_PTZ_GetNodes)(GONVIF_PTZ_GetNodes_Res_S *pstPTZ_GetNodesRes);
typedef ONVIF_RET (*GONVIF_PTZ_GetNode)(GONVIF_PTZ_GetNode_S *pstPTZ_GetNode, GONVIF_PTZ_GetNode_Res_S *pstPTZ_GetNodeRes);
typedef ONVIF_RET (*GONVIF_PTZ_GetConfigurations)(GONVIF_PTZ_GetConfigutations_Res_S *pstPTZ_GetConfigurationsRes);
typedef ONVIF_RET (*GONVIF_PTZ_GetConfiguration)(GONVIF_PTZ_GetConfiguration_S *pstPTZ_GetConfiguration, GONVIF_PTZ_GetConfiguration_Res_S *pstPTZ_GetConfigurationRes);
typedef ONVIF_RET (*GONVIF_PTZ_GetConfigurationOptions)(GONVIF_PTZ_GetConfigurationOptions_S *pstPTZ_GetConfigurationOptions, GONVIF_PTZ_GetConfigurationOptions_Res_S *pstPTZ_GetConfigurationOptionsRes);
typedef ONVIF_RET (*GONVIF_PTZ_SetConfiguration)(GONVIF_PTZ_SetConfiguration_S *pstPTZ_SetConfiguration);
typedef ONVIF_RET (*GONVIF_PTZ_ContinuousMove)(GONVIF_PTZ_ContinuousMove_S *pstPTZ_ContinuousMove);
typedef ONVIF_RET (*GONVIF_PTZ_Stop)(GONVIF_PTZ_Stop_S *pstPTZ_Stop);
typedef ONVIF_RET (*GONVIF_PTZ_GetPresets)(GONVIF_PTZ_GetPresets_S *pstPTZ_GetPresets, GONVIF_PTZ_GetPresets_Res_S *pstPTZ_GetPresetsRes);
typedef ONVIF_RET (*GONVIF_PTZ_SetPreset)(GONVIF_PTZ_SetPreset_S *pstPTZ_SetPreset, GONVIF_PTZ_SetPreset_Res_S *pstPTZ_SetPresetRes);
typedef ONVIF_RET (*GONVIF_PTZ_GotoPreset)(GONVIF_PTZ_GotoPreset_S *pstPTZ_GotoPreset);
typedef ONVIF_RET (*GONVIF_PTZ_RemovePreset)(GONVIF_PTZ_RemovePreset_S *pstPTZ_RemovePreset);





typedef ONVIF_RET (*GONVIF_PTZ_AbsoluteMove)(GONVIF_PTZ_AbsoluteMove_S *pstPTZ_AbsoluteMove);
typedef ONVIF_RET (*GONVIF_PTZ_RelativeMove)(GONVIF_PTZ_RelativeMove_S *pstPTZ_RelativeMove);
typedef ONVIF_RET (*GONVIF_PTZ_GetStatus)(GONVIF_PTZ_GetStatus_Token_S *pstGetStatusReq, GONVIF_PTZ_GetStatus_S *pstGetStatusRes);



typedef struct
{
    GONVIF_PTZ_GetServiceCapabilities pfnPTZ_GetServiceCapabilities;
    GONVIF_PTZ_GetNodes pfnPTZ_GetNodes;
    GONVIF_PTZ_GetNode  pfnPTZ_GetNode;
    GONVIF_PTZ_GetConfigurations pfnPTZ_GetConfigurations;
    GONVIF_PTZ_GetConfiguration  pfnPTZ_GetConfiguration;
    GONVIF_PTZ_GetConfigurationOptions pfnPTZ_GetConfigurationOptions;
    GONVIF_PTZ_SetConfiguration pfnPTZ_SetConfiguration;
    GONVIF_PTZ_ContinuousMove pfnPTZ_ContinuousMove;
    GONVIF_PTZ_Stop pfnPTZ_Stop;
    GONVIF_PTZ_GetPresets pfnPTZ_GetPresets;
    GONVIF_PTZ_SetPreset  pfnPTZ_SetPreset;
    GONVIF_PTZ_GotoPreset pfnPTZ_GotoPreset;
    GONVIF_PTZ_RemovePreset pfnPTZ_RemovePreset;
    
    GONVIF_PTZ_AbsoluteMove pfnPTZ_AbsoluteMove;
    GONVIF_PTZ_RelativeMove pfnPTZ_RelativeMove;
    GONVIF_PTZ_GetStatus pfnGetStatus;
} GOnvif_PTZ_CallbackFunc_S;



#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif



