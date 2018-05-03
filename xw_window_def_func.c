
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_window_wh_df.h"
#include "xw_window_def_func.h"
#include "xw_logsrv.h"

#define  NODE_ID_MAX_LEN        10
#define  NODE_SAVE_MAX_NUMS     300
struct node_def_params{
    char        node_id[NODE_ID_MAX_LEN];
    uint16_t    x;
    uint16_t    y;
    uint16_t    w;
    uint16_t    h;
    uint16_t    linesize;
};

typedef struct node_def_hanlde{
    srcee_mode_type     mode;           //0: 1080 1:600 2: other 
    
    uint16_t            srcee_w;
    uint16_t            srcee_h;
    
    uint16_t            mouse_w;
    uint16_t            mouse_h;

    int                 cnt;
    struct  node_def_params node_params[NODE_SAVE_MAX_NUMS];
}node_def_hanlde_t;



node_def_hanlde_t   *def_params = NULL;





static void xw_node_params_add_1080P(void);
static void xw_node_params_add_600P(void);





static void inline  xw_node_def_add(char *node_id,uint16_t x,uint16_t y,uint16_t w,uint16_t h)
{
    
    if(strlen(node_id) > (NODE_ID_MAX_LEN -1) )
    {
        xw_logsrv_err("input node_id too long :%s\n",node_id);
        return;
    }
    strcpy(def_params->node_params[def_params->cnt].node_id,node_id);
    def_params->node_params[def_params->cnt].x = x;
    def_params->node_params[def_params->cnt].y = y;
    def_params->node_params[def_params->cnt].w = w;
    def_params->node_params[def_params->cnt].h = h;
    def_params->cnt++;
    return ;

}

int xw_window_def_params_init(srcee_mode_type mode)
{
    
    if(def_params)
        return -1;
    
    if(mode > 2)
        return -1;

    def_params = (node_def_hanlde_t *) malloc(sizeof(node_def_hanlde_t));
    if(def_params == NULL)
        return -2;
    memset(def_params,0x0,sizeof(node_def_hanlde_t));
    def_params->mode = mode;

    //dev param init

    if(def_params->mode == SRCEE_MODE_1080)
    {
        def_params->srcee_w = 1920;
        def_params->srcee_h = 1080;
        def_params->mouse_w = 30;
        def_params->srcee_h = 30;

    }else if(def_params->mode == SRCEE_MODE_600)
    {
        def_params->srcee_w = 1024;
        def_params->srcee_h = 600;
        def_params->mouse_w = 20;
        def_params->srcee_h = 20;
 
    }else
    {
        xw_logsrv_err("the mode not define :%d\n",mode);
    }
   
    //init window def param

    if(def_params->mode == SRCEE_MODE_1080) //hdmi
    {  
        xw_node_params_add_1080P();
    }else if(def_params->mode == SRCEE_MODE_600)
    { //lcd
         xw_node_params_add_600P();
    }else{
        xw_logsrv_err("set srcee mode err \n");
    }
    
    return 0;

}

srcee_mode_type xw_get_node_window_mode(void)
{
    
    if(!def_params)
        return SRCEE_MODE_ERR;
    return def_params->mode;

}

int xw_get_node_window_param(char * window_id,uint16_t *x,uint16_t *y,uint16_t *w,uint16_t *h)
{
    if(window_id == NULL)
        return -1;
    
    if(def_params == NULL)
        return -1;


    int i  = 0;

    for(i = 0 ; i < def_params->cnt;i++)
    {
        if(strcmp(window_id,def_params->node_params[i].node_id) == 0)
        {
           
            if(x)
                *x = def_params->node_params[i].x;
            if(y)
                *y = def_params->node_params[i].y;
            if(w)
                *w = def_params->node_params[i].w;
            if(h)
                *h = def_params->node_params[i].h;
            return 0;

        }

    } 
    
    return -1;    

}

int xw_window_def_params_deinit(void){

    if(!def_params)
        return -1;
    free(def_params);
    def_params = NULL;
    return 0;

}




//1080P define params load

