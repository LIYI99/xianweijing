/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_device.h
**
** \brief       onvif device
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/

#ifndef _ONVIF_DEVICE_H_
#define _ONVIF_DEVICE_H_

#include "onvif_std.h"


#define ONVIF_WSDL_URL  "http://www.onvif.org/onvif/ver10/device/wsdl/devicemgmt.wsdl " \
                        "http://www.onvif.org/onvif/ver10/event/wsdl/event.wsdl " \
                        "http://www.onvif.org/onvif/ver10/display.wsdl " \
                        "http://www.onvif.org/onvif/ver10/deviceio.wsdl " \
                        "http://www.onvif.org/onvif/ver20/imaging/wsdl/imaging.wsdl " \
                        "http://www.onvif.org/onvif/ver10/media/wsdl/media.wsdl " \
                        "http://www.onvif.org/onvif/ver20/ptz/wsdl/ptz.wsdl " \
                        "http://www.onvif.org/onvif/ver10/receiver.wsdl " \
                        "http://www.onvif.org/onvif/ver10/recording.wsdl " \
                        "http://www.onvif.org/onvif/ver10/search.wsdl " \
                        "http://www.onvif.org/onvif/ver10/network/wsdl/remotediscovery.wsdl "\
                        "http://www.onvif.org/onvif/ver10/replay.wsdl " \
                        "http://www.onvif.org/onvif/ver20/analytics/wsdl/analytics.wsdl " \
                        "http://www.onvif.org/onvif/ver10/analyticsdevice.wsdl " \
                        "http://www.onvif.org/onvif/ver10/actionengine.wsdl " \
                        "http://www.onvif.org/onvif/ver10/schema/onvif.xsd"
#define ONVIF_DEVMANAGE_MAX_CAPABILITY_NUM  7
#define MAX_WSDL_URL_LENGTH                 1024                       
#define MAX_SERVICE_NUM                     7
#define MAX_SERVICE_CAPABLITY_SIZE          1024
#define ONVIF_DEVMANAGE_MAX_NETINTER_NUM    3
#define ONVIF_DEVMANAGE_MAX_MANUAL_NUM      3
#define ONVIF_DEVMANAGE_MAX_LINKLOCAL_NUM   3
#define ONVIF_DEVMANAGE_MAX_FROMDHCP_NUM    3
#define ONVIF_DEVMANAGE_MAX_FROMRA_NUM      3
#define SPEED_ETH                           10
#define ETHERNET                            6
#define MAX_PORT_NUM                        3
#define MAX_NETWORKPROTOCOL_NUM             3
#define MAX_IPV4ADDR_NUM                    3
#define MAX_IPV6ADDR_NUM                    3
#define MAX_TIME_ZONE_LENGTH                64
#define START_TIME_YEAR                     1900
#define MAX_MANUFACTURER_LENGTH             32
#define MAX_MODEL_LENGTH                    32
#define MAX_FIRMWARE_VERSION_LENGTH         32
#define MAX_SERIAL_NUMBER_LENGTH            32
#define MAX_HARDWARE_ID_LENGTH              32
#define MAX_NTPFROMDHCP_NUM                 3
#define MAX_NTPMANUAL_NUM                   3
#define MAX_MESSAGE_LENGTH                  64


typedef enum
{
    UserLevel_Administrator = 0,
    UserLevel_Operator = 1,
    UserLevel_User = 2,
    UserLevel_Anonymous = 3,
    UserLevel_Extended = 4
}GONVIF_DEVMNG_UserLevel_E;

typedef enum 
{
    CapabilityCategory_All         = 0,
    CapabilityCategory_Analytics   = 1,
    CapabilityCategory_Device      = 2,
    CapabilityCategory_Events      = 3,
    CapabilityCategory_Imaging     = 4,
    CapabilityCategory_Media       = 5,
    CapabilityCategory_PTZ         = 6
}GONVIF_DEVMNG_CapabilityCategory_E;

typedef enum 
{
    Duplex_Full = 0,
    Duplex_Half = 1
}GONVIF_DEVMNG_Duplex_E;

typedef enum 
{
    IPv6DHCPConfiguration_Auto = 0,
    IPv6DHCPConfiguration_Stateful = 1,
    IPv6DHCPConfiguration_Stateless = 2,
    IPv6DHCPConfiguration_Off = 3
}GONVIF_DEVMNG_IPv6DHCPConfiguration_E;

typedef enum 
{
    NetworkProtocolType_HTTP = 0,
    NetworkProtocolType_HTTPS = 1,
    NetworkProtocolType_RTSP = 2
}GONVIF_DEVMNG_NetworkProtocolType_E;

typedef enum 
{
    SetDateTimeType_Manual = 0,
    SetDateTimeType_NTP = 1
}GONVIF_DEVMNG_SetDateTimeType_E;

typedef enum 
{
    FactoryDefaultType_Hard = 0,
    FactoryDefaultType_Soft = 1
}GONVIF_DEVMNG_FactoryDefaultType_E;

