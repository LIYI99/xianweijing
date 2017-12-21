#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "xw_main_isp_show.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_png_load.h"
#include "gk_image_sdk_new.h"


typedef struct  xw_isp_set{
    
    uint8_t     auto_exposure; // 0: auto 1:manul
    uint8_t     exposure_vaule;
    
    uint8_t     filcker;
    
    uint8_t     auto_awb;     //0: auto 1:manul
//    uint8_t     manal_awb;
    uint8_t     red_awb;
    uint8_t     green_awb;
    uint8_t     blue_awb;
    uint16_t    colortemp_awb;
    
    uint8_t     contrast;
    uint8_t     saturation;
    uint8_t     brightness;
    uint8_t     sharpness;
    uint8_t     denoise;
    
    //video set
    uint8_t     filp_h;
    uint8_t     filp_v;
    uint8_t     chroma;
    uint8_t     hdr;
}xw_isp_set_t;



xw_isp_set_t    *xw_isp_p = NULL;
FILE            *xw_isp_fp = NULL;
#define         XW_ISP_FILE_PATH    "/usr/local/bin/isp.cnf"


#define     XW_ISP_BUTTON_NOT_CHCEK_COLOR           0xfeee
#define     XW_ISP_BUTTON_CHCEK_COLOR               0xf0f0
#define     XW_ISP_BUTTON_H                         10
#define     XW_ISP_BUTTON_W                         18




#define     XW_ISP_BAR_WIN_COLOR                    0xfeee
#define     XW_ISP_BAR_COLOR                        0xf00f
#define     XW_ISP_BAR_LINE_H                       6
#define     XW_ISP_BAR_LINE_W                       120
#define     XW_ISP_BAR_MAX_VALUE                    100
#define     XW_ISP_BAR_STEP                         10



#define     XW_ISP_BAR_TEXT_WIN_COLOR               0xfeee
#define     XW_ISP_BAR_TEXT_FONT_COLOR              0xf0f0


static int  xw_isp_exposure_show(void *data);
static int  xw_isp_white_banlance_show(void *data);
static int  xw_isp_filck_show(void *data);
static int  xw_isp_frequen_show(void *data);
static int  xw_video_frequen_show(void *data);




int  xw_main_isp_show(void *data)

{
    if(xw_isp_p != NULL)
        return -1;

    xw_isp_p  = (xw_isp_set_t *)malloc(sizeof(xw_isp_set_t));
    if(xw_isp_p == NULL)
        return -2;
    memset(xw_isp_p,0x0,sizeof(xw_isp_set_t));


    int ret  = 0 ;
    //load isp data 

    xw_isp_fp = fopen(XW_ISP_FILE_PATH ,"rb");
    if( xw_isp_fp )
    {
        ret = fread(xw_isp_p,1,sizeof(xw_isp_set_t),xw_isp_fp);
    }
    //init default
    if(ret <= 0)
    {
        //AE
        xw_isp_p->auto_exposure     = 1;
        xw_isp_p->exposure_vaule    = 30;
        //AWB
        xw_isp_p->auto_awb          = 1;
        xw_isp_p->red_awb           = 30;
        xw_isp_p->green_awb          = 30;
        xw_isp_p->blue_awb          = 30;
        xw_isp_p->colortemp_awb     = 300;

        //ulit
        xw_isp_p->contrast          = 30;
        xw_isp_p->sharpness         = 30;
        xw_isp_p->saturation        = 30;
        xw_isp_p->brightness        = 30;
        xw_isp_p->denoise           = 30;

        //video set
        xw_isp_p->filcker           = 1;  // 1: 50HZ  0:60HZ
        xw_isp_p->chroma            = 90;
        xw_isp_p->filp_v            = 0;
        xw_isp_p->filp_h            = 0;
        xw_isp_p->hdr               = 0;
    }
    //add menu

    ret = xw_isp_exposure_show(NULL);
    ret = xw_isp_white_banlance_show(NULL);
    //return ;
    ret = xw_isp_filck_show(NULL);
    ret = xw_isp_frequen_show(NULL);
    return 0;
    ret = xw_video_frequen_show(NULL);

    return  0;
}

static void xw_isp_exposure_auto_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    if(xw_isp_p->auto_exposure == 0)
    {
        
        bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
        xw_isp_p->auto_exposure = 1;
        //add isp set func

    }else{
        xw_isp_p->auto_exposure = 0;
        bt->color = XW_ISP_BUTTON_NOT_CHCEK_COLOR;
    }

    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return ;
}

