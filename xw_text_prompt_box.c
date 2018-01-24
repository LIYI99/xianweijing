#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "xw_text_prompt_box.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "image_sdk_core.h"
#include "image_zoom_func.h"
#include "image_text_put.h"
#include "xw_logsrv.h"
#include "xw_png_load.h"
#include "xw_msg_prv.h"
#include "xw_window_def_func.h"




#define     SRC_FONT_W              8 
#define     SRC_FONT_H              16
#define     PROMPT_WINDOW_H         48
#define     PROMPT_WINDOW_W         24      //   12 nums text
#define     PROMPT_WINDOW_NUMS      24
#define     BOX_WINDOW_COLOR        0x0
#define     BOX_TEXT_COLOR          0xf00f



#define     TIME_CNT_WINDOW_H           32
#define     TIME_CNT_WINDOW_W           16      //   12 nums text
#define     TIME_CNT_WINDOW_NUMS        4
#define     TIME_CNT_WINDOW_COLOR       0x0
#define     TIME_CNT_TEXT_COLOR         0xf00f


typedef struct  text_box_handle{
    
    pthread_t       xw_promt_box_id;
    uint16_t        errbox_x;
    uint16_t        errbox_y;
    uint16_t        errfont_w;
    uint16_t        errfont_h;

    uint16_t*       errfontsrc;
    uint16_t*       errwindow_cache;
    struct timeval  prompt_tv;
    uint16_t        prompt_sec;

    uint8_t         need_put_snap_name;
    uint8_t         need_put_recod_name;
    uint8_t         xw_promt_quit;

    uint16_t        timebox_x;
    uint16_t        timebox_y;
    uint16_t        timefont_w;
    uint16_t        timefont_h;
    uint16_t*       timecntfontsrc;
    uint16_t*       timewindow_cache;
    struct timeval  time_cnt_tv;
    uint16_t        time_cnt_sec;

    
}text_box_handle_t;


text_box_handle_t*  _xw_message_box = NULL;

static void*    prompt_box_manger(void *data);