typedef enum 
{
    NetworkHostType_IPv4 = 0,
    NetworkHostType_IPv6 = 1,
    NetworkHostType_DNS  = 2
}GONVIF_DEVMNG_NetworkHostType_E;

typedef enum
{
    ScopeDefinition_Fixed = 0,
    ScopeDefinition_Configurable = 1
}GONVIF_DEVMNG_ScopeDefinition_E;

typedef enum
{
    DiscoveryMode_Discoverable = 0,
    DiscoveryMode_NonDiscoverable = 1
}GONIVF_DEVMNG_DiscoveryMode_E;

typedef struct 
{
	GK_S32 minor;
	GK_S32 major;
}GONVIF_DEVMNG_OnvifVersion_S;

typedef struct 
{
    GK_CHAR aszUsername[MAX_USERNAME_LENGTH];
    GK_CHAR aszPassword[MAX_PASSWORD_LENGTH];
    GONVIF_DEVMNG_UserLevel_E enUserLevel;
}GONVIF_DEVMNG_User_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GONVIF_Boolean_E enRuleSupport;
    GONVIF_Boolean_E enAnalyticsModuleSupport;
}GONVIF_DEVMNG_AnalyticsCapabilities_S;

typedef struct 
{
    GONVIF_Boolean_E enIPFilter;
    GONVIF_Boolean_E enZeroConfiguration;
    GONVIF_Boolean_E enIPVersion6;
    GONVIF_Boolean_E enDynDNS;
}GONVIF_DEVMNG_tt_NetWorkCapabilities_S;

typedef struct 
{
    GK_S32 inputConnectors;
    GK_S32 relayOutputs;
}GONVIF_DEVMNG_tt_IOCapabilities_S;

typedef struct 
{
    GONVIF_Boolean_E enTLS1_1;
    GONVIF_Boolean_E enTLS1_2;
    GONVIF_Boolean_E enOnboardKeyGeneration;
    GONVIF_Boolean_E enAccessPolicyConfig;
    GONVIF_Boolean_E enX_x002e509Token;
    GONVIF_Boolean_E enSAMLToken;
    GONVIF_Boolean_E enKerberosToken;
    GONVIF_Boolean_E enRELToken;
}GONVIF_DEVMNG_tt_SecurityCapabilities_S;

typedef struct 
{
    GONVIF_Boolean_E enDiscoveryResolve;
    GONVIF_Boolean_E enDiscoveryBye;
    GONVIF_Boolean_E enRemoteDiscovery;
    GONVIF_Boolean_E enSystemBackup;
    GONVIF_Boolean_E enSystemLogging;
    GONVIF_Boolean_E enFirmwareUpgrade;
    GK_S32 sizeSupportedVersions;
    GONVIF_DEVMNG_OnvifVersion_S stSupportedVersions;
}GONVIF_DEVMNG_tt_SystemCapabilities_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GONVIF_DEVMNG_tt_NetWorkCapabilities_S stNetwork;
    GONVIF_DEVMNG_tt_SystemCapabilities_S stSystem;
    GONVIF_DEVMNG_tt_IOCapabilities_S stIO;
    GONVIF_DEVMNG_tt_SecurityCapabilities_S stSecurity;
}GONVIF_DEVMNG_DeviceCapabilities_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GONVIF_Boolean_E enWSSubscriptionPolicySupport;
    GONVIF_Boolean_E enWSPullPointSupport;
    GONVIF_Boolean_E enWSPausableSubscriptionManagerInterfaceSupport;
}GONVIF_DEVMNG_EventCapabilities_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
}GONVIF_DEVMNG_ImagingCapabilities_S;

typedef struct 
{
    GONVIF_Boolean_E enRTPMulticast;
    GONVIF_Boolean_E enRTP_USCORETCP;
    GONVIF_Boolean_E enRTP_USCORERTSP_USCORETCP;
}GONVIF_DEVMNG_RealTimeStreamingCapabilities_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GONVIF_DEVMNG_RealTimeStreamingCapabilities_S stStreamingCapabilities;
}GONVIF_DEVMNG_MediaCapabilities_S;

typedef struct
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
}GONVIF_DEVMNG_PTZCapabilities_S;

typedef struct 
{
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GK_S32 videoSources;
    GK_S32 videoOutputs;
    GK_S32 AudioSources;
    GK_S32 AudioOutputs;
    GK_S32 RelayOutputs;
}GONVIF_DEVMNG_DeviceIOCapabilities_S;

typedef struct
{
    GONVIF_DEVMNG_DeviceIOCapabilities_S stDeviceIO;
}GONVIF_DEVMNG_CapabilitiesExtension_S;

