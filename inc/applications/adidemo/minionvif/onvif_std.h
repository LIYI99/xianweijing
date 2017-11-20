/*!
*****************************************************************************
** \file        solution/software/subsystem/onvif-no-gsoap/src/onvif_std.h
**
** \brief       onvif type
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/



//*****************************************************************************
//*****************************************************************************
//** Defines and Macros
//*****************************************************************************
//*****************************************************************************
#ifndef  _ONVIF_STD_H_
#define  _ONVIF_STD_H_

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>

typedef unsigned char       GK_U8;
typedef unsigned short      GK_U16;
typedef unsigned int        GK_U32;
typedef unsigned long       GK_ULONG;
typedef unsigned long long  GK_U64;
typedef char                GK_S8;
typedef char                GK_CHAR;
typedef short        		GK_S16;
typedef int          		GK_S32;
typedef long		 		GK_LONG;
typedef long long    		GK_S64;

typedef float               GK_FLOAT;
typedef double              GK_DOUBLE;
typedef int                 GK_Handle;

/* common */
#define MAX_TOKEN_LENGTH        128
#define MAX_NAME_LENGTH         128
#define MAX_TYPE_LENGTH         128
#define MAX_8_LENGTH			8
#define MAX_16_LENGTH			16
#define MAX_32_LENGTH			32
#define MAX_64_LENGTH			64
#define MAX_128_LENGTH			128
#define MAX_256_LENGTH			256
#define MAX_HEADER_ACTION       128

/* device */
#define MAX_USER_NUM            50
#define MAX_USERNAME_LENGTH     64
#define MAX_PASSWORD_LENGTH     64
#define LEN_WEBSERVICE_URL      100
#define LEN_SCOPE_ITEM          100
#define MAX_SCOPE_NUM           10
#define LARGE_WSDL_URL_LENGTH   1024
#define TIME_ZONE            	"China(GMT+8)\n"
#define TIME_ZONE_NUM           8
#define NTP_SERVER              "10.10.10.10"
#define NTP_BUFFER_MAX          100
#define MAX_ADDR_LENGTH         128
#define MAX_TIME_LEN            32
#define MAX_ERROR_LENGTH        128
#define ONVIF_HTTPS_PORT        1111
#define ONVIF_RTSP_PORT         554
#define ONVIF_HTTP_LISTEN_PORT	80
#define MULTICAST_ADDRESS       "239.255.255.250"
#define MULTICAST_PORT		    3702
#define NIC_NAME_LENTH			10
#define ETHERNET_0              "eth0"
#define ETHERNET_1              "eth1"
#define MAX_TIME_LENGTH         100
#define MAX_CMD_LENGTH			100
#define MAX_INFO_LENGTH         512
#define IPV4_STR_LENGTH         16
#define MASK_STR_LENGTH         32
#define GATEWAY_STR_LENGTH      32
#define IPV6_STR_LENGTH         128
#define MAX_SEARCHDOMAIN_NUM    3
#define MAX_SEARCHDOMAIN_LEN    256
#define MAX_DNSDHCP_NUM     3
#define MAX_DNSMANUAL_NUM        3
#define MAX_GATE_COUNT			3

/* media */
#define MAX_URI_LENGTH                  128
#define MAX_PROFILE_NUM					4
#define MAX_VIDEOSOURCE_NUM				4
#define MAX_VIDEOENCODE_NUM				4
#define MAX_VIDEOSOURCEOPTION_NUM		4
#define MAX_VIDEOENCODEOPTION_NUM		4
#define MAX_AUDIOSOURCE_NUM				4
#define MAX_AUDIOENCODE_NUM				4
#define MAX_VIDEOANALYTIC_NUM			2
#define MAX_AUDIOSOURCEOPTION_NUM		4
#define MAX_AUDIOENCODEOPTION_NUM		1
#define MAX_RELAYOUTPUT_NUM             4

/* imaging */


