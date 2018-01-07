#include "xw_logsrv.h"

#define     XW_LOG_INFO_PATH        "/usr/local/bin/logs/message"
#define     XW_LOG_NAME             "xw_log"
#define     XW_LOG_LEVEL            5                //not put debug info 
#define     XW_LOG_CAHCE_SIZE       1024*8
basic_log_t     *xw_logp = NULL;
int     xw_logsrv_init(char *path,uint16_t level){
    
    if(xw_logp != NULL)
        return -1;
    if(path != NULL){
          xw_logp = basic_log_init(XW_LOG_NAME,level, XW_LOG_CAHCE_SIZE,
            path);

    }else{
          xw_logp = basic_log_init(XW_LOG_NAME,level, XW_LOG_CAHCE_SIZE,XW_LOG_INFO_PATH);

    }
    printf("xw_logp:%p\n",xw_logp);
    if(xw_logp != NULL)
        return 0;
    
    return -2;
}
int     xw_logsrv_deinit(void ){
   
    baisc_log_destory(xw_logp);
   xw_logp = NULL;
   return 0;

}

basic_log_t *xw_logsrv_get(void ){
    
    return xw_logp;

}