typedef struct
{
    GONVIF_DEVMNG_AnalyticsCapabilities_S stAnalytics;
    GONVIF_DEVMNG_DeviceCapabilities_S stDevice;
    GONVIF_DEVMNG_EventCapabilities_S stEvent;
    GONVIF_DEVMNG_ImagingCapabilities_S stImaging;
    GONVIF_DEVMNG_MediaCapabilities_S stMedia;
    GONVIF_DEVMNG_PTZCapabilities_S stPTZ;
    GONVIF_DEVMNG_CapabilitiesExtension_S stExtension;
}GONVIF_DEVMNG_Capabilities_S;

typedef struct
{
    GK_CHAR any[MAX_SERVICE_CAPABLITY_SIZE];
}GONVIF_DEVMGN_Service_Capabilities_S;

typedef struct 
{
    GK_CHAR aszNamespace[MAX_ADDR_LENGTH];
    GK_CHAR aszXAddr[MAX_ADDR_LENGTH];
    GONVIF_DEVMGN_Service_Capabilities_S stCapabilities;
    GONVIF_DEVMNG_OnvifVersion_S stVersion;
}GONVIF_DEVMGN_Service_S;

typedef struct 
{
	GONVIF_Boolean_E enIPFilter;
	GONVIF_Boolean_E enZeroConfiguration;
	GONVIF_Boolean_E enIPVersion6;
	GONVIF_Boolean_E enDynDNS;
	GONVIF_Boolean_E enDot11Configuration;
	GK_S32  dot1XConfigurations;
	GONVIF_Boolean_E enHostnameFromDHCP;
	GK_S32  ntp;
	GONVIF_Boolean_E enDHCPv6;
}GONVIF_DEVMGN_NetworkCapabilities_S;

typedef struct 
{
	GONVIF_Boolean_E enTLS1_x002e0;
	GONVIF_Boolean_E enTLS1_x002e1;
	GONVIF_Boolean_E enTLS1_x002e2;
	GONVIF_Boolean_E enOnboardKeyGeneration;
	GONVIF_Boolean_E enAccessPolicyConfig;
	GONVIF_Boolean_E enDefaultAccessPolicy;
	GONVIF_Boolean_E enDot1X;
	GONVIF_Boolean_E enRemoteUserHandling;
	GONVIF_Boolean_E enX_x002e509Token;
	GONVIF_Boolean_E enSAMLToken;
	GONVIF_Boolean_E enKerberosToken;
	GONVIF_Boolean_E enUsernameToken;
	GONVIF_Boolean_E enHttpDigest;
	GONVIF_Boolean_E enRELToken;
}GONVIF_DEVMGN_SecurityCapabilities_S;

typedef struct 
{
	GONVIF_Boolean_E enDiscoveryResolve;
	GONVIF_Boolean_E enDiscoveryBye;
	GONVIF_Boolean_E enRemoteDiscovery;
	GONVIF_Boolean_E enSystemBackup;
	GONVIF_Boolean_E enSystemLogging;
	GONVIF_Boolean_E enFirmwareUpgrade;
	GONVIF_Boolean_E enHttpFirmwareUpgrade;
	GONVIF_Boolean_E enHttpSystemBackup;
	GONVIF_Boolean_E enHttpSystemLogging;
	GONVIF_Boolean_E enHttpSupportInformation;
}GONVIF_DEVMGN_SystemCapabilities_S;

typedef struct
{
	GONVIF_DEVMGN_NetworkCapabilities_S  stNetwork;
	GONVIF_DEVMGN_SecurityCapabilities_S stSecurity;
	GONVIF_DEVMGN_SystemCapabilities_S   stSystem;
}GONVIF_DEVMGN_DeviceServiceCapabilities_S;

typedef struct 
{
    GONVIF_IPType_E enType;
    GK_CHAR aszIPv4Address[IPV4_STR_LENGTH];
    GK_CHAR aszIPv6Address[IPV6_STR_LENGTH];
}GONVIF_DEVMNG_IPAddress_S;

typedef struct 
{
    GONVIF_Boolean_E enFromDHCP;
    GK_S32  sizeSearchDomain;
    GK_CHAR aszSearchDomain[MAX_SEARCHDOMAIN_NUM][MAX_SEARCHDOMAIN_LEN];
    GK_S32  sizeDNSFromDHCP;
    GONVIF_DEVMNG_IPAddress_S stDNSFromDHCP[MAX_DNSDHCP_NUM];
    GK_S32  sizeDNSManual;
    GONVIF_DEVMNG_IPAddress_S stDNSManual[MAX_DNSMANUAL_NUM];
}GONVIF_DEVMNG_DNSInformation_S;

typedef struct
{
    GK_CHAR aszName[MAX_NAME_LENGTH];
    GK_CHAR aszHwAddress[MAX_ADDR_LENGTH];
    GK_S32  MTU;
}GONVIF_DEVMNG_NetworkInterfaceInfo_S;


typedef struct 
{
    GONVIF_Boolean_E enAutoNegotiation;
    GK_S32  speed;
    GONVIF_DEVMNG_Duplex_E enDuplex;
}GONVIF_DEVMNG_NetworkInterfaceConnectionSetting_S;


