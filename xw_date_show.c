
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_date_show.h"
#include "image_sdk_core.h"
#include "xw_logsrv.h"
#include "xw_config.h"
#include "xw_window_def_func.h"
#include "xw_msg_prv.h"


//
#define FONT_SIZE                  CON_FONT_SIZE   
#define FONT_WIN_COLOR             CON_FONT_WIN_COLOR 
#define FONT_COLOR                 CON_FONT_TEXT_COLOR 
//
#define DATE_SET_UP_WINDOW_H        30
#define DATE_SET_UP_WINDOW_W        260
#define DATE_SET_DOWN_WINDOW_H      30
#define DATE_SET_DOWN_WINDOW_W      260

#define DATE_SET_WINDOW_W           80
#define DATE_SET_WINDOW_H           70






typedef enum{
   DATE_WINDOW_RUNING = 0,
   DATE_WINDOW_SET,
} DATE_WINDOW_RUN_STATE;

static void  mouse_ldown_button_setdown(void *data);
static void  mouse_ldown_button_setup(void *data);
static void  mouse_ldown_button_settime(void *data);



static  DATE_WINDOW_RUN_STATE   _state = DATE_WINDOW_RUNING;    
static  struct  tm  set_time;

static  void  mouse_ldown_button_settime(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
   
    if(_state == DATE_WINDOW_RUNING ){
        bt->color = CON_BUTTON_LDOWN_COLOR;
        bt->this_node->freshen_arrt = NEED_FRESHEN;
        _state = DATE_WINDOW_SET;
        return;
    }

    bt->color = CON_BUTTON_LEAVE_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;


    time_t  tmp = 0 ;

    tmp = mktime(&set_time);
    struct timeval  setval;
    struct timezone zval;

    gettimeofday(&setval,&zval);
    setval.tv_sec = tmp;
    settimeofday(&setval,&zval);
    _state = DATE_WINDOW_RUNING;
    //sen cmd to borad
    int values = 1;
    Image_Msg_Send(IDSCAM_IMG_MSG_SET_TIME,&values,4);

    return ;
}

static void mouse_offset_settime_func(void *data){
    return ;
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color =  CON_BUTTON_OFFSET_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return;
}

static void mouse_leave_settime_func(void *data){
    return;
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = 0xd88;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return;
}


static void mouse_lup_settime_func(void *data){
    return;
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = 0xf0f0;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return;
}




//1080
#define YEAR_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 0
#define YEAR_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 0

#define MON_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 46
#define MON_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 46

#define DAY_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 82
#define DAY_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 82

#define HOUR_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 129
#define HOUR_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 129

#define MIN_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 175
#define MIN_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 175

#define SEC_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 216
#define SEC_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 216

//1024 *600

#define SYEAR_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 0
#define SYEAR_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 0

#define SMON_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 25
#define SMON_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 25

#define SDAY_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 44
#define SDAY_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 44

#define SHOUR_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 69
#define SHOUR_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 69

#define SMIN_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 93
#define SMIN_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 93

#define SSEC_SET_UP_X      XW_DATE_SET_UP_WINDOW_X + 115
#define SSEC_SET_DOWN_X    XW_DATE_SET_DOWN_WINDOW_X + 115