/* event */
#define MAX_SUB_ID				        100
#define MAX_EVENT_ID_LEN                16
#define MAX_EVENT_TOPIC_NUM             20
#define MAX_EVENT_DIALECT_NUM           18
#define MAX_EVENT_DIALECT_LEN           128
#define MAX_EVENT_LOCATION_NUM          18
#define MAX_EVENT_LOCATION_LEN          128
#define MAX_TOPICEXPRESSION_LEN         128
#define MAX_MESSAGECONTENT_LEN          128
#define MAX_PHHMS_TIME_LEN              20
#define MAX_TO_LEN                      50
#define MAX_ANY_NUM                     10
#define MAX_ANY_LEN                     1024
#define EVENT_STATE_INIT                0
#define EVENT_STATE_CHANGED             1
#define EVENT_STATE_DELETE              2
#define EVENT_MODE_NOTIFYT              0
#define EVENT_MODE_PULL                 1

/* analytics */
#define MAX_SIMPLE_NUM			        10
#define MAX_ELEMENT_NUM			        10
#define MAX_MESSAGE_NUM                 10
#define MAX_LOCATION_NUM		        4
#define MAX_DESCRIPTION_NUM             4
#define MAX_CONFIG_NUM 			        4


/* PTZ */
#define MAX_PTZ_NODE_NUM                2
#define MAX_PTZ_CONFIGURATION_NUM       2
#define MAX_PRESET_TOUR_OPERATION       3
#define MAX_NUM_EFLIP_MODE              3
#define MAX_NUM_REVERSE_MODE            3
#define MAX_SPACE_LENGTH                128
#define MAX_PRESET_NUM                  255


/*signal*/

#define SOAP_XMLSAVE			SIGUSR1

#define XML_FILE_SIZE                1024*20

#define ONVIF_DEBUG                     0   /*1 is open , 0 is close */
#define ONVIF_ERROR                     0
#define ONVIF_INFOR                     0

#if ONVIF_DEBUG
#define ONVIF_DBG(format, ...) printf("\033[0;0m[ONVIF DBG] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define ONVIF_DBG(format, ...)
#endif

#if ONVIF_INFOR
#define ONVIF_INFO(format, ...) printf("\033[0;36m[ONVIF INFOR] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define ONVIF_INFO(format, ...)
#endif

#if ONVIF_ERROR
#define ONVIF_ERR(format, ...) printf("\033[0;31m[ONVIF ERR] FILE: "__FILE__", LINE: %d: "format"\033[0;0m\n", __LINE__, ##__VA_ARGS__)
#else
#define ONVIF_ERR(format, ...)
#endif


/*soap error*/
#define ERR_SENDER              "s:Sender"
#define ERR_RECEIVER            "s:Receiver"
#define ERR_INVALIDARGVAL       "ter:InvalidArgVal"
#define ERR_ACTIONNOTSUPPORTED  "ter:ActionNotSupported"
#define ERR_NOTAUTHORIZED       "ter:NotAuthorized"
#define ERR_OPERATIONPROHIBITED "ter:OperationProhibited"
//#define ERR_RUNERROR            "ter:Action is Supported but run error"
#define ERR_SERVERRUNERROR      "ter:ServerRunError"
#define ERR_MISSINGATTR         "ter:MissingAttr"
#define ERR_ACTION              "ter:Action"


//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************

typedef enum
{
	GK_FALSE    = 0,
	GK_TRUE     = 1
}GK_BOOL;

typedef enum
{
    Boolean_FALSE = 0,
    Boolean_TRUE = 1
}GONVIF_Boolean_E;

typedef enum
{
    IPType_IPv4 = 0,
    IPType_IPv6 = 1
}GONVIF_IPType_E;

typedef struct
{
    GK_FLOAT min;
    GK_FLOAT max;
}GONVIF_FloatRange_S;

typedef struct
{
    GK_S32 min;
    GK_S32 max;
}GONVIF_IntRange_S;

