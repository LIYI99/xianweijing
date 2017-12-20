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
    uint8_t     gree_awb;
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


#define     XW_ISP_BUTTON_NOT_CHCEK_COLOR          0xfeee
#define     XW_ISP_BUTTON_CHCEK_COLOR                0xf0f0

#define     XW_ISP_BAR_WIN_COLOR                    0xfeee
#define     XW_ISP_BAR_COLOR                        0xf0f0
#define     XW_ISP_BAR_TEXT_WIN_COLOR               0xfeee
#define     XW_ISP_BAR_TEXT_FONT_COLOR              0xf0f0


#define     XW_ISP_BUTTON_H                         20
#define     XW_ISP_BUTTON_W                         30



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
        xw_isp_p->manal_awb         = 0;
        xw_isp_p->red_awb           = 30;
        xw_isp_p->gree_awb          = 30;
        xw_isp_p->blue_awb          = 30;
        xw_isp_p->colortemp_awb     = 300;

        //ulit
        xw_isp_p->contrast          = 30;
        xw_isp_p->sharpness         = 30;
        xw_isp_p->saturation        = 30;
        xw_isp_p->brightness        = 30;
        xw_isp_p->denoise           = 30;

        //video set
        xw_isp_p->filcker           = 50;
        xw_isp_p->chroma            = 90;
        xw_isp_p->filp_v            = 0;
        xw_isp_p->filp_h            = 0;
        xw_isp_p->hdr               = 0;
    }
    
    //create  AE auto 
    window_node_button_t            _bt;
    window_node_bar_t       _bar;
    window_node_text_t      _text;
    struct user_set_node_atrr       _attr;
    
    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    memset(&_bar,0x0,sizeof(window_node_bar_t));
    memset(&_text,0x0,sizeof(window_node_text_t));
    
    _attr.en_node = 1;
    

    _bt.x = XW_AUTO_EXPOUSURE_WINDOW_X ;
    _bt.y = XW_AUTO_EXPOUSURE_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_exposure == 0)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_AUTO_EXPOUSURE_WINDOW_ID,strlen(XW_AUTO_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 

    //create  AE manul bar
    _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BUTTON_W;
    _bar.h = XW_ISP_BUTTON_H;
    
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }

    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  AE manul text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 2 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    


    //create  AWB auto
    _bt.x = XW_AUTO_WHITE_BALANCE_WINDOW_X;
    _bt.y = XW_AUTO_WHITE_BALANCE_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_awb == 0)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_AUTO_WHITE_BALANCE_WINDOW_ID  ,strlen(XW_AUTO_WHITE_BALANCE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    //create  AWB manul
    _bt.x = XW_MANUL_WHITE_BALANCE_WINDOW_X;
    _bt.y = XW_MANUL_WHITE_BALANCE_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->auto_awb == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_WHITE_BALANCE_WINDOW_ID  ,strlen(XW_MANUL_WHITE_BALANCE_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //create filk 50HZ
    _bt.x = XW_FILCKER_50H_WINDOW_X;
    _bt.y = XW_FILCKER_50H_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->filcker == 0)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id, XW_FILCKER_50H_WINDOW_ID ,strlen(XW_FILCKER_50H_WINDOW_ID));
    ret = Image_SDK_Create_Button(_attr,_bt); 
    //create filk 60HZ    
    _bt.x = XW_FILCKER_60H_WINDOW_X;
    _bt.y = XW_FILCKER_60H_WINDOW_Y; 
    _bt.w = XW_ISP_BUTTON_W;
    _bt.h = XW_ISP_BUTTON_H;
    if(xw_isp_p->filcker == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id, XW_FILCKER_60H_WINDOW_ID ,strlen(XW_FILCKER_60H_WINDOW_ID));
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //create  AWB temp
     _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BUTTON_W;
    _bar.h = XW_ISP_BUTTON_H;
    
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }

    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  AE manul text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 2 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    

    
    //create AWB red
     _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BUTTON_W;
    _bar.h = XW_ISP_BUTTON_H;
    
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }

    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  AE manul text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 2 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    

    //create AWB bule
     _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BUTTON_W;
    _bar.h = XW_ISP_BUTTON_H;
    
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }

    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create  AE manul text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 2 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    
    //craate AWB gree

    _bar.x = XW_AE_MANUL_WINDOW_X ;
    _bar.y = XW_AE_MANUL_WINDOW_Y; 
    _bar.w = XW_ISP_BUTTON_W;
    _bar.h = XW_ISP_BUTTON_H;
    
    if(xw_isp_p->auto_exposure == 1)
    {
        _bt.color = XW_ISP_BUTTON_CHCEK_COLOR;

    }else{
        _bt.color = XW_ISP_BUTTON_NOT_CHCEK_COLOR ; 

    }

    memcpy(_bar.text_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID));
    _bt.video_set.mouse_left_down = NULL;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ) );
    ret = Image_SDK_Create_Bar(_attr,_bar);
    //create text

    _text.x = XW_MANUL_EXPOUSURE_TEXT_WINDOW_X;
    _text.y = XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y;
    _text.win_color     = XW_ISP_BAR_TEXT_WIN_COLOR ;
    _text.text_color    = XW_ISP_BAR_TEXT_FONT_COLOR;
    _text.lens          = 2 ;
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID ) );
    ret = Image_SDK_Create_Text(_attr,_text);
    
    char text_buf[10];
    sprintf(text_buf,"%d",xw_isp_p->exposure_vaule);
    Image_SDK_Set_Text_Node_Text(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID,text_buf,strlen(text_buf));
    






    return  0;
}




static void mouse_offset_main_menu_func(void *data)
{
    window_node_menu_t *mt  = (window_node_menu_t *)data;
    mt->this_node->freshen_arrt = NEED_CLEAR;
    mt->this_node->en_node = 0;
    return ;
}


int xw_main_menu_show(void *data)
{
    
       return ret ;

}