int xw_text_prompt_box_show(void*data)
{
    if(_xw_message_box)
        return -1;
    
    int ret = 0;
    uint16_t  tfw,tfh,efw,efh;
    
    ret  = xw_get_node_window_param(XW_TEXT_PROMPT_BOX_WINDOW_ID,NULL,NULL,&efw,&efh);
    if(ret < 0){
        xw_logsrv_err("window:%s get ,w,h fail \n",XW_TEXT_PROMPT_BOX_WINDOW_ID);
        return ret;
    }
    ret  = xw_get_node_window_param(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NULL,NULL,&tfw,&tfh);
    if(ret < 0){
        xw_logsrv_err("window:%s get w,h fail \n",XW_TEXT_TIME_CNT_BOX_WINDOW_ID);
        return ret;
    }
    
    _xw_message_box =  (text_box_handle_t *)malloc(sizeof(text_box_handle_t) + SRC_FONT_H * SRC_FONT_W * PROMPT_WINDOW_NUMS * 2 +
            efw * efh * PROMPT_WINDOW_NUMS*2 + SRC_FONT_H *SRC_FONT_W * TIME_CNT_WINDOW_NUMS * 2 + tfw *tfh * TIME_CNT_WINDOW_NUMS * 2);

    if(!_xw_message_box)
    {
        xw_logsrv_err("malloc text box handle free \n");
    }
    memset(_xw_message_box,0x0,sizeof(text_box_handle_t) + SRC_FONT_H * SRC_FONT_W * PROMPT_WINDOW_NUMS *2 +
            efw * efh * PROMPT_WINDOW_NUMS * 2 + SRC_FONT_H *SRC_FONT_W * TIME_CNT_WINDOW_NUMS * 2  + tfw *tfh * TIME_CNT_WINDOW_NUMS*2);

    _xw_message_box->errfont_w  =  efw;
    _xw_message_box->errfont_h  =  efh;
    _xw_message_box->timefont_w =  tfw;
    _xw_message_box->timefont_h =  tfh;
    
    _xw_message_box->errfontsrc         = (uint16_t *)(((void *)_xw_message_box) + sizeof(text_box_handle_t));
    _xw_message_box->errwindow_cache    = _xw_message_box->errfontsrc + SRC_FONT_H * SRC_FONT_W * PROMPT_WINDOW_NUMS;
    _xw_message_box->timecntfontsrc     = _xw_message_box->errwindow_cache + efw *efh *PROMPT_WINDOW_NUMS;
    _xw_message_box->timewindow_cache   = _xw_message_box->timecntfontsrc + SRC_FONT_H * SRC_FONT_W * TIME_CNT_WINDOW_NUMS;



    //ceate err message window
    struct  user_set_node_atrr _attr;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    memcpy(_attr.node_id,XW_TEXT_PROMPT_BOX_WINDOW_ID,strlen(XW_TEXT_PROMPT_BOX_WINDOW_ID));
    _attr.en_freshen = NEED_CLEAR;
    _attr.en_node    = 0; 
    window_node_menu_t         _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));

    ret  = xw_get_node_window_param(XW_TEXT_PROMPT_BOX_WINDOW_ID,&_mt.x,&_mt.y,&efw,&efh);
    if(ret < 0){
        xw_logsrv_err("window:%s get ,w,h fail \n",XW_TEXT_PROMPT_BOX_WINDOW_ID);
        return ret;
    }
    _mt.h           =  efh;
    _mt.w           =  efw * PROMPT_WINDOW_NUMS;
    _mt.image_cache = (char *)  _xw_message_box->errwindow_cache;
    _mt.color       = 0xf00f; 
    _mt.video_set.mouse_offset =  NULL;//mouse_offset_top_menu;
    _mt.video_set.mouse_leave  =  NULL;//mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt);
    if(ret < 0){
        xw_logsrv_debug("create put text windwow menu fail ret:%d \n",ret);
    }
    ret = Image_SDK_Set_Node_Order(XW_TEXT_PROMPT_BOX_WINDOW_ID,FIXD_ORDER);
    
    
    //time count window
    memcpy(_attr.node_id,XW_TEXT_TIME_CNT_BOX_WINDOW_ID,strlen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID));
    _attr.en_freshen = NEED_CLEAR;
    _attr.en_node    = 0; 
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    
    ret  = xw_get_node_window_param(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,&_mt.x,&_mt.y,&tfw,&tfh);
    if(ret < 0){
        xw_logsrv_err("window:%s get w,h fail \n",XW_TEXT_TIME_CNT_BOX_WINDOW_ID);
        return ret;
    }
    _mt.h           =  tfh;
    _mt.w           =  tfw  * TIME_CNT_WINDOW_NUMS;
    _mt.image_cache =  (char *)_xw_message_box->timewindow_cache;
    _mt.color       = 0xf00f;
    
    _mt.video_set.mouse_offset =  NULL;//mouse_offset_top_menu;
    _mt.video_set.mouse_leave  =  NULL;//mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt); 
    ret = Image_SDK_Set_Node_Order(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,FIXD_ORDER);
    //create manger thread
    pthread_create(&_xw_message_box->xw_promt_box_id,NULL,
             prompt_box_manger,NULL);



    return 0;

}