static  void  mouse_ldown_button_setup(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    
    GK_MOUSE_DATA   mdata = bt->this_node->mouse_data;

    if (xw_get_node_window_mode() == SRCEE_MODE_1080)
    {

        if(mdata.x >= YEAR_SET_UP_X &&  mdata.x < MON_SET_UP_X){
            set_time.tm_year++;
        }else if( mdata.x >= MON_SET_UP_X &&  mdata.x < DAY_SET_UP_X){
            set_time.tm_mon = (set_time.tm_mon + 1) % 12;
        }else if(mdata.x >= DAY_SET_UP_X &&  mdata.x < HOUR_SET_UP_X){
            set_time.tm_mday = (set_time.tm_mday + 1) % 31;
        }else if(mdata.x >= HOUR_SET_UP_X &&  mdata.x < MIN_SET_UP_X){
            set_time.tm_hour = (set_time.tm_hour + 1) % 24; 
        }else if(mdata.x >= MIN_SET_UP_X &&  mdata.x < SEC_SET_UP_X){
            set_time.tm_min = (set_time.tm_min + 1) % 60;
        }else if(mdata.x >= SEC_SET_UP_X ){
            set_time.tm_sec = (set_time.tm_sec + 1) % 60;
        }else{
            ;
        }   
    }else if(xw_get_node_window_mode() == SRCEE_MODE_600){
        
        if(mdata.x >= SYEAR_SET_UP_X &&  mdata.x < SMON_SET_UP_X){
            set_time.tm_year++;
        }else if( mdata.x >= SMON_SET_UP_X &&  mdata.x < SDAY_SET_UP_X){
            set_time.tm_mon = (set_time.tm_mon + 1) % 12;
        }else if(mdata.x >= SDAY_SET_UP_X &&  mdata.x < SHOUR_SET_UP_X){
            set_time.tm_mday = (set_time.tm_mday + 1) % 31;
        }else if(mdata.x >= SHOUR_SET_UP_X &&  mdata.x < SMIN_SET_UP_X){
            set_time.tm_hour = (set_time.tm_hour + 1) % 24; 
        }else if(mdata.x >= SMIN_SET_UP_X &&  mdata.x < SSEC_SET_UP_X){
            set_time.tm_min = (set_time.tm_min + 1) % 60;
        }else if(mdata.x >= SSEC_SET_UP_X ){
            set_time.tm_sec = (set_time.tm_sec + 1) % 60;
        }else{
            ;
        }  
    }else{
        xw_logsrv_err("not find the srcee mode \n");
    }

    return;
    
    
}



static  void  mouse_ldown_button_setdown(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    
    GK_MOUSE_DATA   mdata = bt->this_node->mouse_data;
   

    if (xw_get_node_window_mode() == SRCEE_MODE_1080){

        if(mdata.x >= YEAR_SET_DOWN_X &&  mdata.x < MON_SET_DOWN_X){
            set_time.tm_year--;
        }else if( mdata.x >= MON_SET_DOWN_X &&  mdata.x < DAY_SET_DOWN_X){
            if(set_time.tm_mon > 0)
                set_time.tm_mon--;
        }else if(mdata.x >= DAY_SET_DOWN_X &&  mdata.x < HOUR_SET_DOWN_X){
            if(set_time.tm_mday > 0)
                set_time.tm_mday--;
        }else if(mdata.x >= HOUR_SET_DOWN_X &&  mdata.x < MIN_SET_DOWN_X){
            if(set_time.tm_hour > 0)
                set_time.tm_hour--; 
        }else if(mdata.x >= MIN_SET_DOWN_X &&  mdata.x < SEC_SET_DOWN_X){
            if(set_time.tm_min > 0) 
                set_time.tm_min--;
        }else if(mdata.x >= SEC_SET_DOWN_X ){
            if(set_time.tm_sec > 0)
                set_time.tm_sec--;
        }else{
            ;
        }
    }else if(xw_get_node_window_mode() == SRCEE_MODE_600){
        if(mdata.x >= SYEAR_SET_DOWN_X &&  mdata.x < SMON_SET_DOWN_X){
            set_time.tm_year--;
        }else if( mdata.x >= SMON_SET_DOWN_X &&  mdata.x < SDAY_SET_DOWN_X){
            if(set_time.tm_mon > 0)
                set_time.tm_mon--;
        }else if(mdata.x >= SDAY_SET_DOWN_X &&  mdata.x < SHOUR_SET_DOWN_X){
            if(set_time.tm_mday > 0)
                set_time.tm_mday--;
        }else if(mdata.x >= SHOUR_SET_DOWN_X &&  mdata.x < SMIN_SET_DOWN_X){
            if(set_time.tm_hour > 0)
                set_time.tm_hour--; 
        }else if(mdata.x >= SMIN_SET_DOWN_X &&  mdata.x < SSEC_SET_DOWN_X){
            if(set_time.tm_min > 0) 
                set_time.tm_min--;
        }else if(mdata.x >= SSEC_SET_DOWN_X ){
            if(set_time.tm_sec > 0)
                set_time.tm_sec--;
        }else{
            ;
        }


    }else{
        xw_logsrv_err("not find the srcee mode \n");
    } 

    return;
    
}

