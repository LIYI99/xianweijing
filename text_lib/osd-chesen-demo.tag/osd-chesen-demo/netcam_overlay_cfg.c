#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "netcam_overlay_cfg.h"


OVERLAY_CFG_HANLDE*     _overlay_cfg_info = NULL;
pthread_mutex_t         _overlay_cfg_lock;


int overlay_cfg_hanlde_init(void){
    
    if(_overlay_cfg_info != NULL)
        return -1;
    _overlay_cfg_info = (OVERLAY_CFG_HANLDE *)malloc(sizeof(OVERLAY_CFG_HANLDE));
    if(!_overlay_cfg_info)
        return -1;
    memset(_overlay_cfg_info ,0x0,sizeof(OVERLAY_CFG_HANLDE));
    //lock init
    //
    //set info
    _overlay_cfg_info->enable = 1;

    pthread_mutex_init(&_overlay_cfg_lock,NULL);
    //main
    int i = 0;
    for(i = 0 ; i <  MAX_STREAM_NUMS; i++){

        _overlay_cfg_info->_plan_info[i].enable = 1;
        _overlay_cfg_info->_plan_info[i].streamid = i;

        _overlay_cfg_info->_plan_info[i]._time.enable = 1;
        _overlay_cfg_info->_plan_info[i]._time.vin = 0;
        _overlay_cfg_info->_plan_info[i]._time.streamid = i;
        _overlay_cfg_info->_plan_info[i]._time.font_h = 16; //16个中文字符长度
        _overlay_cfg_info->_plan_info[i]._time.font_size = FONT_SIZE_16;
        _overlay_cfg_info->_plan_info[i]._time.x = 0.3;
        _overlay_cfg_info->_plan_info[i]._time.y = 0.3;
        
        strcpy(_overlay_cfg_info->_plan_info[i]._time.title,kAPP_OVERLAY_CLOCK_NAME);

        _overlay_cfg_info->_plan_info[i]._time.color.argb8888 = 0;
        _overlay_cfg_info->_plan_info[i]._time.canvas = NULL;
        
        _overlay_cfg_info->_plan_info[i]._ch.enable = 1;
        _overlay_cfg_info->_plan_info[i]._ch.vin = 0;
        _overlay_cfg_info->_plan_info[i]._ch.streamid = i;
        _overlay_cfg_info->_plan_info[i]._ch.font_h = 16;
        _overlay_cfg_info->_plan_info[i]._ch.font_size = FONT_SIZE_16;
        _overlay_cfg_info->_plan_info[i]._ch.x = 1/3;
        _overlay_cfg_info->_plan_info[i]._ch.y = 1/6;
        _overlay_cfg_info->_plan_info[i]._ch.color.argb8888 = 0;
        _overlay_cfg_info->_plan_info[i]._ch.canvas = NULL;
        strcpy( _overlay_cfg_info->_plan_info[i]._ch.title,kAPP_OVERLAY_ID_NAME);


        _overlay_cfg_info->_plan_info[i]._text.enable = 1;
        _overlay_cfg_info->_plan_info[i]._text.vin = 0;
        _overlay_cfg_info->_plan_info[i]._text.streamid = i;
        _overlay_cfg_info->_plan_info[i]._text.font_h = 16;
        _overlay_cfg_info->_plan_info[i]._text.font_size = FONT_SIZE_32;
        _overlay_cfg_info->_plan_info[i]._text.x = 0.1;
        _overlay_cfg_info->_plan_info[i]._text.y = 0.2;
        _overlay_cfg_info->_plan_info[i]._text.color.argb8888 = 0;
        _overlay_cfg_info->_plan_info[i]._text.canvas = NULL;
        strcpy( _overlay_cfg_info->_plan_info[i]._text.title,kAPP_OVERLAY_TITLE_NAME);
        strcpy( _overlay_cfg_info->_plan_info[i]._text.buf,"china GOOD !");
    }
    //close three plan
     _overlay_cfg_info->_plan_info[2].enable = 0; //长期不需要第三路
    
     return 0; 
}

int overlay_cfg_hanlde_free(void)
{
    pthread_mutex_lock(&_overlay_cfg_lock);
    if(_overlay_cfg_info != NULL){
        free(_overlay_cfg_info);
    }
    _overlay_cfg_info = NULL;
    pthread_mutex_unlock(&_overlay_cfg_lock);
    pthread_mutex_destroy(&_overlay_cfg_lock);

    return 0;
}

int overlay_cfg_hanlde_enable(int enable)
{
    
    if(enable > 0)
        enable = 1;
    else
        enable = 0;


    pthread_mutex_lock(&_overlay_cfg_lock);
    if(_overlay_cfg_info != NULL){
        _overlay_cfg_info->enable = enable;
    }
    pthread_mutex_unlock(&_overlay_cfg_lock);

}

int overlay_cfg_plan_enable(int streamid,int enable){
    
    if(streamid > (MAX_STREAM_NUMS - 1) || streamid < 0)
        return -1;
    
    if(enable > 0)
        enable = 1;
    else
        enable = 0;
    
    pthread_mutex_lock(&_overlay_cfg_lock);
    if(_overlay_cfg_info != NULL){
        _overlay_cfg_info->_plan_info[streamid].enable = enable;
    }
    pthread_mutex_unlock(&_overlay_cfg_lock);
    return 0;

}

int overlay_cfg_hanlde_set_time(OVERLAY_ARE_INFO *_info){
    
    int  i = 0;
    
    if(_info == NULL)
        return -1;

    pthread_mutex_lock(&_overlay_cfg_lock);
    if(_overlay_cfg_info != NULL){
        for(i = 0 ;i < MAX_STREAM_NUMS ; i++){
            _info->streamid = i;
            _overlay_cfg_info->_plan_info[i]._time  =  *_info;
        }
    }
    pthread_mutex_unlock(&_overlay_cfg_lock);
    return 0;

} //need all set

int overlay_cfg_ch_set(OVERLAY_ARE_INFO  *_info){
    
    int  i = 0;
    
    if(_info == NULL)
        return -1;

    pthread_mutex_lock(&_overlay_cfg_lock);
    if(_overlay_cfg_info != NULL){
        for(i = 0 ;i < MAX_STREAM_NUMS ; i++){
            _info->streamid = i;
            _overlay_cfg_info->_plan_info[i]._ch  =  *_info;
        }
    }
    pthread_mutex_unlock(&_overlay_cfg_lock);
    return 0;

}


int overlay_cfg_text_set(int plan,OVERLAY_ARE_INFO  *_info){
    
    
    int  i = 0;
    
    if(_info == NULL || plan < 0 || plan > ( MAX_STREAM_NUMS - 1))
        return -1;

    pthread_mutex_lock(&_overlay_cfg_lock);
    
    if(_overlay_cfg_info != NULL){
       _overlay_cfg_info->_plan_info[plan]._text  =  *_info;
    }
    
    pthread_mutex_unlock(&_overlay_cfg_lock);
    
    return 0;

}

OVERLAY_CFG_HANLDE* overlay_cfg_get_hanlde(void){
    if(_overlay_cfg_info == NULL)
        return NULL;
    pthread_mutex_lock(&_overlay_cfg_lock);
    return _overlay_cfg_info;
}

void overlay_cfg_free_hanlde(void){
    
    pthread_mutex_unlock(&_overlay_cfg_lock);
    return ;
}

