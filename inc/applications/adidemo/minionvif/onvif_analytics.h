/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_analytics.h
**
** \brief       onvif analytics
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ONVIF_ANALYTICS_H_
#define _ONVIF_ANALYTICS_H_

#include "onvif_std.h"




//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************



typedef struct
{
	GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_ANALYTICS_GetSupportedAnalyticsModules_S;

typedef struct
{
	GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_ANALYTICS_GetAnalyticsModules_S;

typedef struct _tan__GetSupportedRules
{
	GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_ANALYTICS_GetSupportedRules_S;

typedef struct
{
	GK_CHAR aszConfigurationToken[MAX_TOKEN_LENGTH];
}GONVIF_ANALYTICS_GetRules_S;

typedef struct
{
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_CHAR type[MAX_TYPE_LENGTH];
}GONVIF_ANALYTICS_SimpleItemDescription_S;

typedef struct
{
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_CHAR type[MAX_TYPE_LENGTH];
}GONVIF_ANALYTICS_ElementItemDescription_S;

typedef struct
{
	GK_S32 sizeSimpleItemDescription;
	GONVIF_ANALYTICS_SimpleItemDescription_S stSimpleItemDescription[MAX_SIMPLE_NUM];
	GK_S32 sizeElementItemDescription;
	GONVIF_ANALYTICS_ElementItemDescription_S stElementItemDescription[MAX_ELEMENT_NUM];
}GONVIF_ANALYTICS_ItemListDescription_S;

typedef struct
{
	GONVIF_ANALYTICS_ItemListDescription_S stSource;
	GONVIF_ANALYTICS_ItemListDescription_S stKey;
	GONVIF_ANALYTICS_ItemListDescription_S stData;
	GONVIF_Boolean_E enIsProperty;
	GK_CHAR aszParentTopic[MAX_64_LENGTH];
}GONVIF_ANALYTICS_ConfigDescription_Messages_S;

typedef struct
{
	GONVIF_ANALYTICS_ItemListDescription_S stParameters;
	GK_S32 sizeMessages;
	GONVIF_ANALYTICS_ConfigDescription_Messages_S stMessages[MAX_MESSAGE_NUM];
	GK_CHAR name[MAX_NAME_LENGTH];
}GONVIF_ANALYTICS_ConfigDescription_S;

typedef struct
{
	GK_S32 sizeAnalyticsModuleContentSchemaLocation;
	GK_CHAR pszAnalyticsModuleContentSchemaLocation[MAX_LOCATION_NUM][MAX_64_LENGTH];
	GK_S32 sizeAnalyticsModuleDescription;
	GONVIF_ANALYTICS_ConfigDescription_S stAnalyticsModuleDescription[MAX_DESCRIPTION_NUM];
}GONVIF_ANALYTICS_SupportedAnalyticsModules_S;

typedef struct
{
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_CHAR value[MAX_256_LENGTH];
}GONVIF_ANALYTICS_ItemList_SimpleItem_S;

typedef struct
{
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_CHAR any[MAX_256_LENGTH];
}GONVIF_ANALYTICS_ItemList_ElementItem_S;

typedef struct
{
	GK_S32 sizeSimpleItem;
	GONVIF_ANALYTICS_ItemList_SimpleItem_S stSimpleItem[MAX_SIMPLE_NUM];
	GK_S32 sizeElementItem;
	GONVIF_ANALYTICS_ItemList_ElementItem_S stElementItem[MAX_ELEMENT_NUM];
}GONVIF_ANALYTICS_ItemList_S;

typedef struct
{
	GONVIF_ANALYTICS_ItemList_S stParameters;
	GK_CHAR name[MAX_NAME_LENGTH];
	GK_CHAR type[MAX_TYPE_LENGTH];
}GONVIF_ANALYTICS_Config_S;

typedef struct
{
	GK_S32 sizeRuleContentSchemaLocation;
	GK_CHAR pszRuleContentSchemaLocation[MAX_LOCATION_NUM][MAX_64_LENGTH];
	GK_S32 sizeRuleDescription;
	GONVIF_ANALYTICS_ConfigDescription_S stRuleDescription[MAX_DESCRIPTION_NUM];
}GONVIF_ANALYTICS_SupportedRules_S;

typedef struct
{
	GONVIF_ANALYTICS_SupportedAnalyticsModules_S stSupportedAnalyticsModules;
}GONVIF_ANALYTICS_GetSupportedAnalyticsModulesRes_S;

typedef struct
{
	GK_S32 sizeAnalyticsModule;
	GONVIF_ANALYTICS_Config_S stAnalyticsModule[MAX_CONFIG_NUM];
}GONVIF_ANALYTICS_GetAnalyticsModulesRes_S;


typedef struct
{
	GONVIF_ANALYTICS_SupportedRules_S stSupportedRules;
}GONVIF_ANALYITICS_GetSupportedRulesRes_S;

typedef struct
{
	GK_S32 sizeRule;
	GONVIF_ANALYTICS_Config_S stRule[MAX_CONFIG_NUM];
}GONVIF_ANALYTICS_GetRulesRes_s;

typedef struct 
{
    GK_S32 size;
    GONVIF_Boolean_E enRuleSupport;
    GONVIF_Boolean_E enAnalyticsModulesSupport;
    GONVIF_Boolean_E enCellBasedSceneDescriptionSupported;
}GONVIF_ANALYITICS_Capabilities_S;
/*******************************************************************************/
typedef struct 
{
    GONVIF_ANALYITICS_Capabilities_S stCapabilities;
}GONVIF_ANALYTICS_GetServiceCapabilities_Res_S;

/*******************************************************************************/
typedef ONVIF_RET (*GONVIF_Analytics_GetServiceCapabilities)(GONVIF_ANALYTICS_GetServiceCapabilities_Res_S *pstAN_GetServiceCapabilitiesRes);
typedef ONVIF_RET (*GONVIF_Analytics_GetSupportedAnalyticsModules)(GONVIF_ANALYTICS_GetSupportedAnalyticsModules_S *pstGetSupportedAnalyticsModulesReq, GONVIF_ANALYTICS_GetSupportedAnalyticsModulesRes_S *pstGetSupportedAnalyticsModulesRes);
typedef ONVIF_RET (*GONVIF_Analytics_GetAnalyticsModules)(GONVIF_ANALYTICS_GetAnalyticsModules_S *pstGetAnalyticsModulesReq, GONVIF_ANALYTICS_GetAnalyticsModulesRes_S *pstpstGetAnalyticsModulesRes);
typedef ONVIF_RET (*GONVIF_Analytics_GetSupportedRulesRes)(GONVIF_ANALYTICS_GetSupportedRules_S *pstGetSupportedRulesReq, GONVIF_ANALYITICS_GetSupportedRulesRes_S *pstGetSupportedRulesRes);
typedef ONVIF_RET (*GONVIF_Analytics_GetRulesRes)(GONVIF_ANALYTICS_GetRules_S *pstGetRulesReq, GONVIF_ANALYTICS_GetRulesRes_s *pstGetRulesRes);

typedef struct
{
    GONVIF_Analytics_GetServiceCapabilities pfnAN_GetServiceCapabilities;
    GONVIF_Analytics_GetSupportedAnalyticsModules pfnGetSupportedAnalyticsModules;
	GONVIF_Analytics_GetAnalyticsModules pfnGetAnalyticsModules;
	GONVIF_Analytics_GetSupportedRulesRes pfnGetSupportedRules;
	GONVIF_Analytics_GetRulesRes pfnGetRules;
}GOnvif_ANALYTICS_CallbackFunc_S;

#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif  /* _ONVIF_ANALYTICS_H_ */

