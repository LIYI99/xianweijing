
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_top_menu_show.h"
#include "image_sdk_core.h"
#include "xw_preview_show.h"
#include "xw_line_show.h"
#include "xw_text_prompt_box.h"
#include "xw_logsrv.h"
#include "xw_msg_prv.h"
#include "xw_png_load.h"
#include "xw_window_def_func.h"
#include "xw_config.h"


//top menu 1080P
#define     XW_TOP_MENU_H       48
#define     XW_TOP_MENU_W       396

//1024
#define     SXW_TOP_MENU_H       48
#define     SXW_TOP_MENU_W       396



//snap  recod set pereivew button
#define     XW_SNAP_BUTTON_H                45 
#define     XW_SNAP_BUTTON_W                75


//1024
#define     SXW_SNAP_BUTTON_H                45 
#define     SXW_SNAP_BUTTON_W                75






#define     XW_SNAP_BUTTON_OFFSET_COLOR    CON_BUTTON_OFFSET_COLOR
#define     XW_SNAP_BUTTON_LDOWN_COLOR     CON_BUTTON_LDOWN_COLOR
#define     XW_SNAP_BUTTON_LEAVE_COLOR     CON_BUTTON_LEAVE_COLOR   
#define     XW_SNAP_BUTTON_LIEN_SIZE       CON_BUTTON_LINE_SIZE   

static void usr_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h);

static void mouse_offset_button_snap(void *data);
static void mouse_leave_button_snap(void *data);

static void mouse_ldown_button_snap(void *data);
static void mouse_ldown_button_recod(void *data);
static void mouse_ldown_button_set(void *data);
static void mouse_ldown_button_perview(void *data);

static void mouse_offset_top_menu(void *data);
static void mouse_leave_top_menu(void *data);


int  xw_top_menu_show(void *data)
{
   
    
    struct  user_set_node_atrr _attr;
    
    
    int ret = 0;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    //top menu
    memcpy(_attr.node_id,XW_TOP_MENU_WINDOW_ID,strlen(XW_TOP_MENU_WINDOW_ID));
    _attr.en_freshen = NEED_FRESHEN;
    _attr.en_node = 1; 
    window_node_menu_t         _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    
    ret = xw_get_node_window_param(XW_TOP_MENU_WINDOW_ID,&_mt.x,&_mt.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y fail \n",XW_TOP_MENU_WINDOW_ID);
    }
    
    xw_get_png_hw(XW_TOP_MENU_WINDOW_ID,&_mt.w,&_mt.h);
    _mt.image_cache = (char *)xw_get_window_png(XW_TOP_MENU_WINDOW_ID);
    _mt.video_set.mouse_offset = mouse_offset_top_menu;
    _mt.video_set.mouse_leave  = mouse_leave_top_menu;
    ret = Image_SDK_Create_Menu(_attr,_mt); 
    Image_SDK_Set_Node_Order(XW_TOP_MENU_WINDOW_ID,FIXD_ORDER);

    //snap
    window_node_button_t    _bt;

    memcpy(_attr.node_id,XW_SNAP_WINDOW_ID,strlen(XW_SNAP_WINDOW_ID));
    memset(&_bt,0x0,sizeof(_bt));
    
    ret = xw_get_node_window_param(XW_SNAP_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y,w,h fail\n",XW_SNAP_WINDOW_ID );
    }
  
    _bt.color   =   XW_SNAP_BUTTON_LEAVE_COLOR;

    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen          = usr_push_video_button;
    _bt.video_set.mouse_offset      = mouse_offset_button_snap;
    _bt.video_set.mouse_leave       = mouse_leave_button_snap;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_snap;
    _attr.en_freshen = 0;
    ret = Image_SDK_Create_Button(_attr,_bt); 

    //recod
    memcpy(_attr.node_id,XW_RECOD_WINDOW_ID,strlen(XW_RECOD_WINDOW_ID));
    //memset(&_bt,0x0,sizeof(_bt));
    ret = xw_get_node_window_param(XW_RECOD_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y,w,h fail\n",XW_RECOD_WINDOW_ID );
    }
    _bt.color   =   XW_SNAP_BUTTON_LEAVE_COLOR;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_recod;
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    memcpy(_attr.node_id,XW_PERVIEW_WINDOW_ID,strlen(XW_PERVIEW_WINDOW_ID));
    //memset(&_bt,0x0,sizeof(_bt));
    ret = xw_get_node_window_param(XW_PERVIEW_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y,w,h fail\n",XW_PERVIEW_WINDOW_ID );
    }
    
    _bt.color   =   XW_SNAP_BUTTON_LEAVE_COLOR;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen = usr_push_video_button;
    _bt.video_set.mouse_left_down = mouse_ldown_button_perview;

    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //set
    memcpy(_attr.node_id,XW_SET_WINDOW_ID,strlen(XW_SET_WINDOW_ID));
    //memset(&_bt,0x0,sizeof(_bt));
     ret = xw_get_node_window_param(XW_SET_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y,w,h fail\n",XW_SET_WINDOW_ID );
    }
    _bt.color   =  XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen = usr_push_video_button;
    _bt.video_set.mouse_left_down = mouse_ldown_button_set;
    ret = Image_SDK_Create_Button(_attr,_bt);
    
    return ret;
    
}

