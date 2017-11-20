/*!
*****************************************************************************
** \file        onvif.h
**
** \brief       onvif module API
**
** \attention   THIS SAMPLE CODE IS PROVIDED AS IS. GOKE MICROELECTRONICS
**              ACCEPTS NO RESPONSIBILITY OR LIABILITY FOR ANY ERRORS OR
**              OMMISSIONS
**
** (C) Copyright 2012-2013 by GOKE MICROELECTRONICS CO.,LTD
**
*****************************************************************************
*/


#ifndef _ONVIF_H_
#define _ONVIF_H_

#include "onvif_std.h"
#include "onvif_priv.h"

//DeviceMgnt: 		31/79
//DeviceIO:			 5/19
//Event:				 8/4
//Analytics: 			5/10
//AnalyticsDevice: 	0/16
//Display: 			0/9
//Imaging: 			8/7
//Media: 			38/70
//PTZ: 				16/18
//Receiver:			 0/7
//RemoteDiscovery:	 3/3
//Recording: 			0/16
//Replay: 			0/3
//Search: 			0/15

//114/276

#ifdef __cplusplus
extern "C" {
#endif

int onvif_start(int isFromWeb, int webFd);
int onvif_stop(void);

#ifdef __cplusplus
}
#endif



#endif /* _ONVIF_H_ */

