
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "xw_update.h"
#include "image_sdk_core.h"
#include "xw_window_id_df.h"
#include "xw_window_def_func.h"
#include "xw_png_load.h"
#include "xw_logsrv.h"
#include "xw_msg_prv.h"
#include "xw_config.h"
#include "xw_text_prompt_box.h"


typedef enum{
   XW_UPDATE_NOR,
   XW_UPDATE_WIAT_YN,
   XW_UPDATE_RUN,
   XW_UPDATE_QUIT,
}xw_update_state_s;

static xw_update_state_s xw_update_state;
static int          xw_update_stared = 0 ;
static pthread_t    xw_update_th_id;



static void mouse_offset_button_yes(void *data);
static void mouse_offset_button_no(void *data);
static void mouse_leave_button_yes_no(void *data);

static void mouse_ldown_button_yes(void *data);
static void mouse_ldown_button_no(void *data);
static void update_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h);

static void* xw_update_thread_func(void *data);


int xw_update_show(void *data)
{
    
    
    struct  user_set_node_atrr _attr;
    
    
    int ret = 0;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    //update menu
    memcpy(_attr.node_id,XW_UPDATE_WINDOW_ID,strlen(XW_UPDATE_WINDOW_ID));
    _attr.en_freshen = 0;
    _attr.en_node = 0;
    window_node_menu_t _mt;
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    
    ret = xw_get_node_window_param(XW_UPDATE_WINDOW_ID,&_mt.x,&_mt.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y fail \n",XW_TOP_MENU_WINDOW_ID);
    }
    xw_get_png_hw(XW_UPDATE_WINDOW_ID,&_mt.w,&_mt.h);
    _mt.image_cache = (char *)xw_get_window_png(XW_UPDATE_WINDOW_ID);
    _mt.video_set.mouse_offset = NULL;
    _mt.video_set.mouse_leave  = NULL;
    ret = Image_SDK_Create_Menu(_attr,_mt); 

    //update yes

    window_node_button_t _bt;
    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    memcpy(_attr.node_id,XW_UPDATE_YES_WINDOW_ID,strlen(XW_UPDATE_YES_WINDOW_ID));
    _attr.en_node       = 0;
    _attr.en_freshen    = 0;
    ret = xw_get_node_window_param(XW_UPDATE_YES_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y fail \n",XW_UPDATE_YES_WINDOW_ID);
    }
    
    _bt.color = CON_BUTTON_LEAVE_COLOR; 
    _bt.size  = 2;
    _bt.video_set.mouse_leave       = mouse_leave_button_yes_no;
    _bt.video_set.mouse_offset      = mouse_offset_button_yes;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_yes;
    _bt.user_video_freshen          = update_push_video_button;

    ret  = Image_SDK_Create_Button(_attr,_bt);
    //update no
    
    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    memcpy(_attr.node_id,XW_UPDATE_NO_WINDOW_ID,strlen(XW_UPDATE_NO_WINDOW_ID));
    _attr.en_node       = 0;
    _attr.en_freshen    = 0;
    ret = xw_get_node_window_param(XW_UPDATE_NO_WINDOW_ID,&_bt.x,&_bt.y,&_bt.w,&_bt.h);
    if(ret < 0){
        xw_logsrv_err("windows:%s get x,y fail \n",XW_UPDATE_NO_WINDOW_ID);
    }
    
    _bt.color = CON_BUTTON_LEAVE_COLOR; 
    _bt.size  = 2;
    _bt.video_set.mouse_leave       = mouse_leave_button_yes_no;
    _bt.video_set.mouse_offset      = mouse_offset_button_no;
    _bt.video_set.mouse_left_down   = mouse_ldown_button_no;
    _bt.user_video_freshen          = update_push_video_button;


    ret  = Image_SDK_Create_Button(_attr,_bt);
   
    //update bar 
    window_node_bar_t           _bar;
    window_node_text_t          _text;
    memset(&_bar,0x0,sizeof(window_node_bar_t));
    memset(&_text,0x0,sizeof(window_node_text_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    ret = xw_get_node_window_param(XW_UPDATE_BAR_WINDOW_ID,&_bar.x,&_bar.y,&_bar.w,&_bar.h);
    if(ret){

        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_UPDATE_BAR_WINDOW_ID);
    }
    _bar.bar_color = CON_BAR_LINE_COLOR; 
    _bar.now_value =  0;
    _bar.max_value =  100;
    _bar.video_set.mouse_left_down = NULL; 
    memcpy(_bar.text_id,XW_UPDATE_BAR_TEXT_WINDOW_ID,strlen(XW_UPDATE_BAR_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_UPDATE_BAR_WINDOW_ID,strlen(XW_UPDATE_BAR_WINDOW_ID ));
    ret = Image_SDK_Create_Bar(_attr,_bar);

    //update bar text window
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    ret = xw_get_node_window_param(XW_UPDATE_BAR_TEXT_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_UPDATE_BAR_TEXT_WINDOW_ID);
    }
    _text.win_color     = CON_FONT_WIN_COLOR ;
    _text.text_color    = CON_FONT_TEXT_COLOR ;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_UPDATE_BAR_TEXT_WINDOW_ID,strlen(XW_UPDATE_BAR_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    return 0;
}

static void mouse_offset_button_yes(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = CON_BUTTON_OFFSET_COLOR ;
    Image_SDK_Set_Node_En_Freshen(bt->this_node->node_id,NEED_FRESHEN);
    return ;
}

static void mouse_offset_button_no(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = CON_BUTTON_OFFSET_COLOR;
    Image_SDK_Set_Node_En_Freshen(bt->this_node->node_id,NEED_FRESHEN);
    return ;

}

static void mouse_leave_button_yes_no(void *data){

    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = CON_BUTTON_LEAVE_COLOR ;
    Image_SDK_Set_Node_En_Freshen(bt->this_node->node_id,NEED_FRESHEN);
    return;

}


static void mouse_ldown_button_yes(void *data)
{
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color =CON_BUTTON_LDOWN_COLOR ; 
    
    //clear and close now window and buttion
    Image_SDK_Set_Node_En(bt->this_node->node_id,0);
    Image_SDK_Set_Node_En(XW_UPDATE_NO_WINDOW_ID,0);
    Image_SDK_Set_Node_En(XW_UPDATE_WINDOW_ID,0);
    Image_SDK_Set_Node_En_Freshen(XW_UPDATE_WINDOW_ID,NEED_CLEAR);
    
    //open bar 
    
    Image_SDK_Set_Node_En(XW_UPDATE_BAR_WINDOW_ID,1);
    Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_FRESHEN);
    Image_SDK_Set_Node_En(XW_UPDATE_BAR_TEXT_WINDOW_ID,1);
    //send msg to update srv
    int enbale = 1;
    Image_Msg_Send(IDSCAM_EVENT_MSG_UPDATE_SET_ENBALE,(void *)&enbale,sizeof(int));
    //set ui main func state is updated,into update state ,the event stop tirgger
    Image_SDK_Set_State_Update();
    xw_update_state = XW_UPDATE_RUN;
#if 0
    //create a fresh thread
    if(xw_update_stared  == 0)
        pthread_create(&xw_update_th_id,NULL,xw_update_thread_func,NULL);
    
    xw_update_stared = 1;
#endif

    return ;

}

static void mouse_ldown_button_no(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    Image_SDK_Set_Node_En(bt->this_node->node_id,0);
    Image_SDK_Set_Node_En(XW_UPDATE_NO_WINDOW_ID,0);
    Image_SDK_Set_Node_En(XW_UPDATE_WINDOW_ID,0);
    Image_SDK_Set_Node_En_Freshen(XW_UPDATE_WINDOW_ID,NEED_CLEAR);
    int enbale = 0;
    Image_Msg_Send(IDSCAM_EVENT_MSG_UPDATE_SET_ENBALE,(void *)&enbale,sizeof(int));
    xw_update_state = XW_UPDATE_QUIT;


    return;

}




int xw_update_quit(void *data)
{
    
    
    if(xw_update_state == XW_UPDATE_NOR)
        return 0;

    if(xw_update_state == XW_UPDATE_WIAT_YN)
    {
        Image_SDK_Set_Node_En(XW_UPDATE_WINDOW_ID,0);
        Image_SDK_Set_Node_En(XW_UPDATE_YES_WINDOW_ID,0);
        Image_SDK_Set_Node_En(XW_UPDATE_NO_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_UPDATE_WINDOW_ID,NEED_CLEAR);

    }else if(xw_update_state == XW_UPDATE_RUN){
        
        Image_SDK_Set_Node_En(XW_UPDATE_BAR_TEXT_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_TEXT_WINDOW_ID,NEED_CLEAR);
        Image_SDK_Set_Node_En(XW_UPDATE_BAR_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_CLEAR);
        Image_SDK_Set_State_Run();

    }else{
        ; 
    } 
    return 0;
    //xw_text_promt_put("Forthcoming exit update!",2000);
    //xw_update_state = XW_UPDATE_QUIT; 
    //change main handle state
    //return 0;
}

int xw_update_enble(void *data)
{
    

    Image_SDK_Set_Node_En(XW_UPDATE_WINDOW_ID,1);
    Image_SDK_Set_Node_Submenu(XW_UPDATE_WINDOW_ID,1);
    Image_SDK_Set_Node_En(XW_UPDATE_YES_WINDOW_ID,1);
    Image_SDK_Set_Node_En(XW_UPDATE_NO_WINDOW_ID,1);
    Image_SDK_Set_Node_En_Freshen(XW_UPDATE_WINDOW_ID,NEED_FRESHEN);
    xw_update_state = XW_UPDATE_WIAT_YN;
    xw_logsrv_err("enble update window \n");
    return 0;

}

int xw_update_push_sc(int vaule)
{
    
    if(xw_update_state != XW_UPDATE_RUN)
        return -1;

    Image_SDK_Set_Bar_Vaule(XW_UPDATE_BAR_WINDOW_ID,vaule);
    Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_FRESHEN);

    return 0;
}