typedef struct 
{
    GONVIF_DEVMNG_NetworkInterfaceConnectionSetting_S stAdminSetting;
    GONVIF_DEVMNG_NetworkInterfaceConnectionSetting_S stOperSettings;
    GK_S32 interfaceType;
}GONVIF_DEVMNG_NetworkInterfaceLink_S;

typedef struct 
{
    GK_CHAR aszAddress[MAX_ADDR_LENGTH];
    GK_S32  prefixLength;
}GONVIF_DEVMNG_PrefixedIPv4Address_S;

typedef struct 
{
    GK_CHAR aszAddress[MAX_ADDR_LENGTH];
    GK_S32  prefixLength;
}GONVIF_DEVMNG_PrefixedIPv6Address_S;

typedef struct 
{
    GK_S32 sizeManual;
    GONVIF_DEVMNG_PrefixedIPv4Address_S stManual[ONVIF_DEVMANAGE_MAX_MANUAL_NUM];
    GONVIF_DEVMNG_PrefixedIPv4Address_S stLinkLocal;
    GONVIF_DEVMNG_PrefixedIPv4Address_S stFromDHCP;
    GONVIF_Boolean_E enDHCP;
}GONVIF_DEVMNG_IPv4Configuration_S;



typedef struct 
{
    GONVIF_Boolean_E enAcceptRouterAdvert;
    GONVIF_DEVMNG_IPv6DHCPConfiguration_E enDHCP;
    GK_S32 sizeManual;
    GONVIF_DEVMNG_PrefixedIPv6Address_S stManual[ONVIF_DEVMANAGE_MAX_MANUAL_NUM];
    GK_S32 sizeLinkLocal;
    GONVIF_DEVMNG_PrefixedIPv6Address_S stLinkLocal[ONVIF_DEVMANAGE_MAX_LINKLOCAL_NUM];
    GK_S32 sizeFromDHCP;
    GONVIF_DEVMNG_PrefixedIPv6Address_S stFromDHCP[ONVIF_DEVMANAGE_MAX_FROMDHCP_NUM];
    GK_S32 sizeFromRA;
    GONVIF_DEVMNG_PrefixedIPv6Address_S stFromRA[ONVIF_DEVMANAGE_MAX_FROMRA_NUM];
}GONVIF_DEVMNG_IPv6Configuration_S;

typedef struct 
{
    GONVIF_Boolean_E enEnabled;
    GONVIF_DEVMNG_IPv4Configuration_S  stConfig;
}GONVIF_DEVMNG_IPv4NetworkInterface_S;

typedef struct 
{
    GONVIF_Boolean_E enEnabled;
    GONVIF_DEVMNG_IPv6Configuration_S stConfig;
}GONVIF_DEVMNG_IPv6NetworkInterface_S;


typedef struct 
{
    GK_CHAR asztoken[MAX_TOKEN_LENGTH];
    GONVIF_Boolean_E enEnabled;
    GONVIF_DEVMNG_NetworkInterfaceInfo_S stInfo;
    GONVIF_DEVMNG_NetworkInterfaceLink_S stLink;
    GONVIF_DEVMNG_IPv4NetworkInterface_S stIPv4;
    GONVIF_DEVMNG_IPv6NetworkInterface_S stIPv6;
}GONVIF_DEVMNG_NetworkInterface_S;

typedef struct 
{
    GONVIF_Boolean_E enEnabled;
    GK_S32  sizeManual;
    GONVIF_DEVMNG_PrefixedIPv4Address_S stManual[ONVIF_DEVMANAGE_MAX_MANUAL_NUM];
    GONVIF_Boolean_E enDHCP;
}GONVIF_DEVMNG_IPv4NetworkInterfaceSetConfiguration_S;

typedef struct 
{
    GONVIF_Boolean_E enEnabled;
    GONVIF_Boolean_E enAcceptRouterAdvert;
    GK_S32  sizeManual;
    GONVIF_DEVMNG_PrefixedIPv6Address_S stManual[ONVIF_DEVMANAGE_MAX_MANUAL_NUM];
    GONVIF_DEVMNG_IPv6DHCPConfiguration_E enDHCP;
}GONVIF_DEVMNG_IPv6NetworkInterfaceSetConfiguration_S;

typedef struct 
{
  GONVIF_Boolean_E enEnable;
  GONVIF_DEVMNG_NetworkInterfaceConnectionSetting_S stLink;
  GK_S32 MTU;
  GONVIF_DEVMNG_IPv4NetworkInterfaceSetConfiguration_S  stIPv4;
  GONVIF_DEVMNG_IPv6NetworkInterfaceSetConfiguration_S  stIPv6;
}GONVIF_DEVMNG_NetworkInterfaceSetConfiguration_S;

