#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "xw_main_isp_show.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_png_load.h"
#include "image_sdk_core.h"
#include "xw_logsrv.h"
#include "xw_msg_prv.h"
#include "xw_config.h"


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
    uint8_t     filp;
    uint8_t     mirror;
    uint8_t     chroma;
    uint8_t     hdr;
}xw_isp_set_t;



xw_isp_set_t    *xw_isp_p = NULL;
FILE            *xw_isp_fp = NULL;
#define         XW_ISP_FILE_PATH    "/usr/local/bin/isp.cnf"


#define     XW_ISP_BUTTON_NOT_CHCEK_COLOR           CON_SELECT_WINDOW_NOTCHECK_COLOR 
#define     XW_ISP_BUTTON_CHCEK_COLOR               CON_SELECT_WINDOW_CHECK_COLOR  

#define     XW_ISP_BUTTON_H                         10
#define     XW_ISP_BUTTON_W                         18

#define     XW_ISP_BUTTON_REST_H                    32
#define     XW_ISP_BUTTON_REST_W                    39



#define     XW_VIDEO_SET_BUTTON_H                   45
#define     XW_VIDEO_SET_BUTTON_W                   45


#define     XW_VIDEO_SET_BUTTON_LEAVE_COLOR         CON_BUTTON_LEAVE_COLOR  
#define     XW_VIDEO_SET_BUTTON_LDOWN_COLOR         CON_BUTTON_LDOWN_COLOR 
#define     XW_VIDEO_SET_BUTTON_OFFSET_COLOR        CON_BUTTON_OFFSET_COLOR  
#define     XW_VIDEO_SET_BUTTON_SIZE                    2



#define     XW_ISP_BAR_WIN_COLOR                    CON_BAR_WIN_COLOR  
#define     XW_ISP_BAR_COLOR                        CON_BAR_LINE_COLOR
#define     XW_ISP_BAR_LINE_H                       6
#define     XW_ISP_BAR_LINE_W                       120
#define     XW_ISP_BAR_MAX_VALUE                    100
#define     XW_ISP_BAR_STEP                         1
#define     XW_ISP_BAR_STEP_B                       10



#define     XW_ISP_BAR_TEXT_WIN_COLOR               CON_FONT_WIN_COLOR
#define     XW_ISP_BAR_TEXT_FONT_COLOR              CON_FONT_TEXT_COLOR 


static int  xw_isp_exposure_show(void *data);
static int  xw_isp_white_banlance_show(void *data);
static int  xw_isp_filck_show(void *data);
static int  xw_isp_frequen_show(void *data);
static int  xw_video_frequen_show(void *data);
static int  xw_isp_reset_show(void *data);




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
    

    xw_isp_fp = fopen(XW_ISP_FILE_PATH ,"rw+");
#if 0  //not need save data
    if( xw_isp_fp )
    {
        ret = fread(xw_isp_p,1,sizeof(xw_isp_set_t),xw_isp_fp);
    }
#endif
    
    int ndata,size = 4;
    
    //get ae mode
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ENABLE_AE,NULL,size);
    if(ret >= 0){
        xw_isp_p->auto_exposure = ret;
    }else{
        xw_isp_p->auto_exposure = 1;
    }

    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AE_GET_TARGET_RATIO,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->exposure_vaule = ndata;
    }else{
        xw_isp_p->exposure_vaule = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ENABLE_AWB,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->auto_awb = ret;
    }else{
        xw_isp_p->auto_awb = 1;
    }

    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_COLORTEMP,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->colortemp_awb = ndata;
    }else{
        xw_isp_p->colortemp_awb = 300;
    }
    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_CONTRAST,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->contrast = ndata;
    }else{
        xw_isp_p->contrast = 30;
    }

    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SATURATION,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->saturation = ndata;
    }else{
        xw_isp_p->saturation = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_BRIGHTNESS,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->brightness = ndata;
    }else{
        xw_isp_p->brightness = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_DENOISE,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->denoise = ndata;
    }else{
        xw_isp_p->denoise = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SHARPNESS,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->sharpness = ndata;
    }else{
        xw_isp_p->sharpness = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_RED_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->red_awb = ndata;
    }else{
        xw_isp_p->red_awb = 30;
    }
    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_GREEN_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->green_awb = ndata;
    }else{
        xw_isp_p->green_awb = 30;
    }
    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_BLUE_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->blue_awb = ndata;
    }else{
        xw_isp_p->blue_awb = 30;
    }





    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_HUE,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->chroma = ndata;
    }else{
        xw_isp_p->chroma = 30;
    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ANTIFLICKER,(void *)&ndata,size);
    if(ret >= 0){
        
        if(ndata == 50)
            xw_isp_p->filcker = 1;
        else
            xw_isp_p->filcker = 0;

    }else{
        xw_isp_p->filcker = 1;
    }