typedef enum
{
    ONVIF_ERR_INVALID_SOAP_XML = 1,
    ONVIF_OK = 0,

    ONVIF_ERR_MISSINGATTR = -1,
    ONVIF_ERR_FIXED_SCOPE = -2,
    ONVIF_ERR_DEVMNG_SCOPE_OVERWRITE = -3,
    ONVIF_ERR_SERVICE_NOT_SUPPORT = -4,
    ONVIF_ERR_USERLEVEL_NOTSET = -5,
    ONVIF_ERR_PROFILE_EXIST = -6,
    ONVIF_ERR_MAX_PROFILES = -7,
    ONVIF_ERR_NO_PROFILE = -8,
	ONVIF_ERR_INVALID_STREAM = -9,
    ONVIF_ERR_NO_VIDEOSOURCE = -10,
    ONVIF_ERR_NO_VIDEOENCODER = -11,
    ONVIF_ERR_NO_AUDIO_SUPPORT = -12,
    ONVIF_ERR_NO_AUDIOSOURCE = -13,
    ONVIF_ERR_NO_AUDIOENCODER = -14,
    ONVIF_ERR_NO_PTZCFG = -15,
	ONVIF_ERR_COM_CONNECT = -16,

//event error
	ONVIF_ERR_EVENT_SUBSCRIBE_OVERFLOW = -17,
	ONVIF_ERR_EVENT_SUBSCRIPTION_NOTEXIST = -18,
	ONVIF_ERR_EVENT_INVALID_TOPICEXPRESSION = -19,
	ONVIF_ERR_EVENT_INVALID_SUBSCRIPTION_ADDR = -300,
	ONVIF_ERR_EVENT_SERVER_RUN_ERROR = -301,
//media error
    ONVIF_ERR_MEDIA_INVAILD_ARG = -20,
    ONVIF_ERR_MEDIA_PROFILE_EXIST = -21,
    ONVIF_ERR_MEDIA_MAX_NVT_PROFILES = -22,
    ONVIF_ERR_MEDIA_NO_SUCH_PROFILE_EXIST = -23,
    ONVIF_ERR_MEDIA_NO_SUCH_CONFIG_EXIST = -24,
    ONVIF_ERR_MEDIA_CONFIGURATION_CONFLICT = -25,
    ONVIF_ERR_MEDIA_AUDIO_NOT_SUPPORTED = -26,
    ONVIF_ERR_MEDIA_SERVER_RUN_ERROR = -27,
    ONVIF_ERR_MEDIA_DELETE_FIXED_PROFILE = -28,
    ONVIF_ERR_MEDIA_NO_VIDEO_SOURCE = -29,
    ONVIF_ERR_MEDIA_NO_VIDEO_ENCODER = -30,
    ONVIF_ERR_MEDIA_NO_PTZ_SUPPORTED = -31,
    ONVIF_ERR_MEDIA_INCOMPLETE_CONFIG = -32,
    ONVIF_ERR_MEDIA_CONFIG_CANNOT_MODIFY = -33,
    ONVIF_ERR_MEDIA_INVALID_STREAMSETUP = -34,
    ONVIF_ERR_MEDIA_STREAM_CONFLICT = -35,
    ONVIF_ERR_MEDIA_INVALID_MULTICAST_SETTINGS = -36,
//DevMnge error
    ONVIF_ERR_DEVMNG_SERVER_RUN_ERROR = -50,
    ONVIF_ERR_DEVMNG_NO_SUCH_SERVICE = -51,
    ONVIF_ERR_DEVMNG_INVALID_HOSTNAME = -52,
    ONVIF_ERR_DEVMNG_INVALID_DNS_MANUAL = -53,
    ONVIF_ERR_DEVMNG_INVALID_IPV4_ADDR = -54,
    ONVIF_ERR_DENMNG_UNSUPPORTED_IPV6 = -55,
    ONVIF_ERR_DEVMNG_INVALID_IPV6_ADDR = -56,
    ONVIF_ERR_DEVMNG_INVALID_NTP_MANUAL = -57,
    ONVIF_ERR_DEVMNG_INVALID_NETWORK_TOKEN = -58,
    ONVIF_ERR_DEVMNG_NETWORK_PROTOCOL_NOT_SUPPORT = -59,
    ONVIF_ERR_DEVMNG_INVALID_DATETIMETYPE = -60,
    ONVIF_ERR_DEVMNG_INVALID_DATETIME = -61,
    ONVIF_ERR_DEVMNG_INVALID_TIMEZONE = -62,
    ONVIF_ERR_DEVMNG_USERNAME_CLASH = 63,
    ONVIF_ERR_DEVMNG_USER_MUM_EXCEED = -64,
    ONVIF_ERR_DEVMNG_USERNAME_MISSING = -65,
    ONVIF_ERR_DEVMNG_USERNAME_FIXED = -66,
    ONVIF_ERR_DEVMNG_USERNAME_TOO_SHORT = -67,
    ONVIF_ERR_DEVMNG_PASSWORD_TOO_SHORT = -68,
    ONVIF_ERR_DEVMNG_USERLEVEL_NOT_ALLOWED = -69,
    ONVIF_ERR_DEVMNG_USERNAME_TOO_LONG = -70,
    ONVIF_ERR_DEVMNG_PASSWORD_TOO_LONG = -71,
    ONVIF_ERR_DEVMNG_EMPTY_SCOPE = -72,
    ONVIF_ERR_DEVMNG_TOO_MANY_SCOPES = -73,
    ONVIF_ERR_DEVMNG_REMOVE_FIXED_SCOPE = -74,
    ONVIF_ERR_DEVMNG_NO_SUCH_SCOPE = -75,
//DeviceIO error
	ONVIF_ERR_DEVICEIO_INVALID_RELAY_TOKEN = -80,
//PTZ error
	ONVIF_ERR_PTZ_SENDER_XML_INVALID = -200,
	ONVIF_ERR_PTZ_NOT_SUPPORTED = -220,
	ONVIF_ERR_PTZ_NOT_SUCH_NODE = 221,
	ONVIF_ERR_PTZ_SERVER_RUN_ERROR = -222,
	ONVIF_ERR_PTZ_NO_SUCH_CONFIG = -223,
	ONVIF_ERR_PTZ_CAN_NOT_MODIFY = -224,
	ONVIF_ERR_PTZ_CONFIG_CONFLICT = -225,
	ONVIF_ERR_PTZ_NOT_SUCH_PROFILE_EXIST = -226,
	ONVIF_ERR_PTZ_NOT_REFERRNCE_PTZCONFIG = -227,
	ONVIF_ERR_PTZ_NOT_UNSUPPORTED_SPACE = -228,
	ONVIF_ERR_PTZ_INVALID_TRANSLATION = 229,
	ONVIF_ERR_PTZ_INVALID_SPEED = -230,
	ONVIF_ERR_PTZ_INVALID_POSITION = 231,
	ONVIF_ERR_PTZ_TOO_MANY_PRESETS = -232,
	ONVIF_ERR_PTZ_PRESET_EXIST = -233,
	ONVIF_ERR_PTZ_INVALID_PRESET_NAME = -234,
	ONVIF_ERR_PTZ_IS_MOVING = -235,
	ONVIF_ERR_PTZ_NO_SUCH_PRESET_TOKEN = -236,

//not had done
//Image error
	ONVIF_ERR_IMAGE_INVALID_VIDEO_SOURCE_TOKEN = -108,
	ONVIF_ERR_IMAGE_SENDER_XML_INVALID = 109,
    ONVIF_ERR_IMAGE_SERVER_RUN_ERROR = -110,
    ONVIF_ERR_IMAGE_BLC_MODE_INVALID_ARGVAL = -111,
    ONVIF_ERR_IMAGE_BLC_LEVEL_INVALID_ARGVAL        = -112,
    ONVIF_ERR_IMAGE_BRIGHTNESS_INVALID_ARGVAL       = -113,
    ONVIF_ERR_IMAGE_COLORSATURETION_INVALID_ARGVAL  = -114,
    ONVIF_ERR_IMAGE_CONTRAST_INVALID_ARGVAL         = -115,
    ONVIF_ERR_IMAGE_EXPOSURE_MODE_INVALID_ARGVAL    = -116,
    ONVIF_ERR_IMAGE_EXPOSURE_MIN_TIME_INVALID_ARGVAL = -117,
    ONVIF_ERR_IMAGE_EXPOSURE_MAX_TIME_INVALID_ARGVAL = -118,
    ONVIF_ERR_IMAGE_EXPOSURE_TIME_MIN_GREATER_MAX_INVALID_ARGVAL = -119,
    ONVIF_ERR_IMAGE_EXPOSURE_MIN_GAIN_INVALID_ARGVAL = -120,
    ONVIF_ERR_IMAGE_EXPOSURE_MAX_GAIN_INVALID_ARGVAL = -121,
    ONVIF_ERR_IMAGE_EXPOSURE_GAIN_MIN_GREATER_MAX_INVALID_ARGVAL = -122,
    ONVIF_ERR_IMAGE_EXPOSURE_TIME_INVALID_ARGVAL = -123,
    ONVIF_ERR_IMAGE_EXPOSURE_GAIN_INVALID_ARGVAL = -124,
    ONVIF_ERR_IMAGE_FOCUS_MODE_INVALID_ARGVAL = -125,
    ONVIF_ERR_IMAGE_FOCUS_DEFAULT_SPEED_INVALID_ARGVAL = -126,
    ONVIF_ERR_IMAGE_FOCUS_NEAR_LIMIT_INVALID_ARGVAL = -127,
    ONVIF_ERR_IMAGE_FOCUS_FAR_LIMIT_INVALID_ARGVAL = -128,
    ONVIF_ERR_IMAGE_FOCUS_LIMIT_NEAR_GREATER_FAR_INVALID_ARGVAL = -129,
    ONVIF_ERR_IMAGE_IRCUTFILTER_INVALID_ARGVAL = -130,
    ONVIF_ERR_IMAGE_SHARPNESS_INVALID_ARGVAL = -131,
    ONVIF_ERR_IMAGE_WIDEDYNAMICRANGE_MDOE_INVALID_ARGVAL = -132,
    ONVIF_ERR_IMAGE_WIDEDYNAMICRANGE_LEVEL_INVALID_ARGVAL = -133,
    ONVIF_ERR_IMAGE_WHITEBALANCE_MODE_INVALID_ARGVAL = -134,
    ONVIF_ERR_IMAGE_WHITEBALANCE_CR_GAIN_INVALID_ARGVAL = -135,
    ONVIF_ERR_IMAGE_WHITEBALANCE_CB_GAIN_INVALID_ARGVAL = -136,
    ONVIF_ERR_IAMGE_FORCE_PERSISTENCE_INVALID_ARGVAL = -137,

    ONVIF_ERR_IMAGE_FOCUS_MOVE_INVALID_ARGVAL = -150,
    ONVIF_ERR_IMAGE_FOCUS_MOVE_ABSOLUTE_POSITION_INVALID_ARGVAL = -151,
    ONVIF_ERR_IMAGE_FOCUS_MOVE_ABSOLUTE_SPEED_INVALID_ARGVAL = -152,
    ONVIF_ERR_IMAGE_FOCUS_MOVE_RELATIVE_DISTANCE_INVALID_ARGVAL = -153,
    ONVIF_ERR_IMAGE_FOCUS_MOVE_RELATIVE_SPEED_INVALID_ARGVAL = -154,
    ONVIF_ERR_IMAGE_FOCUS_MOVE_CONTINUOUS_SPEED_INVALID_ARGVAL = -155,

//analytics error
	ONVIF_ERR_ANALYTICS_INVALID_TOKEN = -250,

}ONVIF_RET;

typedef struct
{
    GK_CHAR aszTo[MAX_TO_LEN];
    GK_CHAR *pszAction;
}GONVIF_ENV_Header_S;

typedef struct
{
    GONVIF_ENV_Header_S stHeader;
}GONVIF_Soap_S;


#endif
