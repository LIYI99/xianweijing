
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_window_wh_df.h"
#include "xw_config_param_process.h"
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
    int                 cnt;
    struct  node_def_params node_params[NODE_SAVE_MAX_NUMS];
}node_def_hanlde_t;

node_def_hanlde_t   *def_params = NULL;



static void inline  xw_node_def_add(char *node_id,uint16_t x,uint16_t y,uint16_t w,uint16_t h)
{
    
    strcpy(def_params->node_params[def_params->cnt].node_id,node_id);
    def_params->node_params[def_params->cnt].x = x;
    def_params->node_params[def_params->cnt].y = y;
    def_params->node_params[def_params->cnt].w = w;
    def_params->node_params[def_params->cnt].h = h;
    def_params->cnt++;
    return ;

}

int xw_config_def_params_init(srcee_mode_type mode)
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
    
    if(def_params->mode == SRCEE_MODE_1080)
    {
        //main mune        
        xw_node_def_add(XW_MAIN_WINDOW_ID,XW_MAIN_WINDOW_X,XW_MAIN_NUME_H,XW_MAIN_MENU_W);
        
        //line manger
        xw_node_def_add(XW_LINE_SHOW_WINDOW_ID ,XW_LINE_SHOW_WINDOW_X,
                XW_LINE_SHOW_WINDOW_Y,XW_MAINOF_BUTTON_W ,XW_MAINOF_BUTTON_H);
        
        xw_node_def_add(XW_LINE_CHOICE_WINDOW_ID, XW_LINE_CHOICE_WINDOW_X,XW_LINE_CHOICE_WINDOW_Y,XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        xw_node_def_add(XW_LINE_SET_SIZE_WINDOW_ID ,XW_LINE_SET_SIZE_WINDOW_X,
                XW_LINE_SET_SIZE_WINDOW_Y,XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        xw_node_def_add( XW_LINE_SET_COLOR_WINDOW_ID,XW_LINE_SET_COLOR_WINDOW_X,XW_LINE_SET_COLOR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

        xw_node_def_add( XW_LINE_SELECT_LINE_WINDOW_ID  ,XW_LINE_SELECT_LINE_WINDOW_X,XW_LINE_SELECT_LINE_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        xw_node_def_add(XW_LINE_LOCK_WINDOW_ID,XW_LINE_LOCK_WINDOW_X,XW_LINE_LOCK_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

        xw_node_def_add(XW_LINE_CLEAR_WINDOW_ID ,XW_LINE_CLAER_WINDOW_X,XW_LINE_CLAER_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

        xw_node_def_add(XW_LINE_SAVE_WINDOW_ID,XW_LINE_SAVE_WINDOW_X,XW_LINE_SAVE_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //video advanced settings
        
        xw_node_def_add(XW_VIDEO_FILP_WINDOW_ID,XW_VIDEO_FILP_WINDOW_X,XW_VIDEO_FILP_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        xw_node_def_add(XW_VIDEO_MIRROR_WINDOW_ID ,XW_VIDEO_MIRROR_WINDOW_X,XW_VIDEO_MIRROR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);

       
        xw_node_def_add(XW_VIDEO_NIGHGT_WINDOW_ID ,XW_VIDEO_NIGHGT_WINDOW_X,XW_VIDEO_NIGHGT_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
 
        xw_node_def_add(XW_VIDEO_HDR_WINDOW_ID,XW_VIDEO_HDR_WINDOW_X,XW_VIDEO_HDR_WINDOW_Y,
                XW_MAINOF_BUTTON_W,XW_MAINOF_BUTTON_H);
        
        //ISP 
        xw_node_def_add(XW_ISP_COLOR_TEMP_WINDOW_ID ,XW_ISP_COLOR_TEMP_WINDOW_X,XW_ISP_COLOR_TEMP_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_COLOR_TEMP_TEXT_WINDOW_ID  ,XW_ISP_COLOR_TEMP_TEXT_WINDOW_X,
                XW_ISP_COLOR_TEMP_TEXT_WINDOW_Y,XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        xw_node_def_add(XW_ISP_BLUE_WINDOW_ID   ,XW_ISP_BLUE_WINDOW_X,XW_ISP_BLUE_WINDOW_Y,
                SXW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_BLUE_TEXT_WINDOW_ID ,XW_ISP_BLUE_TEXT_WINDOW_X,XW_ISP_BLUE_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        xw_node_def_add(XW_ISP_RED_WINDOW_ID,XW_ISP_RED_WINDOW_X,XW_ISP_RED_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_RED_TEXT_WINDOW_ID ,XW_ISP_RED_TEXT_WINDOW_X,XW_ISP_RED_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        xw_node_def_add( XW_ISP_GREEN_WINDOW_ID   ,XW_ISP_GREEN_WINDOW_X,XW_ISP_GREEN_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_GREEN_TEXT_WINDOW_ID ,XW_ISP_GREEN_TEXT_WINDOW_X,XW_ISP_GREEN_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        xw_node_def_add(XW_ISP_DENOISE_WINDOW_ID ,XW_ISP_DENOISE_WINDOW_X,XW_ISP_DENOISE_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_DENOISE_TEXT_WINDOW_ID ,XW_ISP_DENOISE_TEXT_WINDOW_X,XW_ISP_DENOISE_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
        

        ///
        xw_node_def_add(XW_ISP_SHARPNESS_WINDOW_ID ,XW_ISP_SHARPNESS_WINDOW_X,XW_ISP_SHARPNESS_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SHARPNESS_TEXT_WINDOW_ID  ,XW_ISP_SHARPNESS_TEXT_TEXT_WINDOW_X,XW_ISP_SHARPNESS_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

        
        xw_node_def_add(XW_ISP_BRIGHTNESS_WINDOW_ID  ,XW_ISP_BRIGHTNESS_WINDOW_X,XW_ISP_BRIGHTNESS_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add( XW_ISP_BRIGHTNESS_TEXT_WINDOW_ID  ,XW_ISP_BRIGHTNESS_TEXT_WINDOW_X,XW_ISP_BRIGHTNESS_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);


        xw_node_def_add(XW_ISP_SATURATION_WINDOW_ID ,XW_ISP_SATURATION_WINDOW_X,XW_ISP_SATURATION_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_SATURATION_TEXT_WINDOW_ID ,XW_ISP_SATURATION_TEXT_WINDOW_X,XW_ISP_SATURATION_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);

   
        xw_node_def_add(XW_ISP_CONTRAST_WINDOW_ID ,XW_ISP_CONTRAST_WINDOW_X,XW_ISP_CONTRAST_WINDOW_Y,
                XW_MAINOF_BAR_LINE_W,XW_MAINOF_BAR_LINE_H);

        xw_node_def_add(XW_ISP_CONTRAST_TEXT_WINDOW_ID  ,XW_ISP_CONTRAST_TEXT_WINDOW_X,XW_ISP_CONTRAST_TEXT_WINDOW_Y,
                XW_WINDOW_FONT_SIZE_W,XW_WINDOW_FONT_SIZE_H);
    




    }
    else if(def_params->mode == SRCEE_MODE_600)
    {
    
    }else{
    
        xw_logsrv_err("the srcee  mode not def \n");
    }



    return 0;
    
}

int xw_get_node_param(char * window_id,uint16_t *x,uint16_t *y,uint16_t *w,uint16_t *h)
{
    if(window_id == NULL)
        return -1;
    
    if(strlen(window_id) > sizeof(long))
    {
        xw_logsrv_err("the node id is long:%s \n",window_id);
        return -2 ;
    }
    long int id  = 0;
    memcpy(&id,window_id,strlen(window_id));
    return 0;    

}

int xw_get_main_node_param(char * window_id,uint16_t *x,uint16_t *y,uint16_t *w,uint16_t *h)
{

}
int xw_get_line_wh(uint16_t *w,uint16_t *h)
{

}
int xw_get_top_button_linesize(void)
{

}
int xw_get_main_button_linesize(void)
{

}