#if 0
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
        xw_isp_p->filp              = 0;
        xw_isp_p->mirror            = 0;
        xw_isp_p->hdr               = 0;
    }
#endif
    ret = xw_isp_exposure_show(NULL);
    ret = xw_isp_white_banlance_show(NULL);
    //return ;
    ret = xw_isp_filck_show(NULL);
    ret = xw_isp_frequen_show(NULL);
    ret = xw_video_frequen_show(NULL);
    ret  = xw_isp_reset_show(NULL);

    return  ret ;
}

//static window_node_button_t    *auto_ae_bt = NULL;
static void xw_isp_exposure_auto_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
//    if(auto_ae_bt == NULL)
  //       auto_ae_bt = bt; 
    if(xw_isp_p->auto_exposure == 0)
    {
        
        bt->color = XW_ISP_BUTTON_CHCEK_COLOR;
        xw_isp_p->auto_exposure = 1;
        
        //add isp set func auto
        Image_Msg_Send( IDSCAM_IMG_MSG_ENABLE_AE,NULL,0);


    }else{
        xw_isp_p->auto_exposure = 0;
        bt->color = XW_ISP_BUTTON_NOT_CHCEK_COLOR;
        //add isp set func manlu
        int vaule = xw_isp_p->exposure_vaule;
        Image_Msg_Send( IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO,&vaule,4);
    }

    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return ;
}


static void xw_isp_exposure_manul_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    
   
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/6 * 5))
    {
        
        xw_isp_p->exposure_vaule += XW_ISP_BAR_STEP ;  
        if(xw_isp_p->exposure_vaule >  XW_ISP_BAR_MAX_VALUE ){
            xw_isp_p->exposure_vaule = XW_ISP_BAR_MAX_VALUE;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->exposure_vaule >= XW_ISP_BAR_STEP )
        {
            xw_isp_p->exposure_vaule -= XW_ISP_BAR_STEP ;
        }else{
            xw_isp_p->exposure_vaule = 0;
        }
    }else{
        return ;
    }
    if(xw_isp_p->auto_exposure == 1)
    {
       
        xw_isp_p->auto_exposure= 0;
        Image_SDK_Set_Button_Color(XW_AUTO_EXPOUSURE_WINDOW_ID,XW_ISP_BUTTON_NOT_CHCEK_COLOR);
        Image_SDK_Set_Node_En_Freshen(XW_AUTO_EXPOUSURE_WINDOW_ID,NEED_FRESHEN);
    }

    
    bar->now_value = xw_isp_p->exposure_vaule;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[5];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func
    int vaule = xw_isp_p->exposure_vaule;
     Image_Msg_Send( IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO,&vaule,4);


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
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
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
    memcpy(_attr.node_id,XW_MANUL_EXPOUSURE_WINDOW_ID,strlen(XW_MANUL_EXPOUSURE_WINDOW_ID ));
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
    
    return ret;

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
     Image_Msg_Send( IDSCAM_IMG_MSG_ENABLE_AWB,NULL,0);

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
    int vaule = xw_isp_p->red_awb;
    Image_Msg_Send( IDSCAM_IMG_MSG_AWB_SET_ONE_PUSH,&vaule,4);
    return ;


}