static void xw_isp_exposure_manul_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    if(xw_isp_p->auto_exposure == 1)
        return ;

    if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->exposure_vaule += XW_ISP_BAR_STEP ;  
        if(xw_isp_p->exposure_vaule >  XW_ISP_BAR_MAX_VALUE ){
            xw_isp_p->exposure_vaule = XW_ISP_BAR_MAX_VALUE;
        }
    }else{

        if(xw_isp_p->exposure_vaule >= XW_ISP_BAR_STEP )
        {
            xw_isp_p->exposure_vaule -= XW_ISP_BAR_STEP ;
        }else{
            xw_isp_p->exposure_vaule = 0;
        }
    }
    
    bar->now_value = xw_isp_p->exposure_vaule;

    
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[5];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    
    //add isp set func

   // printf("bar->exposure_vaule:%d\n",bar->now_value);
    return;

}


static int  xw_isp_exposure_show(void *data)
{
    
    
    if(xw_isp_p == NULL)
        return -1;
    //create  AE auto 
    window_node_button_t        _bt;
    window_node_bar_t           _bar;
    window_node_text_t          _text;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;

    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    memset(&_bar,0x0,sizeof(window_node_bar_t));
    memset(&_text,0x0,sizeof(window_node_text_t));
    _attr.en_node = 1;
    

    _bt.x = XW_AUTO_EXPOUSURE_WINDOW_X ;
    _bt.y = XW_AUTO_EXPOUSURE_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = xw_isp_exposure_auto_ldown;
    memcpy(_attr.node_id,XW_AUTO_EXPOUSURE_WINDOW_ID,strlen(XW_AUTO_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 


    //create  AE manul bar
    _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color   = XW_ISP_BAR_COLOR;
    _bar.now_value = xw_isp_p->exposure_vaule;
    _bar.max_value = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down = xw_isp_exposure_manul_ldown;
    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  AE manul text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    
    return 0;

}


static void xw_isp_white_banlance_auto_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    if(xw_isp_p->auto_awb == 1)
        return;
    xw_isp_p->auto_awb = 1;
    bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    Image_SDK_Set_Button_Color(XW_MANUL_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
    //add isp set func

    return ;
}

static void xw_isp_white_banlance_manul_ldown(void *data)
{
    window_node_button_t *bt  = (window_node_button_t *)data;
    if(xw_isp_p->auto_awb == 0)
        return;
    xw_isp_p->auto_awb = 0;
    bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
    //add isp set func
    return ;


}

static void xw_isp_awb_colortemp_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    if(xw_isp_p->auto_awb == 1)
        return ;

    if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->colortemp_awb += XW_ISP_BAR_STEP * 10 ;  
        if(xw_isp_p->colortemp_awb >  XW_ISP_BAR_MAX_VALUE * 10 ){
            xw_isp_p->colortemp_awb = XW_ISP_BAR_MAX_VALUE * 10;
        }
    }else{

        if(xw_isp_p->colortemp_awb >= XW_ISP_BAR_STEP * 10 )
        {
            xw_isp_p->colortemp_awb -= (XW_ISP_BAR_STEP * 10) ;
        }else{
            xw_isp_p->colortemp_awb = 0;
        }
    }
    
    bar->now_value = xw_isp_p->colortemp_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    
    //add isp set func
    


    return;


}

static void xw_isp_awb_red_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    if(xw_isp_p->auto_awb == 1)
        return ;
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->red_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->red_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->red_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->red_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->red_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->red_awb = 0;
        }
    }
    
    bar->now_value = xw_isp_p->red_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;

}

static void xw_isp_awb_green_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    if(xw_isp_p->auto_awb == 1)
        return ;
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->green_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->green_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->green_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->green_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->green_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->green_awb = 0;
        }
    }
    
    bar->now_value = xw_isp_p->green_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}

static void xw_isp_awb_blue_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    if(xw_isp_p->auto_awb == 1)
        return ;
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->blue_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->blue_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->blue_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->blue_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->blue_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->blue_awb = 0;
        }
    }
    
    bar->now_value = xw_isp_p->blue_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}