static void usr_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
{

    window_node_button_t *bt =  (window_node_button_t *)data;
    //xw_logsrv_debug(" video push bt->color:%x\n",bt->color); 
    //top w line
    int i ,k ;
    for(i = bt->y; i < (bt->y+bt->size) ;i ++){
        for(k = bt->x; k < (bt->w + bt->x) ; k++)
            *(fbbuf+ scree_w*i +k) = bt->color; 
    }
    //low w line
    for(i = bt->y+bt->h - bt->size; i < (bt->y+bt->h) ;i ++){
        for(k = bt->x; k < (bt->w + bt->x) ; k++)
            *(fbbuf+ scree_w*i +k) = bt->color; 
    }
    // l h line
    for(i = bt->y; i < (bt->y+bt->h) ;i ++){
        for(k = bt->x; k < ( bt->x + bt->size) ; k++)
            *(fbbuf+ scree_w*i +k) = bt->color; 
    }
    // r h line
    for(i = bt->y; i < (bt->y+bt->h) ;i ++){
        for(k = bt->x + bt->w - bt->size; k  < ( bt->w + bt->x) ; k++)
            *(fbbuf+ scree_w*i +k) = bt->color; 
    }

}


static void mouse_offset_button_snap(void *data)
{
    
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_OFFSET_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 


   }
static void mouse_leave_button_snap(void *data)

{
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LEAVE_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 

}



static void mouse_ldown_button_snap(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    
     bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    //send singed to main srv
    int ret  = 0;
    ret  = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SDCARD_STATE,NULL,0);
    if(ret <= 0){
        xw_text_promt_put("NOT SDCRAD!",3000);
       // xw_text_promt_put("1234567890 100 99 1",3000);

        return ;
    }
    Image_Msg_Send(IDSCAM_IMG_MSG_CAPTURE,NULL,0);
    xw_snap_name_put(NULL);

    return ;
}

static uint8_t record_state = 0;
static void mouse_ldown_button_recod(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    //send singed to main srv
    int ret  = 0;
    if(record_state == 0){
        ret  = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SDCARD_STATE,NULL,0);
        if(ret <= 0){
            xw_text_promt_put("NOT SDCRAD!",3000);
            return ;
        }
        ret = Image_Msg_Send(IDSCAM_IMG_MSG_RECORED_START,NULL,0);
        xw_time_cnt_start(1);
        record_state = 1;


    }else{
        xw_time_cnt_start(0);
        record_state = 0;
        Image_Msg_Send(IDSCAM_IMG_MSG_RECORED_STOP,NULL,0);
        xw_record_name_put(NULL);

    }
    
    return ;

}

static uint8_t main_window_state = 0;

static void mouse_ldown_button_set(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    if(main_window_state == 0){
        Image_SDK_Set_Node_En(XW_MAIN_WINDOW_ID,1);
        Image_SDK_Set_Node_En_Freshen(XW_MAIN_WINDOW_ID,NEED_FRESHEN);
        Image_SDK_Set_Node_Submenu(XW_MAIN_WINDOW_ID,1);

 
        main_window_state = 1;
    }else{
        //printf("set here 0:\n");
        Image_SDK_Set_Node_En(XW_MAIN_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_MAIN_WINDOW_ID,NEED_CLEAR);
        Image_SDK_Set_Node_Submenu(XW_MAIN_WINDOW_ID,0);

        main_window_state = 0;
    }

}

static void mouse_ldown_button_perview(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    int ret;
    ret  = Image_Msg_Get( IDSCAM_IMG_MSG_GET_SDCARD_STATE ,NULL,0);
    if(ret <= 0){
        xw_text_promt_put("NOT SDCRAD!",3000);
        return ;
    }
    
    xw_lines_close_all_root(NULL);
    xw_preview_cl_op(NULL);
    return ;

}

static void mouse_offset_top_menu(void *data)
{
    window_node_menu_t *mt = (window_node_menu_t *)data;
    mt->this_node->en_submenu = 1;
       
}

static void mouse_leave_top_menu(void *data)
{
    window_node_menu_t *mt = (window_node_menu_t *)data;
    mt->this_node->en_submenu = 0;

}