static void xw_isp_awb_colortemp_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
   
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*5))
    {
        
        xw_isp_p->colortemp_awb += XW_ISP_BAR_STEP * 10 ;  
        if(xw_isp_p->colortemp_awb >  XW_ISP_BAR_MAX_VALUE * 10 ){
            xw_isp_p->colortemp_awb = XW_ISP_BAR_MAX_VALUE * 10;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->colortemp_awb >= XW_ISP_BAR_STEP * 10 )
        {
            xw_isp_p->colortemp_awb -= (XW_ISP_BAR_STEP * 10) ;
        }else{
            xw_isp_p->colortemp_awb = 0;
        }
    }else{
        return;
    }

    if(xw_isp_p->auto_awb == 1){
        
        Image_SDK_Set_Button_Color(XW_MANUL_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);
        Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
        Image_SDK_Set_Node_En_Freshen(XW_MANUL_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen(XW_AUTO_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        xw_isp_p->auto_awb = 0;
        //return ;
    }

    
    bar->now_value = xw_isp_p->colortemp_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func
    
    int vaule ;
    vaule = xw_isp_p->colortemp_awb;
    Image_Msg_Send( IDSCAM_IMG_MSG_AWB_SET_COLORTEMP,&vaule,4);


    return;


}

static void xw_isp_awb_red_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->red_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->red_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->red_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->red_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->red_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->red_awb = 0;
        }
    }else{
        int va = 0;
         va = (bar->this_node->mouse_data.x - bar->x)* 10 /(bar->w) ;

         xw_isp_p->red_awb = va * XW_ISP_BAR_STEP_B; 
         if(xw_isp_p->red_awb > XW_ISP_BAR_MAX_VALUE)
             xw_isp_p->red_awb = XW_ISP_BAR_MAX_VALUE;
    }
    if(xw_isp_p->auto_awb == 1){
        
        Image_SDK_Set_Button_Color(XW_MANUL_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);
        Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
        Image_SDK_Set_Node_En_Freshen(XW_MANUL_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen(XW_AUTO_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        xw_isp_p->auto_awb = 0;
        //return ;
    }


    bar->now_value = xw_isp_p->red_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif
    //add isp set func 
    int vaule ;
    vaule = xw_isp_p->red_awb;
    Image_Msg_Send( IDSCAM_IMG_MSG_AWB_SET_RED_GAIN,&vaule,4);

    return ;

}

static void xw_isp_awb_green_ldown(void *data)
{
    
    window_node_bar_t *bar  = (window_node_bar_t *)data;
    
    if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->green_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->green_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->green_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->green_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->green_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->green_awb = 0;
        }
    }else{
        return ;
    }
    if(xw_isp_p->auto_awb == 1){
        
        Image_SDK_Set_Button_Color(XW_MANUL_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);
        Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
        Image_SDK_Set_Node_En_Freshen(XW_MANUL_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen(XW_AUTO_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        xw_isp_p->auto_awb = 0;
        //return ;
    }

    
    bar->now_value = xw_isp_p->green_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule = xw_isp_p->green_awb;
    Image_Msg_Send( IDSCAM_IMG_MSG_AWB_SET_GREEN_GAIN,&vaule,4);


    return ;


}

static void xw_isp_awb_blue_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
        if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->blue_awb += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->blue_awb >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->blue_awb = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->blue_awb >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->blue_awb -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->blue_awb = 0;
        }
    }else{
    
        return;
    }
    
    if(xw_isp_p->auto_awb == 1){
        
        Image_SDK_Set_Button_Color(XW_MANUL_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);
        Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
        Image_SDK_Set_Node_En_Freshen(XW_MANUL_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen(XW_AUTO_WHITE_BALANCE_WINDOW_ID,NEED_FRESHEN);
        xw_isp_p->auto_awb = 0;
        //return ;
    }

    bar->now_value = xw_isp_p->blue_awb;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule = xw_isp_p->blue_awb;
    Image_Msg_Send( IDSCAM_IMG_MSG_AWB_SET_BLUE_GAIN,&vaule,4);

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
    
    return ret;


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
    
    int vaule ;
    vaule = 50;
    Image_Msg_Send( IDSCAM_IMG_MSG_ANTIFLICKER,&vaule,4);

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
    int vaule ;
    vaule =  60;
    Image_Msg_Send( IDSCAM_IMG_MSG_ANTIFLICKER,&vaule,4);

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

    return ret;
}


static void xw_isp_denoise_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->denoise += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->denoise >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->denoise = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->denoise >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->denoise -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->denoise = 0;
        }
    }else{
        return;
    }
    
    bar->now_value = xw_isp_p->denoise;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
    //add isp set func 
    int vaule ;
    vaule =  xw_isp_p->denoise;
    Image_Msg_Send( IDSCAM_IMG_MSG_DENOISE,&vaule,4);
 
    return ;


}

static void xw_isp_sharpness_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->sharpness += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->sharpness >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->sharpness = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->sharpness >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->sharpness -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->sharpness = 0;
        }
    }else{
        return ;
    }
    
    bar->now_value = xw_isp_p->sharpness;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule =  xw_isp_p->sharpness;
    Image_Msg_Send( IDSCAM_IMG_MSG_SHARPNESS,&vaule,4);

    return ;


}