typedef struct
{
    GONVIF_DEVMNG_NetworkProtocolType_E enName;
    GONVIF_Boolean_E enEnabled;
    GK_S32 sizePort;
    GK_S32 port[MAX_PORT_NUM];
}GONVIF_DEVMNG_NetworkProtocol_S;

typedef struct 
{
    GK_S32  sizeIPv4Address;
    GK_CHAR aszIPv4Address[MAX_IPV4ADDR_NUM][IPV4_STR_LENGTH];
    GK_S32  sizeIPv6Address;
    GK_CHAR aszIPv6Address[MAX_IPV6ADDR_NUM][IPV6_STR_LENGTH];
}GONVIF_DEVMNG_NetworkGateway_S;

typedef struct 
{
    GK_CHAR aszTZ[MAX_TIME_ZONE_LENGTH];
}GONVIF_DEVMNG_TimeZone_S;

typedef struct 
{
    GK_S32 hour;
    GK_S32 minute;
    GK_S32 second;
}GONVIF_DEVMNG_Time_S;

typedef struct 
{
    GK_S32 year;
    GK_S32 month;
    GK_S32 day;
}GONVIF_DEVMNG_Date_S;

typedef struct 
{
    GONVIF_DEVMNG_Time_S stTime;
    GONVIF_DEVMNG_Date_S stDate;
}GONVIF_DEVMNG_DateTime_S;

typedef struct 
{
    GONVIF_DEVMNG_SetDateTimeType_E enDateTimeType;
    GONVIF_Boolean_E enDaylightSavings;
    GONVIF_DEVMNG_TimeZone_S stTimeZone;
    GONVIF_DEVMNG_DateTime_S stUTCDateTime;
    GONVIF_DEVMNG_DateTime_S stLocalDateTime;
}GONVIF_DEVMNG_SystemDateTime_S;

typedef struct
{
    GONVIF_DEVMNG_NetworkHostType_E enType;
    GK_CHAR aszIPv4Address[IPV4_STR_LENGTH];
    GK_CHAR aszIPv6Address[IPV6_STR_LENGTH];
    GK_CHAR aszDNSname[MAX_NAME_LENGTH];
}GONVIF_DEVMNG_NetworkHost_S;

typedef struct
{
    GONVIF_Boolean_E enFromDHCP;
    GK_S32  sizeNTPFromDHCP;
    GONVIF_DEVMNG_NetworkHost_S stNTPFromDHCP[MAX_NTPFROMDHCP_NUM];
    GK_S32  sizeNTPManual;
    GONVIF_DEVMNG_NetworkHost_S stNTPManual[MAX_NTPMANUAL_NUM];
}GONVIF_DEVMNG_NTPInformation_S;

typedef struct
{
    GONVIF_Boolean_E enFromDHCP;
    GK_CHAR aszName[MAX_NAME_LENGTH];
}GONVIF_DEVMNG_HostnameInformation_S;

typedef struct
{
    GONVIF_DEVMNG_ScopeDefinition_E enScopeDef;
    GK_CHAR aszScopeItem[LEN_SCOPE_ITEM];
}GONVIF_DEVMNG_Scopes_S;

/*******************************************************************************/

//GetWsdlUrl
typedef struct 
{
    GK_CHAR aszWsdlUrl[MAX_WSDL_URL_LENGTH];
}GONVIF_DEVMNG_GetWsdlUrl_Res_S;

//GetServices
typedef struct
{
    GONVIF_Boolean_E enIncludeCapability;
}GONVIF_DEVMGN_GetServices_S;

typedef struct 
{
	GK_S32 sizeService;
	GONVIF_DEVMGN_Service_S stService[MAX_SERVICE_NUM];
}GONVIF_DEVMGN_GetServices_Res_S;

//GetCapabilities
typedef struct 
{
    GK_S32 sizeCategory;
    GONVIF_DEVMNG_CapabilityCategory_E enCategory[ONVIF_DEVMANAGE_MAX_CAPABILITY_NUM];
}GONVIF_DEVMNG_GetCapabilities_S;

typedef struct 
{
    GONVIF_DEVMNG_Capabilities_S stCapabilities;
}GONVIF_DEVMNG_GetCapabilities_Res_S;

//GetServiceCapabilities
typedef struct 
{
    GONVIF_DEVMGN_DeviceServiceCapabilities_S stCapabilities;
}GONVIF_DEVMGN_GetServiceCapabilities_Res_S;

//SystemReboot
typedef struct
{
	GK_CHAR aszMessage[MAX_MESSAGE_LENGTH];
}GONVIF_DEVMGN_SystemReboot_Res_S;

//GetHostname
typedef struct 
{
    GONVIF_DEVMNG_HostnameInformation_S stHostnameInformation;
}GONVIF_DEVMNG_GetHostname_Res_S;

//SetHostname
typedef struct
{
    GK_CHAR aszName[MAX_NAME_LENGTH];
}GONVIF_DEVMNG_SetHostname_S;

