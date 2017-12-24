#ifndef __LOG_SRV_H__
#define __LOG_SRV_H__

#include <pthread.h>
#include <stdlib.h>
#include<stdio.h>


//#define       stderr
#define     LOGS_STDERR     0
#define     LOGS_ERR        1
#define     LOGS_WING       2
#define     LOGS_NOTICE     3
#define     LOGS_INFO       4
#define     LOGS_DEBUG      5



#define     LOG_DUB_INFO     basic_log_error_core()


#ifdef  __cplusplus
    extern  "C"{
#endif
    
    

    typedef struct basic_log_s basic_log_t;
    
    basic_log_t*    basic_log_init(char* logs_name,unsigned int level, unsigned int cahce_size,
            char* path);
    void    baisc_log_destory(basic_log_t *log_t);
    void    basic_log_error_core(unsigned int  level, basic_log_t *log ,
        const char *fmt, ...);

    void   basic_log_error_core_v1(unsigned int level,const char *fmt ,...);



#ifdef  __cplusplus
}
#endif



#endif