static void xw_isp_brightness_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->brightness += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->brightness >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->brightness = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->brightness >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->brightness -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->brightness = 0;
        }
    }else{
        return;
    }
    
    bar->now_value = xw_isp_p->brightness;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule =  xw_isp_p->brightness;
    Image_Msg_Send( IDSCAM_IMG_MSG_BRIGHTNESS,&vaule,4);
   
    return ;


}

static void xw_isp_saturation_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->saturation += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->saturation >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->saturation = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->saturation >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->saturation -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->saturation = 0;
        }
    }else{
    
        return ;
    }
    
    bar->now_value = xw_isp_p->saturation;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule =  xw_isp_p->saturation;
    Image_Msg_Send( IDSCAM_IMG_MSG_SATURATION,&vaule,4);
   
    return ;


}

static void xw_isp_contrast_ldown(void *data)
{
     window_node_bar_t *bar  = (window_node_bar_t *)data;
    
     if(bar->this_node->mouse_data.x > (bar->x + bar->w/8*7))
    {
        
        xw_isp_p->contrast += XW_ISP_BAR_STEP  ;  
        if(xw_isp_p->contrast >  XW_ISP_BAR_MAX_VALUE  ){
            xw_isp_p->contrast = XW_ISP_BAR_MAX_VALUE ;
        }
    }else if(bar->this_node->mouse_data.x < (bar->x + bar->w/8)){

        if(xw_isp_p->contrast >= XW_ISP_BAR_STEP  )
        {
            xw_isp_p->contrast -= (XW_ISP_BAR_STEP ) ;
        }else{
            xw_isp_p->contrast = 0;
        }
    }else{
    
        return ;
    }
    
    bar->now_value = xw_isp_p->contrast;
    bar->this_node->freshen_arrt = NEED_FRESHEN;
#if 0
    char text[6];
    sprintf( text,"%d",bar->now_value);
    Image_SDK_Set_Text_Node_Text(bar->text_id, text,strlen(text));
#endif

    //add isp set func 
    int vaule ;
    vaule =  xw_isp_p->contrast;
    Image_Msg_Send( IDSCAM_IMG_MSG_CONTRAST,&vaule,4);
   
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


    return ret;

}