//GetDNS
typedef struct
{
	GONVIF_DEVMNG_DNSInformation_S stDNSInformation;
}GONVIF_DEVMNG_GetDNS_Res_S;

//SetDNS
typedef struct 
{
    GONVIF_Boolean_E enFromDHCP;
    GK_S32  sizeSearchDomain;
    GK_CHAR aszSearchDomain[MAX_SEARCHDOMAIN_NUM][MAX_SEARCHDOMAIN_LEN];
    GK_S32  sizeDNSManual;
    GONVIF_DEVMNG_IPAddress_S stDNSManual[MAX_DNSMANUAL_NUM];
}GONVIF_DEVMNG_SetDNS_S;

//GetNTP
typedef struct
{
	GONVIF_DEVMNG_NTPInformation_S stNTPInformation;	
}GONVIF_DEVMNG_GetNTP_Res_S;

//SetNTP
typedef struct
{
    GONVIF_Boolean_E enFromDHCP;
    GK_S32  sizeNTPManual;
    GONVIF_DEVMNG_NetworkHost_S stNTPManual[MAX_NTPMANUAL_NUM];
}GONVIF_DEVMNG_SetNTP_S;

//GetNetworkInterfaces
typedef struct 
{
    GK_S32 sizeNetworkInterfaces;
    GONVIF_DEVMNG_NetworkInterface_S stNetworkInterfaces[ONVIF_DEVMANAGE_MAX_NETINTER_NUM];
}GONVIF_DEVMNG_GetNetworkInterfaces_Res_S;

//SetNetworkInterfaces
typedef struct 
{
    GK_CHAR aszInterfaceToken[MAX_TOKEN_LENGTH];
    GONVIF_DEVMNG_NetworkInterfaceSetConfiguration_S stNetworkInterface;
}GONVIF_DEVMNG_SetNetworkInterfaces_S;

typedef struct 
{
    GONVIF_Boolean_E enRebootNeeded;
}GONVIF_DEVMNG_SetNetworkInterfaces_Res_S;

//GetNetworkProtocols
typedef struct 
{
    GK_S32 sizeNetworkProtocols;
    GONVIF_DEVMNG_NetworkProtocol_S stNetworkProtocols[MAX_NETWORKPROTOCOL_NUM];
}GONVIF_DEVMNG_GetNetworkProtocols_Res_S;

//SetNetworkProtocols
typedef struct 
{
    GK_S32 sizeNetworkProtocols;
    GONVIF_DEVMNG_NetworkProtocol_S stNetworkProtocols[MAX_NETWORKPROTOCOL_NUM];
}GONVIF_DEVMNG_SetNetworkProtocols_S;

//GetNetworkDefaultGateway
typedef struct
{
    GONVIF_DEVMNG_NetworkGateway_S stNetworkGateway;
}GONVIF_DEVMNG_GetNetworkDefaultGateway_Res_S;

//SetNetworkDefaultGateway
typedef struct
{
    GK_S32  sizeIPv4Address;
    GK_CHAR aszIPv4Address[MAX_IPV4ADDR_NUM][IPV4_STR_LENGTH];
    GK_S32  sizeIPv6Address;
    GK_CHAR aszIPv6Address[MAX_IPV6ADDR_NUM][IPV6_STR_LENGTH];
}GONVIF_DEVMNG_SetNetworkDefaultGateway_S;

//GetSystemDateAndTime
typedef struct
{
    GONVIF_DEVMNG_SystemDateTime_S stSystemDataAndTime;
}GONVIF_DEVMNG_GetSystemDateAndTime_Res_S;

//SetSystemDateAndTime
typedef struct
{
    GONVIF_DEVMNG_SetDateTimeType_E enDateTimeType;
    GONVIF_Boolean_E enDaylightSavings;
    GONVIF_DEVMNG_TimeZone_S stTimeZone;
    GONVIF_DEVMNG_DateTime_S stUTCDateTime;
}GONVIF_DEVMNG_SetSystemDateAndTime_S;

//GetDeviceInformation
typedef struct 
{
    GK_CHAR aszManufacturer[MAX_MANUFACTURER_LENGTH];
    GK_CHAR aszModel[MAX_MODEL_LENGTH];
    GK_CHAR aszFirmwareVersion[MAX_FIRMWARE_VERSION_LENGTH];
    GK_CHAR aszSerialNumber[MAX_SERIAL_NUMBER_LENGTH];
    GK_CHAR aszHardwareId[MAX_HARDWARE_ID_LENGTH];
}GONVIF_DEVMNG_GetDeviceInformation_Res_S;

//GetUsers
typedef struct
{
    GK_S32 sizeUser;
    GONVIF_DEVMNG_User_S stUser[MAX_USER_NUM];
}GONVIF_DEVMNG_GetUsers_Res_S;

//CreateUsers
typedef struct
{
    GK_S32 sizeUser;
    GONVIF_DEVMNG_User_S stUser[MAX_USER_NUM];
}GONVIF_DEVMNG_CreateUsers_S;

