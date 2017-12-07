
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_top_menu_show.h"
#include "gk_image_sdk_new.h"


//top menu
#define     XW_TOP_MENU_H_z     48
#define     XW_TOP_MENU_W_z     60
#define     XW_TOP_MENU_H       48
#define     XW_TOP_MENU_W       396


//snap button
#define     XW_SNAP_BUTTON_H                50 
#define     XW_SNAP_BUTTON_W                50
#define     XW_SNAP_BUTTON_OFFSET_COLOR     0xf00f
#define     XW_SNAP_BUTTON_LDOWN_COLOR      0xf0f0
#define     XW_SNAP_BUTTON_LIEN_SIZE        2


static void usr_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h);

static void mouse_offset_button_snap(void *data);
static void mouse_leave_button_snap(void *data);

static void mouse_ldown_button_snap(void *data);
static void mouse_ldown_button_recod(void *data);
static void mouse_ldown_button_set(void *data);
static void mouse_ldown_button_perview(void *data);


int  xw_top_menu_create(void)
{
   
    
    struct  user_set_node_atrr _attr;
    
    int ret = 0;
    //top menu
    memcpy(_attr.node_id,XW_TOP_MENU_WINDOW_ID,strlen(XW_TOP_MENU_WINDOW_ID));
    _attr.en_freshen = 0;
    _attr.en_node = 1;
    window_node_menu_t         _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    _mt.x = XW_TOP_MENU_WINDOW_X; 
    _mt.y = XW_TOP_MENU_WINDOW_Y;
    _mt.h = XW_TOP_MENU_H;
    _mt.w = XW_TOP_MENU_W;
    _mt.image_cache = get_window_png_mem(XW_TOP_MENU_WINDOW_ID);
    _mt.video_set.mouse_offset = NULL;
    _mt.video_set.mouse_leave  = NULL;
    
    ret = Image_SDK_Create_Menu(_attr,_mt); 
    
    //snap
    window_node_button_t    _bt;
    memcpy(_attr.node_id,XW_SNAP_WINDOW_ID,strlen(XW_SNAP_WINDOW_ID));
    memset(&_bt,0x0,sizeof(_bt));
    _bt.x       =   XW_SNAP_WINDOW_X;
    _bt.y       =   XW_SNAP_WINDOW_Y;
    _bt.color   =   0;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen          = usr_push_video_button;
    _bt.video_set.mouse_offset      = mouse_offset_button_snap;
    _bt.video_set.mouse_leave       = mouse_leave_button_snap;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_snap;
    ret = Image_SDK_Create_Button(_attr,_bt); 

    //recod
    memcpy(_attr.node_id,XW_RECOD_WINDOW_ID,strlen(XW_RECOD_WINDOW_ID));
    memset(&_bt,0x0,sizeof(_bt));
    _bt.x       =   XW_RECOD_WINDOW_X;
    _bt.y       =   XW_RECOD_WINDOW_Y;
    _bt.color   =   0;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen = usr_push_video_button;
    //_bt.video_set.mouse_offset      = mouse_offset_button_snap;
    //_bt.video_set.mouse_leave       = mouse_leave_button_snap;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_recod;

    ret = Image_SDK_Create_Button(_attr,_bt); 

    //perivw
    memcpy(_attr.node_id,XW_PERVIEW_WINDOW_ID,strlen(XW_PERVIEW_WINDOW_ID));
    memset(&_bt,0x0,sizeof(_bt));
    _bt.x       =   XW_PERVIEW_WINDOW_X;
    _bt.y       =   XW_PERVIEW_WINDOW_Y;
    _bt.color   =   0;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen = usr_push_video_button;
    _bt.video_set.mouse_left_down = mouse_ldown_button_perview;

    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //set
    memcpy(_attr.node_id,XW_SET_WINDOW_ID,strlen(XW_SET_WINDOW_ID));
    memset(&_bt,0x0,sizeof(_bt));
    _bt.x       =   XW_SET_WINDOW_X;
    _bt.y       =   XW_SET_WINDOW_Y;
    _bt.color   =   0;
    _bt.size    =   XW_SNAP_BUTTON_LIEN_SIZE;
    _bt.user_video_freshen = usr_push_video_button;
    _bt.video_set.mouse_left_down = mouse_ldown_button_set;

    ret = Image_SDK_Create_Button(_attr,_bt); 
    
}

static void usr_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
{

    window_node_button_t *bt =  (window_node_button_t *)data;
    printf(" video push bt->color:%x\n",bt->color); 
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


static void mouse_ldown_button_snap(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    //send singed to main srv
}


static void mouse_ldown_button_snap(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    //send singed to main srv

}
static void mouse_ldown_button_recod(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    //send singed to main srv
}

static void mouse_ldown_button_set(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    //send signed to man srv
}

static void mouse_ldown_button_perview(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_SNAP_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN; 
    //send signed to man srv
}