static int  xw_isp_white_banlance_show(void *data)
{
    
    
    window_node_button_t        _bt;
    window_node_bar_t           _bar;
    window_node_text_t          _text;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;

    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    memset(&_bar,0x0,sizeof(window_node_bar_t));
    memset(&_text,0x0,sizeof(window_node_text_t));
    _attr.en_node = 1;

    //create  AWB auto
    _bt.x = XW_AUTO_WHITE_BALANCE_WINDOW_X;
    _bt.y = XW_AUTO_WHITE_BALANCE_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_awb == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down =  xw_isp_white_banlance_auto_ldown;
    memcpy(_attr.node_id,XW_AUTO_WHITE_BALANCE_WINDOW_ID  ,strlen(XW_AUTO_WHITE_BALANCE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    //create  AWB manul
    _bt.x = XW_MANUL_WHITE_BALANCE_WINDOW_X;
    _bt.y = XW_MANUL_WHITE_BALANCE_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_awb == 0)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = xw_isp_white_banlance_manul_ldown;
    memcpy(_attr.node_id,XW_MANUL_WHITE_BALANCE_WINDOW_ID  ,strlen(XW_MANUL_WHITE_BALANCE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
   
   
    //create  AWB temp
    _bar.x = XW_ISP_COLOR_TEMP_WINDOW_X ;
    _bar.y = XW_ISP_COLOR_TEMP_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->colortemp_awb;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE * 10;
    _bar.video_set.mouse_left_down =  xw_isp_awb_colortemp_ldown;
    //set bar text id 
    memcpy(_bar.text_id,XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID,strlen(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_COLOR_TEMP_WINDOW_ID,strlen(XW_ISP_COLOR_TEMP_WINDOW_ID ));
    ret = Image_SDK_Create_Bar(_attr,_bar);
    
    //create  temp text
    _text.x = XW_ISP_COLOR_TEMP_TEXT_WINDOW_X;
    _text.y = XW_ISP_COLOR_TEMP_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 4;
    memcpy(_attr.node_id,XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID,strlen(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->colortemp_awb);
    printf("colortemp:%s\n",text_buf);
    Image_SDK_Set_Text_Node_Text(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    

    
    //create AWB red
     _bar.x = XW_ISP_RED_WINDOW_X  ;
    _bar.y = XW_ISP_RED_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->red_awb;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down = xw_isp_awb_red_ldown;
    memcpy(_bar.text_id,XW_ISP_RED_TEXT_WINDOW_ID,strlen(XW_ISP_RED_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_RED_WINDOW_ID,strlen(XW_ISP_RED_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_RED_TEXT_WINDOW_X;
    _text.y = XW_ISP_RED_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_RED_TEXT_WINDOW_ID,strlen(XW_ISP_RED_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->red_awb);
    Image_SDK_Set_Text_Node_Text(XW_ISP_RED_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

    
    //create AWB green
    _bar.x = XW_ISP_GREEN_WINDOW_X  ;
    _bar.y = XW_ISP_GREEN_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->green_awb;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down = xw_isp_awb_green_ldown;
    memcpy(_bar.text_id,XW_ISP_GREEN_TEXT_WINDOW_ID,strlen(XW_ISP_GREEN_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_GREEN_WINDOW_ID,strlen(XW_ISP_GREEN_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  gree text
    _text.x = XW_ISP_GREEN_TEXT_WINDOW_X;
    _text.y = XW_ISP_GREEN_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_GREEN_TEXT_WINDOW_ID,strlen(XW_ISP_GREEN_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->green_awb);
    Image_SDK_Set_Text_Node_Text(XW_ISP_GREEN_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

    //craate AWB bule
    _bar.x = XW_ISP_BLUE_WINDOW_X  ;
    _bar.y = XW_ISP_BLUE_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->blue_awb;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down = xw_isp_awb_blue_ldown;
    memcpy(_bar.text_id,XW_ISP_BLUE_TEXT_WINDOW_ID,strlen(XW_ISP_BLUE_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_BLUE_WINDOW_ID,strlen(XW_ISP_BLUE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create text
    _text.x = XW_ISP_BLUE_TEXT_WINDOW_X;
    _text.y = XW_ISP_BLUE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_BLUE_TEXT_WINDOW_ID,strlen(XW_ISP_BLUE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->blue_awb);
    Image_SDK_Set_Text_Node_Text(XW_ISP_BLUE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    
    return 0;


}



static void xw_isp_filck_50HZ_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    if(xw_isp_p->filcker == 1)
        return;
    xw_isp_p->filcker = 1;
    bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    Image_SDK_Set_Button_Color(XW_FILCKER_60H_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
    //add isp set func

    return ;
}

static void xw_isp_filck_60HZ_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    if(xw_isp_p->filcker == 0)
        return;
    xw_isp_p->filcker = 0;
    bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    Image_SDK_Set_Button_Color(XW_FILCKER_50H_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
    //add isp set func

    return ;
}



static int  xw_isp_filck_show(void *data)

{

    
    window_node_button_t        _bt;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;

    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    _attr.en_node = 1;

      //create filk 50HZ
    _bt.x = XW_FILCKER_50H_WINDOW_X;
    _bt.y = XW_FILCKER_50H_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->filcker == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = xw_isp_filck_50HZ_ldown;
    memcpy(_attr.node_id, XW_FILCKER_50H_WINDOW_ID ,strlen(XW_FILCKER_50H_WINDOW_ID));
    ret = Image_SDK_Create_Button(_attr,_bt); 
    //create filk 60HZ    
    _bt.x = XW_FILCKER_60H_WINDOW_X;
    _bt.y = XW_FILCKER_60H_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->filcker == 0)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = xw_isp_filck_60HZ_ldown;
    memcpy(_attr.node_id, XW_FILCKER_60H_WINDOW_ID ,strlen(XW_FILCKER_60H_WINDOW_ID));
    ret = Image_SDK_Create_Button(_attr,_bt); 

    return 0;
}


static void xw_isp_denoise_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->denoise += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->denoise >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->denoise = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->denoise >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->denoise -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->denoise = 0;
        }
    }
    
    bar->now_value = xw_isp_p->denoise;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}

static void xw_isp_sharpness_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->sharpness += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->sharpness >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->sharpness = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->sharpness >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->sharpness -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->sharpness = 0;
        }
    }
    
    bar->now_value = xw_isp_p->sharpness;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}


static void xw_isp_brightness_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->brightness += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->brightness >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->brightness = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->brightness >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->brightness -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->brightness = 0;
        }
    }
    
    bar->now_value = xw_isp_p->brightness;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}

static void xw_isp_saturation_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->saturation += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->saturation >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->saturation = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->saturation >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->saturation -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->saturation = 0;
        }
    }
    
    bar->now_value = xw_isp_p->saturation;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}

static void xw_isp_contrast_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/2))
    {
        
        xw_isp_p->contrast += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->contrast >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->contrast = XW_ISP_BAR_MAX_VALUE ;
        }
    }else{

        if(xw_isp_p->contrast >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->contrast -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->contrast = 0;
        }
    }
    
    bar->now_value = xw_isp_p->contrast;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    
    return ;


}






static int  xw_isp_frequen_show(void *data)
{

    window_node_bar_t           _bar;
    window_node_text_t          _text;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;
    char text_buf[10];

    memset(&_attr,0x0,sizeof(window_node_button_t));
    memset(&_bar,0x0,sizeof(window_node_bar_t));
    memset(&_text,0x0,sizeof(window_node_text_t));
    _attr.en_node = 1;
    
    //create denosie
    _bar.x = XW_ISP_DENOISE_WINDOW_X  ;
    _bar.y = XW_ISP_DENOISE_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->denoise;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down =  xw_isp_denoise_ldown;
    memcpy(_bar.text_id,XW_ISP_DENOISE_TEXT_WINDOW_ID,strlen(XW_ISP_DENOISE_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_DENOISE_WINDOW_ID,strlen(XW_ISP_DENOISE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_DENOISE_TEXT_WINDOW_X;
    _text.y = XW_ISP_DENOISE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_DENOISE_TEXT_WINDOW_ID,strlen(XW_ISP_DENOISE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->denoise);
    Image_SDK_Set_Text_Node_Text(XW_ISP_DENOISE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

    //sharpness
    _bar.x = XW_ISP_SHARPNESS_WINDOW_X  ;
    _bar.y = XW_ISP_SHARPNESS_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->sharpness;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down =  xw_isp_sharpness_ldown;
    memcpy(_bar.text_id,XW_ISP_SHARPNESS_TEXT_WINDOW_ID,strlen(XW_ISP_SHARPNESS_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_SHARPNESS_WINDOW_ID,strlen(XW_ISP_SHARPNESS_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_SHARPNESS_TEXT_WINDOW_X;
    _text.y = XW_ISP_SHARPNESS_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_SHARPNESS_TEXT_WINDOW_ID,strlen(XW_ISP_SHARPNESS_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->sharpness);
    Image_SDK_Set_Text_Node_Text(XW_ISP_SHARPNESS_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

    //brightness
     _bar.x = XW_ISP_BRIGHTNESS_WINDOW_X  ;
    _bar.y = XW_ISP_BRIGHTNESS_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->brightness;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;

    _bar.video_set.mouse_left_down =  xw_isp_brightness_ldown;
    memcpy(_bar.text_id,XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID,strlen(XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_BRIGHTNESS_WINDOW_ID,strlen(XW_ISP_BRIGHTNESS_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_BRIGHTNESS_TEXT_WINDOW_X;
    _text.y = XW_ISP_BRIGHTNESS_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID,strlen(XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->brightness);
    Image_SDK_Set_Text_Node_Text(XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

    //staruration
    
     _bar.x = XW_ISP_SATURATION_WINDOW_X  ;
    _bar.y = XW_ISP_SATURATION_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->saturation;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down =  xw_isp_saturation_ldown ;
    memcpy(_bar.text_id,XW_ISP_SATURATION_TEXT_WINDOW_ID,strlen(XW_ISP_SATURATION_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_SATURATION_WINDOW_ID,strlen(XW_ISP_SATURATION_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_SATURATION_TEXT_WINDOW_X;
    _text.y = XW_ISP_SATURATION_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3;
    memcpy(_attr.node_id,XW_ISP_SATURATION_TEXT_WINDOW_ID,strlen(XW_ISP_SATURATION_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->saturation);
    Image_SDK_Set_Text_Node_Text(XW_ISP_SATURATION_TEXT_WINDOW_ID,text_buf,strlen(text_buf));

     //contrast
     _bar.x = XW_ISP_CONTRAST_WINDOW_X  ;
    _bar.y = XW_ISP_CONTRAST_WINDOW_Y;
    _bar.w = XW_ISP_BAR_LINE_W;
    _bar.h = XW_ISP_BAR_LINE_H;
    _bar.bar_color  = XW_ISP_BAR_COLOR; 
    _bar.now_value  = xw_isp_p->contrast;
    _bar.max_value  = XW_ISP_BAR_MAX_VALUE;
    _bar.video_set.mouse_left_down =  xw_isp_contrast_ldown;
    memcpy(_bar.text_id,XW_ISP_CONTRAST_TEXT_WINDOW_ID,strlen(XW_ISP_CONTRAST_TEXT_WINDOW_ID));
    memcpy(_attr.node_id,XW_ISP_CONTRAST_WINDOW_ID,strlen(XW_ISP_CONTRAST_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  red text
    _text.x = XW_ISP_CONTRAST_TEXT_WINDOW_X;
    _text.y = XW_ISP_CONTRAST_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 3 ;
    memcpy(_attr.node_id,XW_ISP_CONTRAST_TEXT_WINDOW_ID,strlen(XW_ISP_CONTRAST_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    sprintf(text_buf,"%d",xw_isp_p->contrast);
    Image_SDK_Set_Text_Node_Text(XW_ISP_CONTRAST_TEXT_WINDOW_ID,text_buf,strlen(text_buf));


    return 0;

}

static int  xw_video_frequen_show(void *data)
{
    
    window_node_button_t        _bt;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;

    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    _attr.en_node = 1;
    //create filp button
    _bt.x = XW_VIDEO_FILP_WINDOW_X;
    _bt.y = XW_VIDEO_FILP_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    _bt.color = 0xfeee;
    _bt.size  = 2;
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_VIDEO_FILP_WINDOW_ID  ,strlen(XW_VIDEO_FILP_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //create mirror button
    _bt.x = XW_VIDEO_MIRROR_WINDOW_X;
    _bt.y = XW_VIDEO_MIRROR_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    _bt.color = 0xfeee;
    _bt.size  = 2;
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_VIDEO_MIRROR_WINDOW_ID  ,strlen(XW_VIDEO_MIRROR_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
     //create  choram button
    _bt.x = XW_VIDEO_NIGHGT_WINDOW_X;
    _bt.y = XW_VIDEO_NIGHGT_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    _bt.color = 0xfeee;
    _bt.size  = 2;
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_VIDEO_NIGHGT_WINDOW_ID  ,strlen(XW_VIDEO_NIGHGT_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    

     //create mirror button
    _bt.x = XW_VIDEO_HDR_WINDOW_X;
    _bt.y = XW_VIDEO_HDR_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    _bt.color = 0xfeee;
    _bt.size  = 2;
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_VIDEO_HDR_WINDOW_ID  ,strlen(XW_VIDEO_HDR_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    

    
    return 0;



}






