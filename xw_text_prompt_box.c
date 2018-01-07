#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "xw_text_prompt_box.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "gk_image_sdk_new.h"
#include "image_zoom_func.h"
#include "image_text_put.h"
#include "xw_logsrv.h"
#include "xw_png_load.h"


#define     SRC_FONT_W              8 
#define     SRC_FONT_H              16
#define     PROMPT_WINDOW_H         48
#define     PROMPT_WINDOW_W         24      //   12 nums text
#define     PROMPT_WINDOW_NUMS      12
#define     BOX_WINDOW_COLOR        0x0
#define     BOX_TEXT_COLOR          0xf00f



#define     TIME_CNT_WINDOW_H           32
#define     TIME_CNT_WINDOW_W           16      //   12 nums text
#define     TIME_CNT_WINDOW_NUMS        4
#define     TIME_CNT_WINDOW_COLOR       0x0
#define     TIME_CNT_TEXT_COLOR         0xf00f

static uint16_t *errfontsrc = NULL, *errwindow_cache = NULL,*timecntfontsrc = NULL,*timewindow_cache = NULL;



static  struct timeval  prompt_tv,time_cnt_tv;
static  uint16_t        prompt_sec = 0,time_cnt_sec = 0;
static  pthread_t       xw_promt_box_id;
static  uint8_t         xw_promt_quit  = 0;


static void*    prompt_box_manger(void *data);

int xw_text_prompt_box_show(void*data)
{
     
    errfontsrc = (uint16_t *)malloc(sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*PROMPT_WINDOW_NUMS);
    if(!errfontsrc)
        return -1;
    errwindow_cache = (uint16_t *)malloc(sizeof(uint16_t)*PROMPT_WINDOW_H*PROMPT_WINDOW_W*PROMPT_WINDOW_NUMS);
    if(!errwindow_cache){
        if(errfontsrc)
            free(errfontsrc);
        return -1;
    }


    struct  user_set_node_atrr _attr;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));

    int ret = 0;
    //err info 
    memcpy(_attr.node_id,XW_TEXT_PROMPT_BOX_WINDOW_ID,strlen(XW_TEXT_PROMPT_BOX_WINDOW_ID));
    _attr.en_freshen = NEED_CLEAR;
    _attr.en_node    = 0; 
    window_node_menu_t         _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    _mt.x = XW_TEXT_PROMPT_BOX_WINDOW_X; 
    _mt.y = XW_TEXT_PROMPT_BOX_WINDOW_Y;
    
    _mt.h           =  PROMPT_WINDOW_H;
    _mt.w           =  PROMPT_WINDOW_W * PROMPT_WINDOW_NUMS;
    _mt.image_cache = (char *) errwindow_cache;
    _mt.color       = 0xf00f;
    
    _mt.video_set.mouse_offset =  NULL;//mouse_offset_top_menu;
    _mt.video_set.mouse_leave  =  NULL;//mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt);
    if(ret < 0){
        xw_logsrv_debug("create put text windwow menu fail ret:%d \n",ret);
    }
   ret = Image_SDK_Set_Node_Order(XW_TEXT_PROMPT_BOX_WINDOW_ID,FIXD_ORDER);

    //cont sec 
    timecntfontsrc = (uint16_t *)malloc(sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*TIME_CNT_WINDOW_NUMS);
    if(!timecntfontsrc)
        return -1;
    timewindow_cache = (uint16_t *)malloc(sizeof(uint16_t)*TIME_CNT_WINDOW_H*TIME_CNT_WINDOW_W*TIME_CNT_WINDOW_NUMS);
    if(!timewindow_cache){
        if(timecntfontsrc)
            free(timecntfontsrc);
        return -1;
    }
    
    memcpy(_attr.node_id,XW_TEXT_TIME_CNT_BOX_WINDOW_ID,strlen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID));
    _attr.en_freshen = NEED_CLEAR;
    _attr.en_node    = 0; 
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    _mt.x = XW_TEXT_TIME_CNT_BOX_WINDOW_X;
    _mt.y = XW_TEXT_TIME_CNT_BOX_WINDOW_Y;
    
    _mt.h           =  TIME_CNT_WINDOW_H;
    _mt.w           =  TIME_CNT_WINDOW_W * TIME_CNT_WINDOW_NUMS;
    _mt.image_cache =  (char *) timewindow_cache;
    _mt.color       = 0xf00f;
    
    _mt.video_set.mouse_offset =  NULL;//mouse_offset_top_menu;
    _mt.video_set.mouse_leave  =  NULL;//mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt); 
    ret = Image_SDK_Set_Node_Order(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,FIXD_ORDER);

    //create manger thread
    pthread_create(&xw_promt_box_id,NULL,
             prompt_box_manger,NULL);



    return 0;

}