//use test
static void* xw_update_thread_func(void *data)
{
   
    int ret  = 0,get_data = 0,errocnt = 0;
    uint16_t vaule = 0;
    while(1)
    {
        
        if(xw_update_state == XW_UPDATE_QUIT)
        {
            
            Image_SDK_Set_Node_En(XW_UPDATE_BAR_TEXT_WINDOW_ID,0);
            Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_TEXT_WINDOW_ID,NEED_CLEAR);
            Image_SDK_Set_Node_En(XW_UPDATE_BAR_WINDOW_ID,0);
            Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_CLEAR);
            //update error ,now reset runing state
            Image_SDK_Set_State_Run();

            break;

        }
        get_data = 0; 
        ret  = Image_Msg_Get(IDSCAM_EVENT_MSG_UPDATE_GET_VUALE,(void *)&get_data,sizeof(int));
        if(ret < 0)
        {
            xw_logsrv_err("get update scale vaule fail\n");
            sleep(1);
            errocnt++;
            if(errocnt > 10)
            {
                xw_update_state = XW_UPDATE_QUIT;
                xw_text_promt_put("update error!",2000);
                sleep(2);
            }
            continue;
        }

        vaule += get_data;
   
        if(vaule >= 100)
        {
            vaule = 100;

        }
        //push freshen bar 
        Image_SDK_Set_Bar_Vaule(XW_UPDATE_BAR_WINDOW_ID,vaule);
        Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_FRESHEN);
        if(vaule >= 100)
        {
            //update 100 ,now restat device
            Image_SDK_Set_Node_En(XW_UPDATE_BAR_WINDOW_ID,0);
            Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_WINDOW_ID,NEED_CLEAR);
            Image_SDK_Set_Node_En(XW_UPDATE_BAR_TEXT_WINDOW_ID,0);
            Image_SDK_Set_Node_En_Freshen(XW_UPDATE_BAR_TEXT_WINDOW_ID,NEED_CLEAR);
            Image_SDK_Set_State_Run();
            xw_text_promt_put("update good ,now reset",2000);
            break;
        }
        errocnt = 0;
        sleep(1);
    }
    xw_logsrv_err("quit update test thread\n");
    return NULL;

}

static void update_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
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

