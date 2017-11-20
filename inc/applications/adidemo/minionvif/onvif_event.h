/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_event.h
**
** \brief       onvif event
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ONVIF_EVENT_H_
#define _ONVIF_EVENT_H_

#include "onvif_std.h"


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


//*****************************************************************************
//*****************************************************************************
//** Data Structures
//*****************************************************************************
//*****************************************************************************



typedef struct
{
    GK_CHAR aszAddress[MAX_ADDR_LENGTH];
}GONVIF_EVENT_EndpointReferenceType_S;

typedef struct
{
    //GK_S32 sizeTopicExpression;   //This element is needed, client may send numbers of TopicExpression
    GK_CHAR aszTopicExpression[MAX_TOPICEXPRESSION_LEN];  //GK_CHAR **aszTopicExpression
    //GK_S32 sizeMessageContent;    //This element is needed, client may send numbers of MessageContent
    GK_CHAR aszMessageContent[MAX_MESSAGECONTENT_LEN];  //GK_CHAR **aszMessageContent
}GONVIF_EVENT_FilterType_S;

#if 0
typedef struct
{
    GK_S32 size;
    GK_CHAR **any;
}GONVIF_EVENT_Subscribe_SubscriptionPolicy_S;

typedef struct
{
    GK_S32 size;
    GK_CHAR **any;
}GONVIF_EVENT_CreatePullPointSubscription_SubscriptionPolicy_S;
#endif

typedef struct
{
    GK_S32 size;
    GK_CHAR any[MAX_ANY_NUM][MAX_ANY_LEN];
}GONVIF_EVENT_TopicSetType_S;

typedef struct
{
	GONVIF_Boolean_E enWSSubscriptionPolicySupport;
	GONVIF_Boolean_E enWSPullPointSupport;
	GONVIF_Boolean_E enWSPausableSubscriptionManagerInterfaceSupport;
	GK_S32  maxNotificationProducers;
	GK_S32  maxPullPoints;
	GONVIF_Boolean_E enPersistentNotificationStorage;
}GONVIF_EVENT_Capabilities_S;

typedef struct
{
    GK_S8 *dialect;
}GONVIF_EVENT_TopicExpressionType_S;

typedef struct
{
    GK_S8 *any;
}GONVIF_EVENT_NotificationMessageHolderType_Message_S;

typedef struct
{
    GONVIF_EVENT_EndpointReferenceType_S stSubscriptionReference;
    GONVIF_EVENT_TopicExpressionType_S stTopic;
    GONVIF_EVENT_EndpointReferenceType_S stProducerReference;
    GONVIF_EVENT_NotificationMessageHolderType_Message_S stMessage;
}GONVIF_EVENT_NotificationMessageHolderType_S;

/******************************************************************************/
//GetServiceCapabilities
typedef struct
{
	GONVIF_EVENT_Capabilities_S stCapabilities;
}GONVIF_EVENT_GetServiceCapabilities_Res_S;

//GetEventProperties
typedef struct
{
    GK_S32 sizeTopicNamespaceLocation;
    GK_CHAR aszTopicNamespaceLocation[MAX_EVENT_LOCATION_NUM][MAX_EVENT_LOCATION_LEN];
    GONVIF_Boolean_E enFixedTopicSet;
    GONVIF_EVENT_TopicSetType_S stTopicSet;  //Content of this element is different from ONVIF standard.
    GK_S32 sizeTopicExpressionDialect;
    GK_CHAR aszTopicExpressionDialect[MAX_EVENT_DIALECT_NUM][MAX_EVENT_DIALECT_LEN];
    GK_S32 sizeMessageContentFilterDialect;
    GK_CHAR aszMessageContentFilterDialect[MAX_EVENT_DIALECT_NUM][MAX_EVENT_DIALECT_LEN];
    GK_S32 sizeProducerPropertiesFilterDialect;//no needed
    GK_CHAR aszProducerPropertiesFilterDialect[ MAX_EVENT_DIALECT_NUM][MAX_EVENT_DIALECT_LEN];
    GK_S32 sizeMessageContentSchemaLocation;
    GK_CHAR aszMessageContentSchemaLocation[MAX_EVENT_LOCATION_NUM][MAX_EVENT_LOCATION_LEN];
}GONVIF_EVENT_GetEventProperties_Res_S;

//Subscribe
typedef struct
{
    GONVIF_EVENT_EndpointReferenceType_S stConsumerReference;
    GONVIF_EVENT_FilterType_S stFilter; //Content of this element is different from ONVIF standard.
    GK_CHAR aszInitialTerminationTime[MAX_PHHMS_TIME_LEN];
    //GONVIF_EVENT_Subscribe_SubscriptionPolicy_S stSubscriptionPolicy;    
}GONVIF_EVENT_Subscribe_S;

