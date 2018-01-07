#include <string.h>
#include "xw_main_isp_show.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_png_load.h"
#include "gk_image_sdk_new.h"
#include "xw_logsrv.h"
#define     MAIN_MENU_H         396
#define     MAIN_MENU_W         813
#define     MAIN_MENU_COLOR     0x0



static void mouse_leave_main_menu_func(void *data)
{
    window_node_menu_t *mt  = (window_node_menu_t *)data;
    mt->this_node->freshen_arrt = NEED_CLEAR;
    mt->this_node->en_node = 0;
    mt->this_node->en_submenu = 0;
    return ;
}

static void mouse_offset_main_menu_func(void *data)
{
    window_node_menu_t *mt  = (window_node_menu_t *)data;
      return ;
}


int xw_main_menu_show(void *data)
{
    
    struct user_set_node_atrr _attr;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    memcpy(_attr.node_id,XW_MAIN_WINDOW_ID,strlen(XW_MAIN_WINDOW_ID ));
    _attr.en_freshen = NEED_FRESHEN;

    window_node_menu_t  mt;
    memset(&mt,0x0,sizeof(window_node_menu_t));
    
    mt.x    = XW_MAIN_WINDOW_X;
    mt.y    = XW_MAIN_WINDOW_Y;
    
    xw_get_png_hw(XW_MAIN_WINDOW_ID,&mt.w,&mt.h);
   
    mt.image_cache = (char *)xw_get_window_png(XW_MAIN_WINDOW_ID);
    mt.video_set.mouse_offset =  NULL;//mouse_offset_main_menu_func;

    mt.video_set.mouse_leave  =  mouse_leave_main_menu_func;
    int ret = 0;
    ret = Image_SDK_Create_Menu(_attr,mt);
    return ret ;

}