static int  text_to_image(char *s)
{

    if(s == NULL || *s == '\0')
        return -1;

    memset(_xw_message_box->errfontsrc,0x0,sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*PROMPT_WINDOW_NUMS);
    memset(_xw_message_box->errwindow_cache,0x0,sizeof(uint16_t)*PROMPT_WINDOW_H*PROMPT_WINDOW_W*PROMPT_WINDOW_NUMS);



    uint8_t *getp = NULL;
    int     k,j,i;
    for(k = 0 ;*s != '\0';s++,k++)
    {
        if(k > PROMPT_WINDOW_NUMS)
            break;
        getp = image_text_lib_put_pixl(s);
       // xw_logsrv_debug("put info : %s ,getp:%p\n",s,getp); 

        if(getp)
        {
            for(i = 0 ; i < SRC_FONT_H ;i++ ){
                for(j = 0 ;j < SRC_FONT_W;j++,getp++ ){
                    if(*getp == 0){ 
                        *(_xw_message_box->errfontsrc + i*(SRC_FONT_W*PROMPT_WINDOW_NUMS) + k*SRC_FONT_W + j) = BOX_WINDOW_COLOR;
                    }else{
                        *(_xw_message_box->errfontsrc + i*(SRC_FONT_W*PROMPT_WINDOW_NUMS) + k*SRC_FONT_W + j) = BOX_TEXT_COLOR;
                    }
                }   
            
            } 
        }
        getp = NULL;
    }
    
    image_zoom_t    zt;
    zt.inbuf        = _xw_message_box->errfontsrc;
    zt.inwidth      = SRC_FONT_W * PROMPT_WINDOW_NUMS;
    zt.inheight     = SRC_FONT_H;
    zt.outbuf       = _xw_message_box->errwindow_cache;
    zt.outwidth     = _xw_message_box->errfont_w * PROMPT_WINDOW_NUMS;
    zt.outheight    = _xw_message_box->errfont_h;
    iamge_zoom_func(&zt);

    return 0;

}



int xw_text_promt_put(char *s,int msec){

    
    text_to_image(s);
    gettimeofday(&_xw_message_box->prompt_tv,NULL);
    _xw_message_box->prompt_sec = msec/1000;
    Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,1);
    Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_FRESHEN);
    return 0;
}

int xw_time_cnt_start(uint8_t en)
{
    
    if(en == 1){
        gettimeofday(&_xw_message_box->time_cnt_tv,NULL);
        Image_SDK_Set_Node_En(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,1);
       // Image_SDK_Set_Node_En_Freshen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NEED_FRESHEN);
    }else{
        //gettimeofday(&time_cnt_tv,NULL);
        Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NEED_CLEAR);
        _xw_message_box->time_cnt_tv.tv_sec  = 0 ;
        _xw_message_box->time_cnt_tv.tv_usec = 0 ;
    }   
    
    return 0;
}


static int  time_text_to_image(char *s)
{

    if(s == NULL || *s == '\0')
        return -1;

    memset(_xw_message_box->timecntfontsrc,0x0,sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*TIME_CNT_WINDOW_NUMS);
    memset(_xw_message_box->timewindow_cache,0x0,sizeof(uint16_t)*TIME_CNT_WINDOW_H*TIME_CNT_WINDOW_W*TIME_CNT_WINDOW_NUMS);




    uint8_t *getp = NULL;
    int     k,j,i;
    for(k = 0 ;*s != '\0';s++,k++)
    {
        if(k > TIME_CNT_WINDOW_NUMS)
            break;

        getp = image_text_lib_put_pixl(s);
        //xw_logsrv_debug("put info : %s ,getp:%p\n",s,getp); 

        if(getp)
        {
            for(i = 0 ; i < SRC_FONT_H ;i++ ){
                for(j = 0 ;j < SRC_FONT_W;j++,getp++ ){
                    if(*getp == 0){ 
                            *(_xw_message_box->timecntfontsrc + i*(SRC_FONT_W*TIME_CNT_WINDOW_NUMS) + k*SRC_FONT_W + j) = TIME_CNT_WINDOW_COLOR ; 
                        }else{
                            *(_xw_message_box->timecntfontsrc + i*(SRC_FONT_W*TIME_CNT_WINDOW_NUMS) + k*SRC_FONT_W + j) = TIME_CNT_TEXT_COLOR;
                        }
                }   

            } 
        }
        
        getp = NULL;
    }

    image_zoom_t    zt;
    zt.inbuf        = _xw_message_box->timecntfontsrc;
    zt.inwidth      = SRC_FONT_W * TIME_CNT_WINDOW_NUMS;
    zt.inheight     = SRC_FONT_H;
    zt.outbuf       = _xw_message_box->timewindow_cache;
    zt.outwidth     = _xw_message_box->timefont_w * TIME_CNT_WINDOW_NUMS;
    zt.outheight    = _xw_message_box->timefont_h;
    iamge_zoom_func(&zt);

    return 0;

}