typedef struct
{
    GONVIF_EVENT_EndpointReferenceType_S stSubscriptionReference;
    time_t currentTime;
    time_t terminationTime;
}GONVIF_EVENT_Subscribe_Res_S;

//Unsubscribe
typedef struct
{
    GK_S32 size;
    GK_CHAR **any;
}GONVIF_EVENT_Unsubscribe_S;  

//Renew
typedef struct
{
    GK_CHAR aszTerminationTime[MAX_PHHMS_TIME_LEN];
}GONVIF_EVENT_Renew_S;

typedef struct
{
    time_t currentTime;
    time_t terminationTime;
}GONVIF_EVENT_Renew_Res_S;

//SetSynchronizationPoint
typedef struct
{
    GK_CHAR dummy;
}GONVIF_EVENT_SetSynchronizationPoint_S;

//CreatePullPointSubscription
typedef struct
{
    GONVIF_EVENT_FilterType_S  stFilter;//Content of this element is different from ONVIF standard.
    GK_CHAR aszInitialTerminationTime[MAX_PHHMS_TIME_LEN]; //absolute or relative time
    //GONVIF_EVENT_CreatePullPointSubscription_SubscriptionPolicy_S stSubscriptionPolicy;
}GONVIF_EVENT_CreatePullPointSubscription_S;

typedef struct
{
    GONVIF_EVENT_EndpointReferenceType_S stSubscriptionReference;
    time_t aszCurrentTime;
    time_t aszTerminationTime;
}GONVIF_EVENT_CreatePullPointSubscription_Res_S;

//PullMessages
typedef struct
{
    GK_S32  timeout;                    //ONVIF standard use LONG64 datatype.
    GK_S32  messageLimit;
}GONVIF_EVENT_PullMessages_S;

typedef struct
{
    time_t currentTime;
    time_t terminationTime;
    GK_S32 sizeNotificationMessage;
    GONVIF_EVENT_NotificationMessageHolderType_S pstNotificationMessage;
}GONVIF_EVENT_PullMessages_Res_S;

/******************************************************************************/

typedef ONVIF_RET (*GONVIF_EVENT_GetServiceCapabilities)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_GetServiceCapabilities_Res_S *pstEV_GetServiceCapabilitiesRes);
typedef ONVIF_RET (*GONVIF_EVENT_GetEventProperties)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_GetEventProperties_Res_S *pstEV_GetEventPropertiesRes);
typedef ONVIF_RET (*GONVIF_EVENT_Subscribe)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_Subscribe_S *pstEV_Subscribe, GONVIF_EVENT_Subscribe_Res_S *pstEV_SubscribeRes);
typedef ONVIF_RET (*GONVIF_EVENT_Unsubscribe)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_Unsubscribe_S *pstEV_Unsubscribe);
typedef ONVIF_RET (*GONVIF_EVENT_Renew)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_Renew_S *pstEV_Renew, GONVIF_EVENT_Renew_Res_S *pstEV_RenewRes);
typedef ONVIF_RET (*GONVIF_EVENT_SetSynchronizationPoint)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_SetSynchronizationPoint_S *pstEV_SetSynchronizationPoint);
typedef ONVIF_RET (*GONVIF_EVENT_CreatePullPointSubscription)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_CreatePullPointSubscription_S *pstEV_CreatePullPointSubscription, GONVIF_EVENT_CreatePullPointSubscription_Res_S *pstEV_CreatePullPointSubscriptionRes);
typedef ONVIF_RET (*GONVIF_EVENT_PullMessages)(GONVIF_Soap_S *pstSoap, GONVIF_EVENT_PullMessages_S *pstEV_PullMessages, GONVIF_EVENT_PullMessages_Res_S *pstEV_PullMessagesRes);


typedef struct
{
  GONVIF_EVENT_GetServiceCapabilities      pfnEV_GetServiceCapabilities;  
  GONVIF_EVENT_GetEventProperties          pfnEV_GetEventProperties;
  GONVIF_EVENT_Subscribe                   pfnEV_Subscribe;
  GONVIF_EVENT_Unsubscribe                 pfnEV_Unsubscribe;
  GONVIF_EVENT_Renew                       pfnEV_Renew;
  GONVIF_EVENT_SetSynchronizationPoint     pfnEV_SetSynchronizationPoint;
  GONVIF_EVENT_CreatePullPointSubscription pfnEV_CreatePullPointSubscription;
  GONVIF_EVENT_PullMessages                pfnEV_PullMessages;
}GOnvif_EVENT_CallbackFunction_S;



#ifdef __cplusplus
extern "C" {
#endif




#ifdef __cplusplus
}
#endif



#endif /* _ONVIF_EVENT_H_ */