static void usr_push_isp_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
{

    window_node_button_t *bt =  (window_node_button_t *)data;
    // printf(" video push bt->color:%x\n",bt->color); 
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


static void    xw_video_set_button_offset(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_OFFSET_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return ;


}
static void    xw_video_set_button_leave(void *data){
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    return ;
}


static void     xw_video_filp_button_ldown(void *data)
{
    
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    if(xw_isp_p->filp == 0)
        xw_isp_p->filp = 1;
    else
        xw_isp_p->filp = 0;
    //add video filp set func,set filp vluae
     int vaule ;
    vaule =  1;
    Image_Msg_Send(IDSCAM_IMG_MSG_FLIP,&vaule,4);

    return ;
    
}

static void     xw_video_mirror_button_ldown(void *data)
{

    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    if(xw_isp_p->mirror == 0)
        xw_isp_p->mirror = 0;
    else
        xw_isp_p->mirror = 1;

    //add video mirror set func

    int vaule ;
    vaule =  1;
    Image_Msg_Send( IDSCAM_IMG_MSG_MIRROR,&vaule,4);

    
    return ;

}

static void     xw_video_day_night_button_ldown(void *data)
{

    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    //add video mirror set func
    int vaule  = 0;
    Image_Msg_Send(IDSCAM_IMG_MSG_SET_BW_MODE,&vaule,4);
    return ;
}

static void     xw_video_hdr_button_ldown(void *data)
{
    
    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    if(xw_isp_p->hdr == 0)
        xw_isp_p->hdr = 0;
    else
        xw_isp_p->hdr = 1;

    //add video mirror set func
    int vaule ;
    vaule = xw_isp_p->hdr;
    Image_Msg_Send( IDSCAM_IMG_MSG_SENSOR_HDR_OP_MODE,&vaule,4);
      
    return ;
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
    _bt.w = XW_VIDEO_SET_BUTTON_W;
    _bt.h = XW_VIDEO_SET_BUTTON_H;
    _bt.color =  XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    _bt.size  =  XW_VIDEO_SET_BUTTON_SIZE;
    
    _bt.video_set.mouse_left_down =  xw_video_filp_button_ldown;
    _bt.video_set.mouse_offset = xw_video_set_button_offset;
    _bt.video_set.mouse_leave = xw_video_set_button_leave;

    _bt.user_video_freshen = usr_push_isp_video_button;
    memcpy(_attr.node_id,XW_VIDEO_FILP_WINDOW_ID  ,strlen(XW_VIDEO_FILP_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
    //create mirror button
    _bt.x = XW_VIDEO_MIRROR_WINDOW_X;
    _bt.y = XW_VIDEO_MIRROR_WINDOW_Y;
    _bt.w =XW_VIDEO_SET_BUTTON_W;
    _bt.h = XW_VIDEO_SET_BUTTON_H;
    _bt.color =XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    _bt.size  = XW_VIDEO_SET_BUTTON_SIZE;
    _bt.video_set.mouse_left_down =  xw_video_mirror_button_ldown;
    memcpy(_attr.node_id,XW_VIDEO_MIRROR_WINDOW_ID  ,strlen(XW_VIDEO_MIRROR_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    
     //create  choram button
    _bt.x = XW_VIDEO_NIGHGT_WINDOW_X;
    _bt.y = XW_VIDEO_NIGHGT_WINDOW_Y;
    _bt.w = XW_VIDEO_SET_BUTTON_W;
    _bt.h = XW_VIDEO_SET_BUTTON_H;
    _bt.color = XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    _bt.size  = XW_VIDEO_SET_BUTTON_SIZE;
    _bt.video_set.mouse_left_down =  xw_video_day_night_button_ldown;
    memcpy(_attr.node_id,XW_VIDEO_NIGHGT_WINDOW_ID  ,strlen(XW_VIDEO_NIGHGT_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    

     //create mirror button
    _bt.x = XW_VIDEO_HDR_WINDOW_X;
    _bt.y = XW_VIDEO_HDR_WINDOW_Y;
    _bt.w = XW_VIDEO_SET_BUTTON_W;
    _bt.h =  XW_VIDEO_SET_BUTTON_H;
    _bt.color =  XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    _bt.size  =  XW_VIDEO_SET_BUTTON_SIZE;
    _bt.video_set.mouse_left_down =  xw_video_hdr_button_ldown;
    memcpy(_attr.node_id,XW_VIDEO_HDR_WINDOW_ID  ,strlen(XW_VIDEO_HDR_WINDOW_ID ) );
    ret = Image_SDK_Create_Button(_attr,_bt); 
    

    
    return ret;



}


static void     xw_isp_reset_button_ldown(void *data)
{

    window_node_button_t *bt  = (window_node_button_t *)data;
    bt->color = XW_VIDEO_SET_BUTTON_LDOWN_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    //send reset signel to device serv
    int vaule ,ret = 0,size = 4,ndata;
    vaule =  1;
    ret = Image_Msg_Send( IDSCAM_IMG_MSG_RESET_ISP,&vaule,4);
    if(ret < 0){
        xw_logsrv_err("send reset signle fail \n");
        return ;
    }
    
    //reset isp
    //
    //AE
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ENABLE_AE,NULL,size);
    if(ret >= 0)
    {
        if(ret != xw_isp_p->auto_exposure){
            xw_isp_p->auto_exposure = ret;
            //set color
            if(xw_isp_p->auto_exposure){
               Image_SDK_Set_Button_Color(XW_AUTO_EXPOUSURE_WINDOW_ID,
                        XW_ISP_BUTTON_CHCEK_COLOR );
            }else{
                Image_SDK_Set_Button_Color(XW_AUTO_EXPOUSURE_WINDOW_ID,
                        XW_ISP_BUTTON_NOT_CHCEK_COLOR );
            }
        }
    }
    
    //MANULE AE
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AE_GET_TARGET_RATIO,(void *)&ndata,size);
    if(ret >= 0)
    {
     //   ndata = 70; //test
        xw_isp_p->exposure_vaule = ndata;
        Image_SDK_Set_Bar_Vaule(XW_MANUL_EXPOUSURE_WINDOW_ID,ndata);
    }
    
    //AUDO AWB
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ENABLE_AWB,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->auto_awb = ret;
        if(xw_isp_p->auto_awb){
            Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID,
                        XW_ISP_BUTTON_CHCEK_COLOR );

        }else{
            Image_SDK_Set_Button_Color(XW_AUTO_WHITE_BALANCE_WINDOW_ID,
                        XW_ISP_BUTTON_NOT_CHCEK_COLOR );
        }
    }

    //TEMPCOLOR
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_COLORTEMP,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->colortemp_awb = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_COLOR_TEMP_WINDOW_ID ,ndata);

    }
    //CONTRAST
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_CONTRAST,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->contrast = ndata;
         Image_SDK_Set_Bar_Vaule(XW_ISP_CONTRAST_WINDOW_ID ,ndata);

    }

    //SATRU
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SATURATION,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->saturation = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_SATURATION_WINDOW_ID, ndata);

    }
    //BR
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_BRIGHTNESS,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->brightness = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_BRIGHTNESS_WINDOW_ID, ndata);

    }
    //DENOISE
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_DENOISE,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->denoise = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_DENOISE_WINDOW_ID ,ndata);

    }
    
    //
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_SHARPNESS,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->sharpness = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_SHARPNESS_WINDOW_ID,  ndata);

    }

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_RED_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->red_awb = ndata;
        Image_SDK_Set_Bar_Vaule(XW_ISP_RED_WINDOW_ID ,ndata);

    }
    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_GREEN_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->green_awb = ndata;
        Image_SDK_Set_Bar_Vaule( XW_ISP_GREEN_WINDOW_ID,   ndata);

    }
    
    ret = Image_Msg_Get(IDSCAM_IMG_MSG_AWB_GET_BLUE_GAIN,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->blue_awb = ndata;
        Image_SDK_Set_Bar_Vaule( XW_ISP_BLUE_WINDOW_ID ,  ndata);

    }

