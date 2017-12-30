#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "xw_text_prompt_box.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "gk_image_sdk_new.h"
#include "image_zoom_func.h"
#include "xw_logsrv.h"
#include "xw_png_load.h"

#define     PROMPT_WINDOW_H         451
#define     PROMPT_WINDOW_W         211


int xw_text_prompt_box(void*data)
{

    
    struct  user_set_node_atrr _attr;
    int ret = 0;
    //top menu
    memcpy(_attr.node_id,XW_TEXT_PROMPT_BOX_WINDOW_ID,strlen(XW_TEXT_PROMPT_BOX_WINDOW_ID));
    _attr.en_freshen = NEED_CLEAR;
    _attr.en_node    = 0; 
    window_node_menu_t         _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    _mt.x = XW_TEXT_PROMPT_BOX_WINDOW_X; 
    _mt.y = XW_TEXT_PROMPT_BOX_WINDOW_Y;
    
   // _mt.h = PROMPT_WINDOW_H;
   // _mt.w = PROMPT_WINDOW_W;
    
    xw_get_png_hw("testprivew-1ID",&_mt.w,&_mt.h);
    _mt.w = 210;
    _mt.image_cache = (char *)xw_get_window_png("testprivew-1ID");

    xw_logsrv_debug("w:%d h:%d _mt.image_cache:%p\n",_mt.w,_mt.h,_mt.image_cache);


#if 0
    uint16_t    *outbuf  = (uint16_t *)malloc(_mt.h * _mt.w*sizeof(uint16_t));

    image_zoom_t    zt;

    xw_get_png_hw(XW_MAIN_WINDOW_ID,&zt.inwidth,&zt.inheight);
    zt.inbuf        = xw_get_window_png(XW_MAIN_WINDOW_ID);
    zt.outwidth     = _mt.w;
    zt.outheight    = _mt.h;
    zt.outbuf       = outbuf;
    iamge_zoom_func(&zt);


    
    _mt.image_cache = (char *)outbuf;

#endif

    //printf("top image mem:%p\n",_mt.image_cache);
    _mt.video_set.mouse_offset =  NULL;//mouse_offset_top_menu;
    _mt.video_set.mouse_leave  =  NULL;//mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt); 
    
    //Image_SDK_Set_Node_Order(XW_TOP_MENU_WINDOW_ID,FIXD_ORDER);


    return 0;



}


static int show_state = 0; 

int xw_text_promt_put(char *s,int msec){
    
    if(show_state == 0){
        Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,1);
        Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_FRESHEN);
        show_state = 1;
    }else{
        Image_SDK_Set_Node_En(XW_TEXT_PROMPT_BOX_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_TEXT_PROMPT_BOX_WINDOW_ID,NEED_CLEAR);
        show_state = 0;
    }

    return 0;
}