static void xw_node_params_add_1080P(void)
{
        //ada main mueu        
        xw_node_def_add(XW_MAIN_WINDOW_ID,XW_MAIN_WINDOW_X,XW_MAIN_WINDOW_Y,XW_MAIN_MENU_W,XW_MAIN_MENU_H);
        
        //line manger of main
        
        //close of open lines
        
        xw_node_def_add(XW_LINE_RARR_WINDOW_ID ,XW_LINE_RARR_WINDOW_X,
                XW_LINE_RARR_WINDOW_Y,XW_LINE_RARR_WINDOW_W , XW_LINE_RARR_WINDOW_H) ;
        

        xw_node_def_add(XW_LINE_SHOW_WINDOW_ID ,XW_LINE_SHOW_WINDOW_X,
                XW_LINE_SHOW_WINDOW_Y,XW_MAINOF_BUTTON_W ,XW_MAINOF_BUTTON_H);
        //selec lines array   
        xw_node_def_add(XW_LINE_CHOICE_WINDOW_ID, XW_LINE_CHOICE_WINDOW_X,XW_LINE_CHOICE_WINDOW_Y,XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //set line size
        xw_node_def_add(XW_LINE_SET_SIZE_WINDOW_ID ,XW_LINE_SET_SIZE_WINDOW_X,
                XW_LINE_SET_SIZE_WINDOW_Y,XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //set line color
        xw_node_def_add( XW_LINE_SET_COLOR_WINDOW_ID,XW_LINE_SET_COLOR_WINDOW_X,XW_LINE_SET_COLOR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
       
        //select line
        xw_node_def_add( XW_LINE_SELECT_LINE_WINDOW_ID  ,XW_LINE_SELECT_LINE_WINDOW_X,XW_LINE_SELECT_LINE_WINDOW_Y,
            XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
         
        //line lock or video lock
        xw_node_def_add(XW_LINE_LOCK_WINDOW_ID,XW_LINE_LOCK_WINDOW_X,XW_LINE_LOCK_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //line clear
        xw_node_def_add(XW_LINE_CLEAR_WINDOW_ID ,XW_LINE_CLAER_WINDOW_X,XW_LINE_CLAER_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

        
        //line params save
        xw_node_def_add(XW_LINE_SAVE_WINDOW_ID,XW_LINE_SAVE_WINDOW_X,XW_LINE_SAVE_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //add video set button of main 
        
        //filp
        xw_node_def_add(XW_VIDEO_FILP_WINDOW_ID,XW_VIDEO_FILP_WINDOW_X,XW_VIDEO_FILP_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //mirror
        xw_node_def_add(XW_VIDEO_MIRROR_WINDOW_ID ,XW_VIDEO_MIRROR_WINDOW_X,XW_VIDEO_MIRROR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

       //night or day
        xw_node_def_add(XW_VIDEO_NIGHGT_WINDOW_ID ,XW_VIDEO_NIGHGT_WINDOW_X,XW_VIDEO_NIGHGT_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
    
        //hdr
        xw_node_def_add(XW_VIDEO_HDR_WINDOW_ID,XW_VIDEO_HDR_WINDOW_X,XW_VIDEO_HDR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //isp  set button of main  
        
        //color temp
        xw_node_def_add(XW_ISP_COLOR_TEMP_WINDOW_ID ,XW_ISP_COLOR_TEMP_WINDOW_X,XW_ISP_COLOR_TEMP_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID  ,XW_ISP_COLOR_TEMP_TEXT_WINDOW_X,
                XW_ISP_COLOR_TEMP_TEXT_WINDOW_Y,XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        //wab bule
        xw_node_def_add(XW_ISP_BLUE_WINDOW_ID   ,XW_ISP_BLUE_WINDOW_X,XW_ISP_BLUE_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_BLUE_TEXT_WINDOW_ID ,XW_ISP_BLUE_TEXT_WINDOW_X,XW_ISP_BLUE_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        //wab red
        xw_node_def_add(XW_ISP_RED_WINDOW_ID,XW_ISP_RED_WINDOW_X,XW_ISP_RED_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_RED_TEXT_WINDOW_ID ,XW_ISP_RED_TEXT_WINDOW_X,XW_ISP_RED_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        
        // wab green 
        xw_node_def_add( XW_ISP_GREEN_WINDOW_ID   ,XW_ISP_GREEN_WINDOW_X,XW_ISP_GREEN_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_GREEN_TEXT_WINDOW_ID ,XW_ISP_GREEN_TEXT_WINDOW_X,XW_ISP_GREEN_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        //denoise
        xw_node_def_add(XW_ISP_DENOISE_WINDOW_ID ,XW_ISP_DENOISE_WINDOW_X,XW_ISP_DENOISE_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_DENOISE_TEXT_WINDOW_ID ,XW_ISP_DENOISE_TEXT_WINDOW_X,XW_ISP_DENOISE_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        
        //sharpness
        xw_node_def_add(XW_ISP_SHARPNESS_WINDOW_ID ,XW_ISP_SHARPNESS_WINDOW_X,XW_ISP_SHARPNESS_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SHARPNESS_TEXT_WINDOW_ID  ,XW_ISP_SHARPNESS_TEXT_WINDOW_X,XW_ISP_SHARPNESS_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        //brightness
        xw_node_def_add(XW_ISP_BRIGHTNESS_WINDOW_ID  ,XW_ISP_BRIGHTNESS_WINDOW_X,XW_ISP_BRIGHTNESS_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add( XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID  ,XW_ISP_BRIGHTNESS_TEXT_WINDOW_X,XW_ISP_BRIGHTNESS_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        //staturation
        xw_node_def_add(XW_ISP_SATURATION_WINDOW_ID ,XW_ISP_SATURATION_WINDOW_X,XW_ISP_SATURATION_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SATURATION_TEXT_WINDOW_ID ,XW_ISP_SATURATION_TEXT_WINDOW_X,XW_ISP_SATURATION_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        //contrast
        xw_node_def_add(XW_ISP_CONTRAST_WINDOW_ID ,XW_ISP_CONTRAST_WINDOW_X,XW_ISP_CONTRAST_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_CONTRAST_TEXT_WINDOW_ID  ,XW_ISP_CONTRAST_TEXT_WINDOW_X,XW_ISP_CONTRAST_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
    
        //manu ae
        xw_node_def_add( XW_MANUL_EXPOUSURE_WINDOW_ID   ,XW_AE_MANUL_WINDOW_X,XW_AE_MANUL_WINDOW_Y,
               XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add( XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID, XW_MANUL_EXPOUSURE_TEXT_WINDOW_X ,
                XW_MANUL_EXPOUSURE_TEXT_WINDOW_Y,XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
    
       //
       //50hz filcker 
       xw_node_def_add( XW_FILCKER_50H_WINDOW_ID ,XW_FILCKER_50H_WINDOW_X,XW_FILCKER_50H_WINDOW_Y,
                XW_MAINOF_CHECK_WINDOW_W, XW_MAINOF_CHECK_WINDOW_H);
        
       //60hz filcker
       xw_node_def_add( XW_FILCKER_60H_WINDOW_ID ,XW_FILCKER_60H_WINDOW_X,XW_FILCKER_60H_WINDOW_Y,
                XW_MAINOF_CHECK_WINDOW_W, XW_MAINOF_CHECK_WINDOW_H);
        
        //auto wab
        xw_node_def_add( XW_AUTO_WHITE_BALANCE_WINDOW_ID ,XW_AUTO_WHITE_BALANCE_WINDOW_X ,XW_AUTO_WHITE_BALANCE_WINDOW_Y,
                XW_MAINOF_CHECK_WINDOW_W, XW_MAINOF_CHECK_WINDOW_H);
        xw_node_def_add( XW_MANUL_WHITE_BALANCE_WINDOW_ID ,XW_MANUL_WHITE_BALANCE_WINDOW_X ,XW_MANUL_WHITE_BALANCE_WINDOW_Y,
                XW_MAINOF_CHECK_WINDOW_W, XW_MAINOF_CHECK_WINDOW_H);


        //auto ae
        xw_node_def_add( XW_AUTO_EXPOUSURE_WINDOW_ID,XW_AUTO_EXPOUSURE_WINDOW_X,XW_AUTO_EXPOUSURE_WINDOW_Y,
                XW_MAINOF_CHECK_WINDOW_W, XW_MAINOF_CHECK_WINDOW_H);
        
        //isp reset
       xw_node_def_add( XW_ISP_RESET_WINDOW_ID, XW_ISP_RESET_WINDOW_X,XW_ISP_RESET_WINDOW_Y,
               XW_MAINOF_BUTTON_REST_W, XW_MAINOF_BUTTON_REST_H);


       //date set button of main
       //year
        xw_node_def_add(XW_DATE_YEAR_WINDOW_ID ,XW_DATE_YEAR_WINDOW_X,XW_DATE_YEAR_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        //moon
        xw_node_def_add(XW_DATE_MONN_WINDOW_ID ,XW_DATE_MONN_WINDOW_X ,XW_DATE_MONN_WINDOW_Y ,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
       
        //day
        xw_node_def_add(XW_DATE_DAY_WINDOW_ID ,XW_DATE_DAY_WINDOW_X ,XW_DATE_DAY_WINDOW_Y ,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
       

        //hour
        xw_node_def_add(XW_DATE_HOUR_WINDOW_ID ,XW_DATE_HOUR_WINDOW_X,XW_DATE_HOUR_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        //min
        xw_node_def_add(XW_DATE_MIN_WINDOW_ID ,XW_DATE_MIN_WINDOW_X,XW_DATE_MIN_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        
        //sec
        xw_node_def_add(XW_DATE_SEC_WINDOW_ID,XW_DATE_SEC_WINDOW_X,XW_DATE_SEC_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        
        //set up
        xw_node_def_add(XW_DATE_SET_UP_WINDOW_ID ,XW_DATE_SET_UP_WINDOW_X ,XW_DATE_SET_UP_WINDOW_Y,
                XW_MAINOF_DATE_SET_UP_W, XW_MAINOF_DATE_SET_UP_H);
    
        //set down
        xw_node_def_add(XW_DATE_SET_DOWN_WINDOW_ID ,XW_DATE_SET_DOWN_WINDOW_X ,XW_DATE_SET_DOWN_WINDOW_Y,
                XW_MAINOF_DATE_SET_DOWN_W, XW_MAINOF_DATE_SET_DOWN_H);

        //set all
        xw_node_def_add(XW_DATE_SET_WINDOW_ID ,XW_DATE_SET_WINDOW_X ,XW_DATE_SET_WINDOW_Y,
                XW_MAINOF_DATE_SET_BUTTON_W,   XW_MAINOF_DATE_SET_BUTTON_H);

        //top meun
        //
        xw_node_def_add(XW_TOP_MENU_WINDOW_ID,XW_TOP_MENU_WINDOW_X,XW_TOP_MENU_WINDOW_Y,XW_TOP_MENU_W,XW_TOP_MENU_H);
        //snap
        xw_node_def_add( XW_SNAP_WINDOW_ID   , XW_SNAP_WINDOW_X ,XW_SNAP_WINDOW_Y, 
                XW_TOPOF_BUTTON_W ,XW_TOPOF_BUTTON_H);
        //recod
        xw_node_def_add(XW_RECOD_WINDOW_ID, XW_RECOD_WINDOW_X,XW_RECOD_WINDOW_Y,
                XW_TOPOF_BUTTON_W ,XW_TOPOF_BUTTON_H);
        //previw
        xw_node_def_add(XW_PERVIEW_WINDOW_ID, XW_PERVIEW_WINDOW_X,XW_PERVIEW_WINDOW_Y,
                XW_TOPOF_BUTTON_W ,XW_TOPOF_BUTTON_H);
        //set off/on
        xw_node_def_add(XW_SET_WINDOW_ID, XW_SET_WINDOW_X,XW_SET_WINDOW_Y,
                XW_TOPOF_BUTTON_W ,XW_TOPOF_BUTTON_H);

        //iamge perviwe window def
        //mages any perviwe
        xw_node_def_add(XW_PERVIEW_IMAGE_ANY_WINDOW_ID, XW_PERVIEW_IMAGE_ANY_WINDOW_X,XW_PERVIEW_IMAGE_ANY_WINDOW_Y,
                XW_PERVIEW_WINDOW_ANY_W ,XW_PERVIEW_WINDOW_ANY_H);

        //one big image perviwe
        xw_node_def_add(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID, XW_PERVIEW_IMAGE_ONLY_WINDOW_X,XW_PERVIEW_IMAGE_ONLY_WINDOW_Y,
                XW_PERVIEW_WINDOW_ONLY_W ,XW_PERVIEW_WINDOW_ONLY_H);

        //most images previwe next 
        xw_node_def_add(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID, XW_PERVIEW_IMAGE_ANEXT_WINDOW_X,XW_PERVIEW_IMAGE_ANEXT_WINDOW_Y,
                XW_PERVIEW_WINDOW_ANEXT_W ,XW_PERVIEW_WINDOW_ANEXT_H);
        //pevr
        xw_node_def_add(XW_PERVIEW_IMAGE_APER_WINDOW_ID, XW_PERVIEW_IMAGE_APER_WINDOW_X,XW_PERVIEW_IMAGE_APER_WINDOW_Y,
                XW_PERVIEW_WINDOW_ANEXT_W ,XW_PERVIEW_WINDOW_ANEXT_H);

        //message info put
        //prompt box
        xw_node_def_add(XW_TEXT_PROMPT_BOX_WINDOW_ID ,XW_TEXT_PROMPT_BOX_WINDOW_X , XW_TEXT_PROMPT_BOX_WINDOW_Y,
               XW_TEXT_PROMPT_BOX_FONT_W,  XW_TEXT_PROMPT_BOX_FONT_H);
        //record time count
        xw_node_def_add(XW_TEXT_TIME_CNT_BOX_WINDOW_ID ,XW_TEXT_TIME_CNT_BOX_WINDOW_X ,XW_TEXT_TIME_CNT_BOX_WINDOW_Y,
                XW_TEXT_TIME_CNT_BOX_FONT_W, XW_TEXT_TIME_CNT_BOX_FONT_H);


        //update window about
        xw_node_def_add(XW_UPDATE_WINDOW_ID , XW_UPDATE_WINDOW_X ,XW_UPDATE_WINDOW_Y,0, 0);
        xw_node_def_add(XW_UPDATE_YES_WINDOW_ID , XW_UPDATE_YES_WINDOW_X ,XW_UPDATE_YES_WINDOW_Y,121, 43);
        xw_node_def_add(XW_UPDATE_NO_WINDOW_ID , XW_UPDATE_NO_WINDOW_X ,XW_UPDATE_NO_WINDOW_Y,121, 43);
        xw_node_def_add(XW_UPDATE_BAR_WINDOW_ID , XW_UPDATE_BAR_WINDOW_X ,XW_UPDATE_BAR_WINDOW_Y,300, 24);
        xw_node_def_add(XW_UPDATE_BAR_TEXT_WINDOW_ID , XW_UPDATE_BAR_TEXT_WINDOW_X ,XW_UPDATE_BAR_TEXT_WINDOW_Y,12, 24);
	
	//addy debug version
		
    
    return ;
}

//1024 *600 
static void xw_node_params_add_600P(void)
{
        
        
         xw_node_def_add(XW_MAIN_WINDOW_ID,SXW_MAIN_WINDOW_X,SXW_MAIN_WINDOW_Y,SXW_MAIN_MENU_W,SXW_MAIN_MENU_H);
        
        //line manger of main
        
        //close of open lines
        xw_node_def_add(XW_LINE_RARR_WINDOW_ID ,SXW_LINE_RARR_WINDOW_X,
                SXW_LINE_RARR_WINDOW_Y,SXW_LINE_RARR_WINDOW_W , SXW_LINE_RARR_WINDOW_H) ;
        


        xw_node_def_add(XW_LINE_SHOW_WINDOW_ID ,SXW_LINE_SHOW_WINDOW_X,
                SXW_LINE_SHOW_WINDOW_Y,SXW_MAINOF_BUTTON_W ,SXW_MAINOF_BUTTON_H);
        //selec lines array   
        xw_node_def_add(XW_LINE_CHOICE_WINDOW_ID, SXW_LINE_CHOICE_WINDOW_X,SXW_LINE_CHOICE_WINDOW_Y,SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //set line size
        xw_node_def_add(XW_LINE_SET_SIZE_WINDOW_ID ,SXW_LINE_SET_SIZE_WINDOW_X,
                SXW_LINE_SET_SIZE_WINDOW_Y,SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //set line color
        xw_node_def_add(XW_LINE_SET_COLOR_WINDOW_ID,SXW_LINE_SET_COLOR_WINDOW_X,SXW_LINE_SET_COLOR_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
       
        //select line
        xw_node_def_add(XW_LINE_SELECT_LINE_WINDOW_ID  ,SXW_LINE_SELECT_LINE_WINDOW_X,SXW_LINE_SELECT_LINE_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //line lock or video lock
        xw_node_def_add(XW_LINE_LOCK_WINDOW_ID,SXW_LINE_LOCK_WINDOW_X,SXW_LINE_LOCK_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //line clear
        xw_node_def_add(XW_LINE_CLEAR_WINDOW_ID ,SXW_LINE_CLAER_WINDOW_X,SXW_LINE_CLAER_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);

        
        //line params save
        xw_node_def_add(XW_LINE_SAVE_WINDOW_ID,SXW_LINE_SAVE_WINDOW_X,SXW_LINE_SAVE_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //add video set button of main 
        
        //filp
        xw_node_def_add(XW_VIDEO_FILP_WINDOW_ID,SXW_VIDEO_FILP_WINDOW_X,SXW_VIDEO_FILP_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //mirror
        xw_node_def_add(XW_VIDEO_MIRROR_WINDOW_ID ,SXW_VIDEO_MIRROR_WINDOW_X,SXW_VIDEO_MIRROR_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);

       //night or day
        xw_node_def_add(XW_VIDEO_NIGHGT_WINDOW_ID ,SXW_VIDEO_NIGHGT_WINDOW_X,SXW_VIDEO_NIGHGT_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
    
        //hdr
        xw_node_def_add(XW_VIDEO_HDR_WINDOW_ID,SXW_VIDEO_HDR_WINDOW_X,SXW_VIDEO_HDR_WINDOW_Y,
                SXW_MAINOF_BUTTON_W,SXW_MAINOF_BUTTON_H);
        
        //isp  set button of main  
        
        //color temp
        xw_node_def_add(XW_ISP_COLOR_TEMP_WINDOW_ID ,SXW_ISP_COLOR_TEMP_WINDOW_X,SXW_ISP_COLOR_TEMP_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID  ,SXW_ISP_COLOR_TEMP_TEXT_WINDOW_X,
                SXW_ISP_COLOR_TEMP_TEXT_WINDOW_Y,SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        //wab bule
        xw_node_def_add(XW_ISP_BLUE_WINDOW_ID   ,SXW_ISP_BLUE_WINDOW_X,SXW_ISP_BLUE_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_BLUE_TEXT_WINDOW_ID ,SXW_ISP_BLUE_TEXT_WINDOW_X,SXW_ISP_BLUE_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        //wab red
        xw_node_def_add(XW_ISP_RED_WINDOW_ID,SXW_ISP_RED_WINDOW_X,SXW_ISP_RED_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_RED_TEXT_WINDOW_ID ,SXW_ISP_RED_TEXT_WINDOW_X,SXW_ISP_RED_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        
        // wab green 
        xw_node_def_add(XW_ISP_GREEN_WINDOW_ID   ,SXW_ISP_GREEN_WINDOW_X,SXW_ISP_GREEN_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_GREEN_TEXT_WINDOW_ID ,SXW_ISP_GREEN_TEXT_WINDOW_X,SXW_ISP_GREEN_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        //denoise
        xw_node_def_add(XW_ISP_DENOISE_WINDOW_ID ,SXW_ISP_DENOISE_WINDOW_X,SXW_ISP_DENOISE_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_DENOISE_TEXT_WINDOW_ID ,SXW_ISP_DENOISE_TEXT_WINDOW_X,SXW_ISP_DENOISE_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        
        //sharpness
        xw_node_def_add(XW_ISP_SHARPNESS_WINDOW_ID ,SXW_ISP_SHARPNESS_WINDOW_X,SXW_ISP_SHARPNESS_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SHARPNESS_TEXT_WINDOW_ID  ,SXW_ISP_SHARPNESS_TEXT_WINDOW_X,SXW_ISP_SHARPNESS_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);

        //brightness
        xw_node_def_add(XW_ISP_BRIGHTNESS_WINDOW_ID  ,SXW_ISP_BRIGHTNESS_WINDOW_X,SXW_ISP_BRIGHTNESS_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID  ,SXW_ISP_BRIGHTNESS_TEXT_WINDOW_X,SXW_ISP_BRIGHTNESS_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);

        //staturation
        xw_node_def_add(XW_ISP_SATURATION_WINDOW_ID ,SXW_ISP_SATURATION_WINDOW_X,SXW_ISP_SATURATION_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SATURATION_TEXT_WINDOW_ID ,SXW_ISP_SATURATION_TEXT_WINDOW_X,SXW_ISP_SATURATION_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);

        //contrast
        xw_node_def_add(XW_ISP_CONTRAST_WINDOW_ID ,SXW_ISP_CONTRAST_WINDOW_X,SXW_ISP_CONTRAST_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_CONTRAST_TEXT_WINDOW_ID  ,SXW_ISP_CONTRAST_TEXT_WINDOW_X,SXW_ISP_CONTRAST_TEXT_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
    
        //manu ae
        xw_node_def_add(XW_MANUL_EXPOUSURE_WINDOW_ID   ,SXW_AE_MANUL_WINDOW_X,SXW_AE_MANUL_WINDOW_Y,
               SXW_MAINOF_BAR_LINE_W,SXW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_MANUL_EXPOUSURE_TEXT_WINDOW_ID, SXW_MANUL_EXPOUSURE_TEXT_WINDOW_X ,
                SXW_MANUL_EXPOUSURE_TEXT_WINDOW_Y,SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
    
       //
       //50hz filcker 
       xw_node_def_add(XW_FILCKER_50H_WINDOW_ID ,SXW_FILCKER_50H_WINDOW_X,SXW_FILCKER_50H_WINDOW_Y,
                SXW_MAINOF_CHECK_WINDOW_W, SXW_MAINOF_CHECK_WINDOW_H);
        
       //60hz filcker
       xw_node_def_add(XW_FILCKER_60H_WINDOW_ID ,SXW_FILCKER_60H_WINDOW_X,SXW_FILCKER_60H_WINDOW_Y,
                SXW_MAINOF_CHECK_WINDOW_W, SXW_MAINOF_CHECK_WINDOW_H);
        
        //auto wab
        xw_node_def_add(XW_AUTO_WHITE_BALANCE_WINDOW_ID ,SXW_AUTO_WHITE_BALANCE_WINDOW_X ,SXW_AUTO_WHITE_BALANCE_WINDOW_Y,
                SXW_MAINOF_CHECK_WINDOW_W, SXW_MAINOF_CHECK_WINDOW_H);
        
        xw_node_def_add(XW_MANUL_WHITE_BALANCE_WINDOW_ID ,SXW_MANUL_WHITE_BALANCE_WINDOW_X ,SXW_MANUL_WHITE_BALANCE_WINDOW_Y,
                SXW_MAINOF_CHECK_WINDOW_W, SXW_MAINOF_CHECK_WINDOW_H);

        
        //auto ae
        xw_node_def_add(XW_AUTO_EXPOUSURE_WINDOW_ID,SXW_AUTO_EXPOUSURE_WINDOW_X,SXW_AUTO_EXPOUSURE_WINDOW_Y,
                SXW_MAINOF_CHECK_WINDOW_W, SXW_MAINOF_CHECK_WINDOW_H);
        
        //isp reset
       xw_node_def_add(XW_ISP_RESET_WINDOW_ID, SXW_ISP_RESET_WINDOW_X,SXW_ISP_RESET_WINDOW_Y,
               SXW_MAINOF_BUTTON_REST_W, SXW_MAINOF_BUTTON_REST_H);


       //date set button of main
       //year
        xw_node_def_add(XW_DATE_YEAR_WINDOW_ID ,SXW_DATE_YEAR_WINDOW_X,SXW_DATE_YEAR_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        //moon
        xw_node_def_add(XW_DATE_MONN_WINDOW_ID ,SXW_DATE_MONN_WINDOW_X ,SXW_DATE_MONN_WINDOW_Y ,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
       
        //day
        xw_node_def_add(XW_DATE_DAY_WINDOW_ID ,SXW_DATE_DAY_WINDOW_X ,SXW_DATE_DAY_WINDOW_Y ,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
       

        //hour
        xw_node_def_add(XW_DATE_HOUR_WINDOW_ID ,SXW_DATE_HOUR_WINDOW_X,SXW_DATE_HOUR_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);

        //min
        xw_node_def_add(XW_DATE_MIN_WINDOW_ID ,SXW_DATE_MIN_WINDOW_X,SXW_DATE_MIN_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        
        //sec
        xw_node_def_add(XW_DATE_SEC_WINDOW_ID,SXW_DATE_SEC_WINDOW_X,SXW_DATE_SEC_WINDOW_Y,
                SXW_WINDOW_FONT_SIZE_W,SXW_WINDOW_FONT_SIZE_H);
        
        //set up
        xw_node_def_add(XW_DATE_SET_UP_WINDOW_ID ,SXW_DATE_SET_UP_WINDOW_X ,SXW_DATE_SET_UP_WINDOW_Y,
                SXW_MAINOF_DATE_SET_UP_W, SXW_MAINOF_DATE_SET_UP_H);
    
        //set down
        xw_node_def_add(XW_DATE_SET_DOWN_WINDOW_ID ,SXW_DATE_SET_DOWN_WINDOW_X ,SXW_DATE_SET_DOWN_WINDOW_Y,
                SXW_MAINOF_DATE_SET_DOWN_W, SXW_MAINOF_DATE_SET_DOWN_H);

        //set all
        xw_node_def_add(XW_DATE_SET_WINDOW_ID ,SXW_DATE_SET_WINDOW_X ,SXW_DATE_SET_WINDOW_Y,
                SXW_MAINOF_DATE_SET_BUTTON_W,   SXW_MAINOF_DATE_SET_BUTTON_H);

        
        
        //top-------
        xw_node_def_add(XW_TOP_MENU_WINDOW_ID,SXW_TOP_MENU_WINDOW_X,SXW_TOP_MENU_WINDOW_Y,SXW_TOP_MENU_W,SXW_TOP_MENU_H);
        //snap
        xw_node_def_add(XW_SNAP_WINDOW_ID   , SXW_SNAP_WINDOW_X ,SXW_SNAP_WINDOW_Y, 
                SXW_TOPOF_BUTTON_W ,SXW_TOPOF_BUTTON_H);
        //recod
        xw_node_def_add(XW_RECOD_WINDOW_ID, SXW_RECOD_WINDOW_X,SXW_RECOD_WINDOW_Y,
                SXW_TOPOF_BUTTON_W ,SXW_TOPOF_BUTTON_H);
        //previw
        xw_node_def_add(XW_PERVIEW_WINDOW_ID, SXW_PERVIEW_WINDOW_X,SXW_PERVIEW_WINDOW_Y,
                SXW_TOPOF_BUTTON_W ,SXW_TOPOF_BUTTON_H);
        //set off/on
        xw_node_def_add(XW_SET_WINDOW_ID, SXW_SET_WINDOW_X,SXW_SET_WINDOW_Y,
                SXW_TOPOF_BUTTON_W ,SXW_TOPOF_BUTTON_H);

        //iamge perviwe window def
        //mages any perviwe
        xw_node_def_add(XW_PERVIEW_IMAGE_ANY_WINDOW_ID, SXW_PERVIEW_IMAGE_ANY_WINDOW_X,SXW_PERVIEW_IMAGE_ANY_WINDOW_Y,
                SXW_PERVIEW_WINDOW_ANY_W ,SXW_PERVIEW_WINDOW_ANY_H);

        //one big image perviwe
        xw_node_def_add(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID, SXW_PERVIEW_IMAGE_ONLY_WINDOW_X,SXW_PERVIEW_IMAGE_ONLY_WINDOW_Y,
                SXW_PERVIEW_WINDOW_ONLY_W ,SXW_PERVIEW_WINDOW_ONLY_H);

        //most images previwe next 
        xw_node_def_add(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID, SXW_PERVIEW_IMAGE_ANEXT_WINDOW_X,SXW_PERVIEW_IMAGE_ANEXT_WINDOW_Y,
                SXW_PERVIEW_WINDOW_ANEXT_W ,SXW_PERVIEW_WINDOW_ANEXT_H);
        //pevr
        xw_node_def_add(XW_PERVIEW_IMAGE_APER_WINDOW_ID, SXW_PERVIEW_IMAGE_APER_WINDOW_X,SXW_PERVIEW_IMAGE_APER_WINDOW_Y,
                SXW_PERVIEW_WINDOW_ANEXT_W ,SXW_PERVIEW_WINDOW_ANEXT_H);

        //message info put
        //prompt box
        xw_node_def_add(XW_TEXT_PROMPT_BOX_WINDOW_ID ,SXW_TEXT_PROMPT_BOX_WINDOW_X , SXW_TEXT_PROMPT_BOX_WINDOW_Y,
               SXW_TEXT_PROMPT_BOX_FONT_W,  SXW_TEXT_PROMPT_BOX_FONT_H);
        //record time count
        xw_node_def_add(XW_TEXT_TIME_CNT_BOX_WINDOW_ID ,SXW_TEXT_TIME_CNT_BOX_WINDOW_X ,SXW_TEXT_TIME_CNT_BOX_WINDOW_Y,
                SXW_TEXT_TIME_CNT_BOX_FONT_W, SXW_TEXT_TIME_CNT_BOX_FONT_H);

        //update window about
        xw_node_def_add(XW_UPDATE_WINDOW_ID , SXW_UPDATE_WINDOW_X ,SXW_UPDATE_WINDOW_Y,0, 0);
        xw_node_def_add(XW_UPDATE_YES_WINDOW_ID , SXW_UPDATE_YES_WINDOW_X ,SXW_UPDATE_YES_WINDOW_Y,30, 18);
        xw_node_def_add(XW_UPDATE_NO_WINDOW_ID , SXW_UPDATE_NO_WINDOW_X ,SXW_UPDATE_NO_WINDOW_Y,30, 18);
        xw_node_def_add(XW_UPDATE_BAR_WINDOW_ID , SXW_UPDATE_BAR_WINDOW_X ,SXW_UPDATE_BAR_WINDOW_Y,120, 18);
        xw_node_def_add(XW_UPDATE_BAR_TEXT_WINDOW_ID , SXW_UPDATE_WINDOW_X ,SXW_UPDATE_WINDOW_Y,8, 16);

	
	//deubug version window

        
        return ;
    
}