static int  text_to_image(char *s)
{

    if(s == NULL || *s == '\0')
        return -1;

    memset(errfontsrc,0x0,sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*PROMPT_WINDOW_NUMS);
    memset(errwindow_cache,0x0,sizeof(uint16_t)*PROMPT_WINDOW_H*PROMPT_WINDOW_W*PROMPT_WINDOW_NUMS);



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
                        *(errfontsrc + i*(SRC_FONT_W*PROMPT_WINDOW_NUMS) + k*SRC_FONT_W + j) = BOX_WINDOW_COLOR;
                    }else{
                        *(errfontsrc + i*(SRC_FONT_W*PROMPT_WINDOW_NUMS) + k*SRC_FONT_W + j) = BOX_TEXT_COLOR;
                    }
                }   
            
            } 
        }
        getp = NULL;
    }
    
    image_zoom_t    zt;
    zt.inbuf        = errfontsrc;
    zt.inwidth      = SRC_FONT_W * PROMPT_WINDOW_NUMS;
    zt.inheight     = SRC_FONT_H;
    zt.outbuf       = errwindow_cache;
    zt.outwidth     = PROMPT_WINDOW_W * PROMPT_WINDOW_NUMS;
    zt.outheight    = PROMPT_WINDOW_H;
    iamge_zoom_func(&zt);

    return 0;

}



int xw_text_promt_put(char *s,int msec){


    text_to_image(s);
    gettimeofday(&prompt_tv,NULL);
    prompt_sec = msec/1000;
    Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,1);
    Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_FRESHEN);
    return 0;
}

int xw_time_cnt_start(uint8_t en)
{
    
    if(en == 1){
        gettimeofday(&time_cnt_tv,NULL);
        Image_SDK_Set_Node_En(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,1);
       // Image_SDK_Set_Node_En_Freshen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NEED_FRESHEN);
    }else{
        //gettimeofday(&time_cnt_tv,NULL);
        Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_TEXT_TIME_CNT_BOX_WINDOW_ID,NEED_CLEAR);
        time_cnt_tv.tv_sec  = 0 ;
        time_cnt_tv.tv_usec = 0 ;
    }   
    
    return 0;
}


static int  time_text_to_image(char *s)
{

    if(s == NULL || *s == '\0')
        return -1;

    memset(timecntfontsrc,0x0,sizeof(uint16_t)*SRC_FONT_H*SRC_FONT_W*TIME_CNT_WINDOW_NUMS);
    memset(timewindow_cache,0x0,sizeof(uint16_t)*TIME_CNT_WINDOW_H*TIME_CNT_WINDOW_W*TIME_CNT_WINDOW_NUMS);




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
                            *(timecntfontsrc + i*(SRC_FONT_W*TIME_CNT_WINDOW_NUMS) + k*SRC_FONT_W + j) = TIME_CNT_WINDOW_COLOR ; 
                        }else{
                            *(timecntfontsrc + i*(SRC_FONT_W*TIME_CNT_WINDOW_NUMS) + k*SRC_FONT_W + j) = TIME_CNT_TEXT_COLOR;
                        }
                }   

            } 
        }
        
        getp = NULL;
    }

    image_zoom_t    zt;
    zt.inbuf        = timecntfontsrc;
    zt.inwidth      = SRC_FONT_W * TIME_CNT_WINDOW_NUMS;
    zt.inheight     = SRC_FONT_H;
    zt.outbuf       = timewindow_cache;
    zt.outwidth     = TIME_CNT_WINDOW_W * TIME_CNT_WINDOW_NUMS;
    zt.outheight    = TIME_CNT_WINDOW_H;
    iamge_zoom_func(&zt);

    return 0;

}




static void*    prompt_box_manger(void *data)
{
    
    prompt_tv.tv_sec    = 0 ;
    prompt_tv.tv_usec   = 0 ;
    prompt_sec          = 0 ;
    time_cnt_tv.tv_sec  = 0 ;
    time_cnt_tv.tv_usec = 0 ;

    

    struct  timeval tv,tvp,tvs;
    tv.tv_sec = 0 ;
    tvp.tv_sec = 0 ;
    int     timecnt = 0 ;
    char    timebuf[TIME_CNT_WINDOW_NUMS + 1];
    while(1){
       if(xw_promt_quit)
           break;

        gettimeofday(&tv,NULL);
        if(tvp.tv_sec == 0){
            tvp = tv;
        }

        if(time_cnt_tv.tv_sec)
        {   
            
            if(tvs.tv_sec != time_cnt_tv.tv_sec && time_cnt_tv.tv_sec != 0)
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

            tvs = time_cnt_tv;

        }else{
            timecnt = 0;
        }
        
        if(prompt_sec > 0 && tv.tv_sec != tvp.tv_sec)
        {
            prompt_sec --;
            if(prompt_sec == 0){
                Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,0);
                Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_CLEAR);
            }
        }
       
        usleep(200000);
        tvp = tv;

    }
    return NULL;   
}
int xw_text_prompt_box_quit(void*data){
    
    xw_promt_quit = 1;
    pthread_join(xw_promt_box_id,NULL);
    xw_promt_quit = 0;
    free(errfontsrc );
    free(errwindow_cache);
    free(timecntfontsrc);
    free(timewindow_cache);

    return 0;



}