static void*    prompt_box_manger(void *data)
{
    
    _xw_message_box->prompt_tv.tv_sec    = 0 ;
    _xw_message_box->prompt_tv.tv_usec   = 0 ;
    _xw_message_box->prompt_sec          = 0 ;
    _xw_message_box->time_cnt_tv.tv_sec  = 0 ;
    _xw_message_box->time_cnt_tv.tv_usec = 0 ;

    
    int ret = 0 ;
    struct  timeval tv,tvp,tvs;
    tv.tv_sec = 0 ;
    tvp.tv_sec = 0 ;
    int     timecnt = 0 ;
    char    timebuf[TIME_CNT_WINDOW_NUMS + 1];
    while(1){
       if(_xw_message_box->xw_promt_quit)
           break;

        gettimeofday(&tv,NULL);
        if(tvp.tv_sec == 0){
            tvp = tv;
        }

        if(_xw_message_box->time_cnt_tv.tv_sec)
        {   
            
            if(tvs.tv_sec != _xw_message_box->time_cnt_tv.tv_sec && _xw_message_box->time_cnt_tv.tv_sec != 0)
                timecnt = 0;
            
            if(tv.tv_sec != tvp.tv_sec)
            {
                timecnt++;
                sprintf(timebuf,"%04d",timecnt);
                //xw_logsrv_debug("time cnt:%s\n",timebuf);
                time_text_to_image(timebuf);
                //Image_SDK_Set_Node_En(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,1);
                Image_SDK_Set_Node_En_Freshen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NEED_FRESHEN);
            }

            tvs = _xw_message_box->time_cnt_tv;

        }else{
            timecnt = 0;
        }
        
        if(_xw_message_box->prompt_sec > 0 && tv.tv_sec != tvp.tv_sec)
        {
            _xw_message_box->prompt_sec --;
            if(_xw_message_box->prompt_sec == 0){
                Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,0);
                Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_CLEAR);
            }
        }
#if 1
        if(_xw_message_box->need_put_snap_name)
        {
            
            char xbuf[25];
            ret  = Image_Msg_Get(IDSCAM_EVENT_GET_CAPTURE_FILENAME,xbuf, 25);
            if(ret >= 0){
                 xw_text_promt_put(xbuf,2000);
            }
            _xw_message_box->need_put_snap_name = 0;
        }

        if(_xw_message_box->need_put_recod_name)
        {
            
            char xbuf[25];
           // sleep(1);
            ret  = Image_Msg_Get(IDSCAM_EVENT_GET_RECORED_FILENAME,xbuf, 25);
            if(ret >= 0)
            {
                xw_text_promt_put(xbuf,3000);
            }
            _xw_message_box->need_put_recod_name = 0;
        }


#endif

        usleep(200000);
        tvp = tv;

    }
    return NULL;   
}

int xw_text_prompt_box_quit(void*data){
    
    _xw_message_box->xw_promt_quit = 1;
    pthread_join(_xw_message_box->xw_promt_box_id,NULL);
    _xw_message_box->xw_promt_quit = 0;
    
    if(_xw_message_box)
        free(_xw_message_box);
    _xw_message_box = NULL;

    return 0;



}


int xw_snap_name_put(char *s)
{
    _xw_message_box->need_put_snap_name = 1;
    return 0;
}


int xw_record_name_put(char *s)
{
    _xw_message_box->need_put_recod_name = 1;
      return 0;
}