#if 0

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_HUE,(void *)&ndata,size);
    if(ret >= 0){
        xw_isp_p->chroma = ndata;
        Image_SDK_Set_Bar_Vaule(XW_MANUL_EXPOUSURE_WINDOW_ID,ndata);

    }else{
        xw_isp_p->chroma = 30;
    }
#endif

    ret = Image_Msg_Get(IDSCAM_IMG_MSG_GET_ANTIFLICKER,(void *)&ndata,size);
    if(ret >= 0)
    {
        ndata = 50;   
        if(ndata == 50){
            xw_isp_p->filcker = 1;
            Image_SDK_Set_Button_Color(XW_FILCKER_60H_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);
            Image_SDK_Set_Button_Color(XW_FILCKER_50H_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);

        }else{
            xw_isp_p->filcker = 0;
            Image_SDK_Set_Button_Color(XW_FILCKER_60H_WINDOW_ID, XW_ISP_BUTTON_CHCEK_COLOR);
            Image_SDK_Set_Button_Color(XW_FILCKER_50H_WINDOW_ID, XW_ISP_BUTTON_NOT_CHCEK_COLOR);

        }

    }

    Image_SDK_Set_Node_En_Freshen(XW_MAIN_WINDOW_ID,NEED_FRESHEN);
    return ;

}



static int  xw_isp_reset_show(void *data)

{
    window_node_button_t        _bt;
    struct user_set_node_atrr   _attr;
    int                         ret = 0 ;

    memset(&_bt,0x0,sizeof(window_node_button_t));
    memset(&_attr,0x0,sizeof(window_node_button_t));
    _attr.en_node = 1;
    //create filp button
    _bt.x = XW_ISP_RESET_WINDOW_X;
    _bt.y = XW_ISP_RESET_WINDOW_Y;
    _bt.w = XW_ISP_BUTTON_REST_W ; 
    _bt.h = XW_ISP_BUTTON_REST_H ;
    _bt.color =  XW_VIDEO_SET_BUTTON_LEAVE_COLOR;
    _bt.size  =  XW_VIDEO_SET_BUTTON_SIZE;
    
    _bt.video_set.mouse_left_down = xw_isp_reset_button_ldown;
    _bt.video_set.mouse_offset = xw_video_set_button_offset;
    _bt.video_set.mouse_leave = xw_video_set_button_leave;
    _bt.user_video_freshen = usr_push_isp_video_button;
    memcpy(_attr.node_id, XW_ISP_RESET_WINDOW_ID,strlen(XW_ISP_RESET_WINDOW_ID));
    ret = Image_SDK_Create_Button(_attr,_bt); 

    return ret;
}


int  xw_main_isp_save_param(char *path)
{
    FILE *fp = NULL;

    if(path){
        fp = fopen(path,"wr+");
    }else{
        fp = xw_isp_fp;
    }
    
    if(fp == NULL)
        return -1;

    int ret = 0;

    fseek(xw_isp_fp,0,SEEK_SET);
    ret = fwrite((void *)xw_isp_p,1,sizeof(xw_isp_set_t),xw_isp_fp);
    
    return ret;

}


int  xw_main_isp_quit_show(void *data){

    if(xw_isp_fp)  
        fclose(xw_isp_fp);
    xw_isp_fp = NULL;
    if(xw_isp_p)
        free(xw_isp_p);
    xw_isp_p = NULL;
    return 0;

}



