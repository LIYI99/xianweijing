#ifndef __XW_LOGSRV_H__
#define __XW_LOGSRV_H__
#include <stdint.h>
#include "image_logsrv.h"
int     xw_logsrv_init(char *path,uint16_t level);
int     xw_logsrv_deinit(void );
basic_log_t *xw_logsrv_get(void );


#define DEBUG   0

#define xw_logsrv_err(format,...)           basic_log_error_core(LOGS_ERR,xw_logsrv_get(),format,##__VA_ARGS__)

#if DEBUG 
    #define xw_logsrv_debug(format,...)        basic_log_error_core(LOGS_DEBUG,xw_logsrv_get(),format,##__VA_ARGS__)
    #define xw_logsrv_info(format,...)         basic_log_error_core(LOGS_INFO,xw_logsrv_get(),format,##__VA_ARGS__)
#else
    #define xw_logsrv_debug(format,...)
    #define xw_logsrv_info(format,...) 
#endif


  



#endif