//DeleteUsers
typedef struct 
{
    GK_S32  sizeUsername;
    GK_CHAR aszUsername[MAX_USER_NUM][MAX_USERNAME_LENGTH];
}GONVIF_DEVMNG_DeleteUsers_S;

//SetUser
typedef struct
{
    GK_S32 sizeUser;
    GONVIF_DEVMNG_User_S stUser[MAX_USER_NUM];
}GONVIF_DEVMNG_SetUser_S;

//SetSystemFactoryDefault
typedef struct 
{
    GONVIF_DEVMNG_FactoryDefaultType_E enFactoryDefault;
}GONVIF_DEVMNG_SetSystemFactoryDefault_S;

//GetScopes
typedef struct
{
	GK_S32 sizeScopes;
	GONVIF_DEVMNG_Scopes_S *pstScopes;
}GONVIF_DEVMNG_GetScopes_Res_S;

//SetScopes
typedef struct
{
    GK_S32 sizeScopes;
    GK_CHAR **pszScopes;
}GONVIF_DEVMNG_SetScopes_S;

//AddScopes
typedef struct
{
    GK_S32 sizeScopeItem;
    GK_CHAR **pszScopeItem;
}GONVIF_DEVMNG_AddScopes_S;

//RemoveScopes
typedef struct
{
    GK_S32 sizeScopeItem;
    GK_CHAR **pszScopeItem;
}GONVIF_DEVMNG_RemoveScopes_S;

typedef struct
{
    GK_S32 sizeScopeItem;
    GK_CHAR **pszScopeItem;
}GONVIF_DEVMNG_RemoveScopes_Res_S;

//GetDiscoveryMode
typedef struct
{
    GONIVF_DEVMNG_DiscoveryMode_E enDiscoveryMode; 
}GONIVF_DEVMNG_GetDiscoveryMode_Res_S;

//SetDiscoveryMode
typedef struct
{
    GONIVF_DEVMNG_DiscoveryMode_E enDiscoveryMode; 
}GONIVF_DEVMNG_SetDiscoveryMode_S;

/*******************************************************************************/

typedef ONVIF_RET(*GONVIF_DEVMNG_GetWsdlUrl)(GONVIF_DEVMNG_GetWsdlUrl_Res_S *pstDM_GetWsdlUrlRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetServices)(GONVIF_DEVMGN_GetServices_S *pstDM_GetServices, GONVIF_DEVMGN_GetServices_Res_S *pstDM_GetServicesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetCapabilities)(GONVIF_DEVMNG_GetCapabilities_S *pstDM_GetCapabilities, GONVIF_DEVMNG_GetCapabilities_Res_S *pstDM_GetCapabilitiesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetServiceCapabilities)(GONVIF_DEVMGN_GetServiceCapabilities_Res_S *pstDM_GetServiceCapabilitiesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SystemReboot)(GONVIF_DEVMGN_SystemReboot_Res_S *pstDM_SystemRebootRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetHostname)(GONVIF_DEVMNG_GetHostname_Res_S *pstDM_GetHostnameRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetHostname)(GONVIF_DEVMNG_SetHostname_S *pstDM_SetHostname);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetDNS)(GONVIF_DEVMNG_GetDNS_Res_S *pstDM_GetDNSRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetDNS)(GONVIF_DEVMNG_SetDNS_S *pstDM_SetDNS);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetNTP)(GONVIF_DEVMNG_GetNTP_Res_S *pstDM_GetNTPRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetNTP)(GONVIF_DEVMNG_SetNTP_S *pstDM_SetNTP);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetNetworkInterfaces)(GONVIF_DEVMNG_GetNetworkInterfaces_Res_S *pstDM_GetNetworkInterfacesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetNetworkInterfaces)(GONVIF_DEVMNG_SetNetworkInterfaces_S *pstDM_SetNetworkInterfaces, GONVIF_DEVMNG_SetNetworkInterfaces_Res_S *pstDM_SetNetworkInterfacesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetNetworkProtocols)(GONVIF_DEVMNG_GetNetworkProtocols_Res_S *pstDM_GetNetworkProtocolRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetNetworkProtocols)(GONVIF_DEVMNG_SetNetworkProtocols_S *pstDM_SetNetworkProtocols);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetNetworkDefaultGateway)(GONVIF_DEVMNG_GetNetworkDefaultGateway_Res_S *pstDM_GetNetworkDefaultGatewayRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetNetworkDefaultGateway)(GONVIF_DEVMNG_SetNetworkDefaultGateway_S *pstDM_SetNetworkDefaultGateway);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetSystemDateAndTime)(GONVIF_DEVMNG_GetSystemDateAndTime_Res_S *pstDM_GetSystemDateAndTimeRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetSystemDateAndTime)(GONVIF_DEVMNG_SetSystemDateAndTime_S *pstDM_SetSystemDateAndTime);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetDeviceInformation)(GONVIF_DEVMNG_GetDeviceInformation_Res_S* pstDM_GetDeviceInformationRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetUsers)(GONVIF_DEVMNG_GetUsers_Res_S *pstDM_GetUsersRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_CreateUsers)(GONVIF_DEVMNG_CreateUsers_S *pstDM_CreateUsers);
typedef ONVIF_RET(*GONVIF_DEVMNG_DeleteUsers)(GONVIF_DEVMNG_DeleteUsers_S *pstDM_DeleteUsers);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetUser)(GONVIF_DEVMNG_SetUser_S *pstDM_SetUser);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetSystemFactoryDefault)(GONVIF_DEVMNG_SetSystemFactoryDefault_S *pstDM_SetSystemFactoryDefault);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetScopes)(GONVIF_DEVMNG_GetScopes_Res_S *pstDM_GetScopesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetScopes)(GONVIF_DEVMNG_SetScopes_S *pstDM_SetScopes);
typedef ONVIF_RET(*GONVIF_DEVMNG_AddScopes)(GONVIF_DEVMNG_AddScopes_S *pstDM_AddScopes);
typedef ONVIF_RET(*GONVIF_DEVMNG_RemoveScope)(GONVIF_DEVMNG_RemoveScopes_S *pstDM_RemoveScopes, GONVIF_DEVMNG_RemoveScopes_Res_S *pstDM_RemoveScopesRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_GetDiscoveryMode)(GONIVF_DEVMNG_GetDiscoveryMode_Res_S *pstDM_GetDiscoveryModeRes);
typedef ONVIF_RET(*GONVIF_DEVMNG_SetDiscoveryMode)(GONIVF_DEVMNG_SetDiscoveryMode_S *pstDM_SetDiscoveryMode);


