#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xw_main_line_manger_show.h"
#include "xw_line_show.h"
#include "gk_image_sdk_new.h"
#include "xw_png_load.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"

struct  xw_line_ui_set{
        uint8_t     order;
        uint16_t    *png_p;
        uint16_t    h;
        uint16_t    w;
};


static  struct xw_line_ui_set   *arry_set = NULL,*size_set = NULL,*color_set = NULL;
static  uint16_t     arry_now = 0 , size_now = 0, color_now = 0,line_show_close = 0;

#define     ARRY_SET_NUMS       8
#define     SIZE_SET_NUMS       4
#define     COLOR_SET_NUMS      6



static void xw_line_show_button_ldow(void *data)
{
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = 0xf00f;
    Image_SDK_Set_Node_En_Freshen(XW_LINE_SHOW_WINDOW_ID,NEED_FRESHEN);
    xw_lines_cl_op_all(NULL);
    return;
    
}

static void xw_line_show_button_lup(void *data)
{
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = 0xfeee;
    Image_SDK_Set_Node_En_Freshen(XW_LINE_SHOW_WINDOW_ID,NEED_FRESHEN);
    return;

    //   xw_lines_cl_op_all(NULL);
    
}

static void xw_line_setarry_button_ldow(void *data)
{
    
    int ret = 0;
    arry_now ++;
    arry_now =  arry_now%8;
    Image_SDK_Set_Node_En_Freshen(XW_LINE_RARR_WINDOW_ID,NEED_CLEAR);
    usleep(100000);
    ret = xw_lines_arry_set_order(arry_now);
    if(ret != 0)
    {
        printf("fail \n");
        arry_now --;
        return;
    }
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->image_cache = (char *)arry_set[arry_now].png_p;
    Image_SDK_Set_Node_En_Freshen(XW_LINE_CHOICE_WINDOW_ID,NEED_FRESHEN);
    
    return;
}

static void usr_push_video_button_line(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
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




int  xw_main_line_manger_show(void *data)
{

    
    if(arry_set != NULL)
        return -1;
   
    //init png ui data
    arry_set =(struct xw_line_ui_set *) malloc(sizeof(struct xw_line_ui_set) * (ARRY_SET_NUMS +
                SIZE_SET_NUMS + COLOR_SET_NUMS));
    if(arry_set == NULL)
        return -2;
    memset(arry_set,0x0,((sizeof(struct xw_line_ui_set))*(ARRY_SET_NUMS + SIZE_SET_NUMS + COLOR_SET_NUMS)));
    size_set    = arry_set +  ARRY_SET_NUMS;
    color_set   = arry_set +  ARRY_SET_NUMS + SIZE_SET_NUMS;

    int i  = 0;

     char idbuf[10];

    for(i = 0 ;i < ARRY_SET_NUMS ; i++ ){
        arry_set[i].order = i+1;
        sprintf(idbuf,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,i+1);
        arry_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&arry_set[i].w,&arry_set[i].h);

    }

    for(i = 0 ;i < SIZE_SET_NUMS ; i++ ){
        size_set[i].order = i+1;
        sprintf(idbuf,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,i+1);
        size_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&size_set[i].w,&size_set[i].h);

    }

    for(i = 0 ;i < COLOR_SET_NUMS ; i++ ){
        color_set[i].order = i+1;
        sprintf(idbuf,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,i+1);
        color_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&color_set[i].w,&color_set[i].h);

    }

    
    //create button  show
    struct user_set_node_atrr  node_attr;
    memset(&node_attr,0x0,sizeof(struct user_set_node_atrr));
    node_attr.en_node       = 1;
    node_attr.en_freshen    = 0;
    node_attr.move_arrt     = 0;
    window_node_button_t    _button;
    memset(&_button,0x0,sizeof(window_node_button_t));
    _button.x     =  XW_LINE_SHOW_WINDOW_X ;
    _button.y     =  XW_LINE_SHOW_WINDOW_Y ;
    _button.color =  0xfeee;
    _button.w     = 45;
    _button.h     = 45;
    _button.size  =  2;
    _button.video_set.mouse_left_down = xw_line_show_button_ldow;
    _button.video_set.mouse_left_up   = xw_line_show_button_lup;

    _button.user_video_freshen =  usr_push_video_button_line;

    memcpy(node_attr.node_id,XW_LINE_SHOW_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
   

    //create set arry 
    _button.x  =     XW_LINE_CHOICE_WINDOW_X ;
    _button.y     =  XW_LINE_CHOICE_WINDOW_Y ;
    _button.color =  0xfeee;
    _button.size  =  1;
    _button.image_cache = (char *)arry_set[arry_now].png_p;
    _button.w     = arry_set[arry_now].w;
    _button.h     = arry_set[arry_now].h;
    _button.video_set.mouse_left_down   =   xw_line_setarry_button_ldow;
 
    _button.video_set.mouse_left_up     = NULL;
    _button.user_video_freshen          = NULL;// usr_push_video_button_line;

    memcpy(node_attr.node_id,XW_LINE_CHOICE_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
   
    return;
    // set size
     _button.x    =  XW_LINE_SET_SIZE_WINDOW_X ;
    _button.y     =  XW_LINE_SET_SIZE_WINDOW_Y ;
    _button.color =  0xfeee;
    _button.size  =  2;
    _button.image_cache = (char *)size_set[size_now].png_p;
    _button.w     = size_set[size_now].w;
    _button.h     = size_set[size_now].h;
    _button.video_set.mouse_left_down =  NULL;
    _button.video_set.mouse_left_up = NULL;
    _button.user_video_freshen =  NULL;

    memcpy(node_attr.node_id,XW_LINE_SET_SIZE_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    return ;

    // color size
     _button.x    =  XW_LINE_SET_COLOR_WINDOW_X ;
    _button.y     =  XW_LINE_SET_COLOR_WINDOW_Y ;
    _button.color =  0xfeee;
    _button.size  =  2;
    _button.image_cache = (char *)color_set[color_now].png_p;
    _button.w     = color_set[color_now].w;
    _button.h     = color_set[color_now].h;
    _button.video_set.mouse_left_down = NULL;
    
    memcpy(node_attr.node_id,XW_LINE_SET_COLOR_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
     
#if 0    
    memcpy(node_attr.node_id,XW_LINE_SHOW_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    
    memcpy(node_attr.node_id,XW_LINE_SHOW_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
#endif


}