static void usr_push_video_button_set(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
{

    window_node_button_t *bt =  (window_node_button_t *)data;
    //printf(" video push bt->color:%x\n",bt->color); 
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


static uint8_t  need_quit_thread = 0;

//need thread mode run
void*   xw_date_show(void *data)
{
    
    
    struct user_set_node_atrr  node_attr;
    
    node_attr.en_node       = 1;
    node_attr.en_freshen    = 0;
    node_attr.move_arrt     = 0;

    //set English font size
    int  ret = 0 ;
    
    //year

    memcpy(node_attr.node_id,XW_DATE_YEAR_WINDOW_ID,strlen(XW_DATE_YEAR_WINDOW_ID));
    window_node_text_t  _text;
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    
    ret = xw_get_node_window_param(XW_DATE_YEAR_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_YEAR_WINDOW_ID);
    } 
    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 4;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //moon
    memcpy(node_attr.node_id,XW_DATE_MONN_WINDOW_ID,strlen(XW_DATE_MONN_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    
    ret = xw_get_node_window_param(XW_DATE_MONN_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_MONN_WINDOW_ID);
    } 

    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //day

    memcpy(node_attr.node_id,XW_DATE_DAY_WINDOW_ID,strlen(XW_DATE_DAY_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    
    ret = xw_get_node_window_param(XW_DATE_DAY_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_DAY_WINDOW_ID);
    } 
    
    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //housr
    memcpy(node_attr.node_id,XW_DATE_HOUR_WINDOW_ID ,strlen(XW_DATE_HOUR_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    ret = xw_get_node_window_param(XW_DATE_HOUR_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_HOUR_WINDOW_ID);
    } 
    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //minues
    memcpy(node_attr.node_id ,XW_DATE_MIN_WINDOW_ID, strlen(XW_DATE_MIN_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    ret = xw_get_node_window_param(XW_DATE_MIN_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_MIN_WINDOW_ID);
    } 

    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    //sec
    memcpy(node_attr.node_id,XW_DATE_SEC_WINDOW_ID,strlen(XW_DATE_SEC_WINDOW_ID));
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    ret = xw_get_node_window_param(XW_DATE_SEC_WINDOW_ID,&_text.x,&_text.y,&_text.asc_width,&_text.font_size);
    if(ret < 0)
    {
        xw_logsrv_err("window;%s get ,x,y w,h fail\n",XW_DATE_SEC_WINDOW_ID);
    } 


    _text.x             += _text.asc_width;
    _text.y             += _text.font_size;
    _text.lens          = 2;
    _text.win_color     = FONT_WIN_COLOR;
    _text.text_color    = FONT_COLOR; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //date time set  button create
    memcpy(node_attr.node_id,XW_DATE_SET_UP_WINDOW_ID  ,strlen(XW_DATE_SET_UP_WINDOW_ID));
    node_attr.en_freshen = NORTHING;  
    window_node_button_t bt;
    memset(&bt,0x0,sizeof(window_node_button_t));
    
    ret = xw_get_node_window_param(XW_DATE_SET_UP_WINDOW_ID,&bt.x,&bt.y,&bt.w,&bt.h);
    if(ret < 0){
    
        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_DATE_SET_UP_WINDOW_ID);
    }
    bt.color            =  0;
    bt.video_set.mouse_left_down = mouse_ldown_button_setup;
    ret = Image_SDK_Create_Button(node_attr,bt);
    
    
    memcpy(node_attr.node_id,XW_DATE_SET_DOWN_WINDOW_ID  ,strlen(XW_DATE_SET_DOWN_WINDOW_ID));
    node_attr.en_freshen = NORTHING;  
    memset(&bt,0x0,sizeof(window_node_button_t));
    ret = xw_get_node_window_param(XW_DATE_SET_DOWN_WINDOW_ID,&bt.x,&bt.y,&bt.w,&bt.h);
    if(ret < 0){
    
        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_DATE_SET_DOWN_WINDOW_ID);
    }

    bt.color            =  0;
    bt.video_set.mouse_left_down = mouse_ldown_button_setdown;
    ret = Image_SDK_Create_Button(node_attr,bt);

    ret = Image_SDK_Set_Node_Disp(XW_DATE_SET_UP_WINDOW_ID,CLOSE_DISP);
    ret = Image_SDK_Set_Node_Disp(XW_DATE_SET_DOWN_WINDOW_ID,CLOSE_DISP);

    memcpy(node_attr.node_id,XW_DATE_SET_WINDOW_ID  ,strlen(XW_DATE_SET_WINDOW_ID));
    node_attr.en_freshen = NORTHING; //NEED_FRESHEN; // NORTHING;  
    memset(&bt,0x0,sizeof(window_node_button_t));
    
    ret = xw_get_node_window_param(XW_DATE_SET_WINDOW_ID,&bt.x,&bt.y,&bt.w,&bt.h);
    if(ret < 0){
    
        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_DATE_SET_WINDOW_ID);
    }
    bt.color            =  FONT_WIN_COLOR ;
    bt.size             =  3;
    bt.user_video_freshen = usr_push_video_button_set;
    bt.video_set.mouse_left_down = mouse_ldown_button_settime;
    bt.video_set.mouse_leave = mouse_leave_settime_func;
    bt.video_set.mouse_offset = mouse_offset_settime_func;
    bt.video_set.mouse_left_up = mouse_lup_settime_func;
    ret = Image_SDK_Create_Button(node_attr,bt);


    

    //get time and push
    time_t  tp = 0;
    struct  tm  *time_now,last_time;
    memset(&last_time,0x0,sizeof(struct tm));
    
    char year[32];
    memset(year,0x0,32);

    last_time.tm_mon    = 13;
    last_time.tm_hour   = 25;
    last_time.tm_min    = 61;

    while(1)
    {
        if(need_quit_thread != 0)
            break;
        // at set timeing ,stop show   
        time(&tp);
        time_now = localtime(&tp);

        if(_state != DATE_WINDOW_SET)
            set_time = *time_now;

        if(last_time.tm_year != set_time.tm_year)
        {
            sprintf(year,"%4d",set_time.tm_year+1900);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_YEAR_WINDOW_ID,year,4);
        }
        if(last_time.tm_mon != set_time.tm_mon)
        {
            sprintf(year,"%02d",set_time.tm_mon+1);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_MONN_WINDOW_ID,year,2);
        }
        if(last_time.tm_mday != set_time.tm_mday)
        {   
            sprintf(year,"%02d",set_time.tm_mday);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_DAY_WINDOW_ID, year,2);
        }
        if(last_time.tm_hour != set_time.tm_hour)
        {
            sprintf(year,"%02d",set_time.tm_hour);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_HOUR_WINDOW_ID , year,2);
        }
        if(last_time.tm_min != set_time.tm_min)
        {
            sprintf(year,"%02d",set_time.tm_min);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_MIN_WINDOW_ID, year,2);
        }
        if(last_time.tm_sec != set_time.tm_sec)
        {
            sprintf(year,"%02d",set_time.tm_sec);
            ret = Image_SDK_Set_Text_Node_Text(XW_DATE_SEC_WINDOW_ID, year,2);
        }
//        xw_logsrv_debug("xxxxxxxxxxxxxxx\n");        
        last_time =  set_time;
        usleep(100000);
    }
    need_quit_thread = 0;  
    return NULL;


}

pthread_t   xw_date_id;

int  xw_date_show_thread(void *data )
{
    
    pthread_create(&xw_date_id,NULL,
            xw_date_show,NULL);

    return 0;
}

int     xw_date_quit_thread(void *data)
{
    need_quit_thread = 1;
    pthread_join(xw_date_id,NULL);
    need_quit_thread = 0;

    
    return 0;

}