typedef struct 
{
    GONVIF_DEVMNG_GetWsdlUrl pfnDM_GetWsdlUrl;
    GONVIF_DEVMNG_GetServices pfnDM_GetServices;        
    GONVIF_DEVMNG_GetCapabilities pfnDM_GetCapabilities;                    //done
    GONVIF_DEVMNG_GetServiceCapabilities pfnDM_GetServiceCapabilities;
    
    GONVIF_DEVMNG_SystemReboot pfnDM_SystemReboot;
    GONVIF_DEVMNG_GetHostname pfnDM_GetHostname;  
    GONVIF_DEVMNG_SetHostname pfnDM_SetHostname;
    
    GONVIF_DEVMNG_GetDNS pfnDM_GetDNS;
    GONVIF_DEVMNG_SetDNS pfnDM_SetDNS;
    GONVIF_DEVMNG_GetNTP pfnDM_GetNTP;
    GONVIF_DEVMNG_SetNTP pfnDM_SetNTP;
    GONVIF_DEVMNG_GetNetworkInterfaces pfnDM_GetNetworkInterfaces;          //done
    GONVIF_DEVMNG_SetNetworkInterfaces pfnDM_SetNetworkInterfaces; 
    GONVIF_DEVMNG_GetNetworkProtocols  pfnDM_GetNetworkProtocols;
    GONVIF_DEVMNG_SetNetworkProtocols  pfnDM_SetNetworkProtocols;
    GONVIF_DEVMNG_GetNetworkDefaultGateway pfnDM_GetNetworkDefaultGateway;
    GONVIF_DEVMNG_SetNetworkDefaultGateway pfnDM_SetNetworkDefaultGateway;  
    
    GONVIF_DEVMNG_GetSystemDateAndTime pfnDM_GetSystemDateAndTime;
    GONVIF_DEVMNG_SetSystemDateAndTime pfnDM_SetSystemDateAndTime;
    GONVIF_DEVMNG_GetDeviceInformation pfnDM_GetDeviceInformation;          //done
    
    GONVIF_DEVMNG_GetUsers    pfnDM_GetUsers;
    GONVIF_DEVMNG_CreateUsers pfnDM_CreateUsers;
    GONVIF_DEVMNG_DeleteUsers pfnDM_DeleteUsers;
    GONVIF_DEVMNG_SetUser     pfnDM_SetUser;
    
    GONVIF_DEVMNG_SetSystemFactoryDefault  pfnDM_SetSystemFactoryDefault;
    
    GONVIF_DEVMNG_GetScopes pfnDM_GetScopes;                                //done
    GONVIF_DEVMNG_SetScopes pfnDM_SetScopes;
    GONVIF_DEVMNG_AddScopes pfnDM_AddScopes;
    GONVIF_DEVMNG_RemoveScope pfnDM_RemoveScopes;
    GONVIF_DEVMNG_GetDiscoveryMode pfnDM_GetDiscoveryMode;
    GONVIF_DEVMNG_SetDiscoveryMode pfnDM_SetDiscoveryMode;
}GOnvif_DEVMNG_CallbackFunc_S;


#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif // _ONVIF_DEVICE_H_

