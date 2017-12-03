
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_date_show.h"
#include "gk_image_sdk_new.h"

#define FONT_SIZE           16
#define FONT_WIN_COLOR      0xf00f
#define FONT_COLOR          0x3A39


#define DATE_SET_UP_WINDOW_H        20
#define DATE_SET_UP_WINDOW_W        200
#define DATE_SET_DOWN_WINDOW_H      20
#define DATE_SET_DOWN_WINDOW_W      200


typedef enum{
   DATE_WINDOW_RUNING = 0,
   DATE_WINDOW_SET,
}DATE_WINDOW_RUN_STATE;

static  DATA_WINDOW_RUN_STATE   _state = DATE_WINDOW_RUNING;    


static void  date_set_button_up_ldown(void *data);
static void  date_set_button_down_ldown(void *data);
static void  date_set_button_time_mouse_ldown(void *data);





static  void  mouse_rdown_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    bt->color = 0xFE41;
}



void    xw_date_show(void *data)
{
    
    
    struct user_set_node_atrr  node_attr;
    
    node_attr.en_node       = 1;
    node_attr.en_freshen    = 1;
    node_attr.move_arrt     = 0;

    //set English font size
    int  size_w = FONT_SIZE/2 ,size_h = FONT_SIZE, ret = 0;
    
    //year
    memcpy(node_attr.node_id,XW_DATE_YEAR_WINDOW_ID,strlen(XW_DATE_YEAR_WINDOW_ID));
    window_node_text_t  _text;
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_YEAR_WINDOW_X + size_w;
    _text.y             = XW_DATE_YEAR_WINDOW_Y + size_h;
    _text.lens          = 4;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //moon
    memcpy(node_attr.node_id,XW_DATE_MONN_WINDOW_ID,strlen(XW_DATE_MONN_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_MONN_WINDOW_X + size_w;
    _text.y             = XW_DATE_MONN_WINDOW_Y + size_h;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //day
    memcpy(node_attr.node_id,XW_DATE_DAY_WINDOW_ID,strlen(XW_DATE_DAY_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_DAY_WINDOW_X + size_w;
    _text.y             = XW_DATE_DAY_WINDOW_Y + size_h;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //housr
    memcpy(node_attr.node_id,XW_DATE_HOUR_WINDOW_ID ,strlen(XW_DATE_HOUR_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_HOUR_WINDOW_X + size_w;
    _text.y             = XW_DATE_HOUR_WINDOW_Y + size_h;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //minues
    memcpy(node_attr.node_id ,XW_DATE_MIN_WINDOW_ID, strlen(XW_DATE_MIN_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_MIN_WINDOW_X + size_w;
    _text.y             = XW_DATE_MIN_WINDOW_Y + size_h;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    //sec
    memcpy(node_attr.node_id,XW_DATE_SEC_WINDOW_ID,strlen(XW_DATE_SEC_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x             = XW_DATE_SEC_WINDOW_X + size_w;
    _text.y             = XW_DATE_SEC_WINDOW_Y + size_h;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //date time set  button create
    memcpy(node_attr.node_id,XW_DATE_SET_UP_WINDOW_ID  ,strlen(XW_DATE_SET_UP_WINDOW_ID));
    node_attr.en_freshen = NORTHING;  
    window_node_button_t bt;
    memset(&bt,0x0,sizeof(window_node_button_t));
    bt.x                =  XW_DATE_SET_UP_WINDOW_X ;
    bt.y                =  XW_DATE_SET_UP_WINDOW_Y ;
    bt.w                =  DATE_SET_UP_WINDOW_W;
    bt.h                =  DATE_SET_UP_WINDOW_H;
    bt.color            =  0;
    bt.video_set.mouse_left_down = NULL;
    ret = Image_SDK_Create_Button(node_attr,bt);
    
    
    memcpy(node_attr.node_id,XW_DATE_SET_DOWN_WINDOW_ID  ,strlen(XW_DATE_SET_DOWN_WINDOW_ID));
    node_attr.en_freshen = NORTHING;  
    memset(&bt,0x0,sizeof(window_node_button_t));
    bt.x                =  XW_DATE_SET_DOWN_WINDOW_X ;
    bt.y                =  XW_DATE_SET_DOWN_WINDOW_Y ;
    bt.w                =  DATE_SET_DOWN_WINDOW_W;
    bt.h                =  DATE_SET_DOWN_WINDOW_H;
    bt.color            =  0;
    bt.video_set.mouse_left_down = NULL;
    ret = Image_SDK_Create_Button(node_attr,bt);

    

    //get time and push
    time_t  tp = 0;
    struct  tm  *time_now,last_time;
    memset(&last_time,0x0,sizeof(struct tm));
    
    char    *year       = (char *)malloc(32);
    last_time.tm_mon    = 13;
    last_time.tm_hour   = 25;
    last_time.tm_min    = 61;

    while(1)
    {
        // at set timeing ,stop show   
        if(_state == DATE_WINDOW_SET)
        {
            sleep(1);
            continue;
        } 
        time(&tp);
        time_now = localtime(&tp);

        if(last_time.tm_year != time_now->tm_year)
        {
            sprintf(year,"%d",time_now->tm_year+1900);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_YEAR_WINDOW_ID,year,4);
        }
        if(last_time.tm_mon != time_now->tm_mon)
        {
            sprintf(year,"%d",time_now->tm_mon+1);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_MONN_WINDOW_ID,year,2);
        }
        if(last_time.tm_mday != time_now->tm_mday)
        {   
            sprintf(year,"%d",time_now->tm_mday);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_DAY_WINDOW_ID, year,2);
        }
        if(last_time.tm_hour != time_now->tm_hour)
        {
            sprintf(year,"%d",time_now->tm_hour);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_HOUR_WINDOW_ID , year,2);
        }
        if(last_time.tm_min != time_now->tm_min)
        {
            sprintf(year,"%d",time_now->tm_min);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_MIN_WINDOW_ID, year,2);
        }
        if(last_time.tm_sec != time_now->tm_sec)
        {
            sprintf(year,"%d",time_now->tm_sec);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_SEC_WINDOW_ID, year,2);
        }
        last_time = *time_now;
        usleep(800000);
    }
    
    return;

}

static void  date_set_button_up_ldown(void *data)
{


}

static void  date_set_button_down_ldown(void *data);
static void  date_set_button_time_mouse_ldown(void *data);


void    xw_date_set(void *data){

}



