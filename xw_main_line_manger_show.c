#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "xw_main_line_manger_show.h"
#include "xw_line_show.h"
#include "image_sdk_core.h"
#include "xw_png_load.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_logsrv.h"
#include "xw_config.h"
#include "xw_preview_show.h"
#include "xw_window_def_func.h"
#include "xw_msg_prv.h"

struct  xw_line_ui_set{
        uint8_t     order;
        uint16_t    *png_p;
        uint16_t    h;
        uint16_t    w;
};


static  struct xw_line_ui_set   *arry_set = NULL,*size_set = NULL,*color_set = NULL,*select_set = NULL;
static  uint16_t     arry_now = 0 , size_now = 0, color_now = 0,select_now = 0;
static  uint16_t     colors[6] = {0xa97d,0xc4e4,0x97ad,0xe452,0xd282,0xa716};


#define     ARRY_SET_NUMS       8
#define     SIZE_SET_NUMS       4
#define     COLOR_SET_NUMS      6
#define     SELECT_SET_NUMS     16




static void xw_line_show_button_ldow(void *data)
{
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = CON_BUTTON_LDOWN_COLOR; 
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    //need close prewiv
    xw_preview_close();
    xw_lines_cl_op_all(NULL);
    return;
    
}


#define     XW_LINE_MANGER_BUTTON_OFFSET_COLOR     CON_BUTTON_OFFSET_COLOR 
#define     XW_LINE_MANGER_BUTTON_LDOWN_COLOR     CON_BUTTON_LDOWN_COLOR  
#define     XW_LINE_MANGER_BUTTON_LEAVE_COLOR       CON_BUTTON_LEAVE_COLOR 

#define     XW_LINE_MANGER_BUTTON_SIZE              2



static void xw_line_show_button_lup(void *data)
{
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;


     return;

       
}

static void xw_line_show_button_offset(void *data)
{
    
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color =  XW_LINE_MANGER_BUTTON_OFFSET_COLOR;
     bt->this_node->freshen_arrt = NEED_FRESHEN;


      return;

       
}

static void xw_line_show_button_leave(void *data)
{
    
      
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

  
    return;
}




static void xw_line_setarry_button_ldow(void *data)
{
    
    int ret = 0;
    arry_now ++;
    arry_now =  arry_now % ARRY_SET_NUMS;
    
    ret = xw_lines_arry_set_order(arry_now);
    if(ret != 0)
    {
        xw_logsrv_err("set line arry fail,maybe lock or not open \n");
        arry_now --;
        return;
    }
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->image_cache = (char *)arry_set[arry_now].png_p;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

      
    return;
}


static void xw_line_setsize_button_ldow(void *data)
{
    
    int ret = 0;
    size_now ++;
    size_now =  size_now % SIZE_SET_NUMS;
    
    ret = xw_lines_line_set_param(0, colors[color_now],size_now + 1);
    if(ret < 0){


        if(size_now != 0)
            size_now --;
        return ;

    }

    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->image_cache = (char *)size_set[size_now].png_p;
    bt->this_node->freshen_arrt = NEED_FRESHEN;


    
    return;
}


static void xw_line_setcolor_button_ldow(void *data)
{
    
    int ret = 0;
    color_now ++;
    color_now =  color_now % COLOR_SET_NUMS;
    
    ret = xw_lines_line_set_param(0, colors[color_now],size_now + 1);
    if(ret < 0){
        if(color_now != 0)
            color_now--;
        return ;
    }


    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->image_cache = (char *)color_set[color_now].png_p;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
   

    
    return;
}

static void xw_line_select_line_button_ldow(void *data)
{
    
    int ret = 0;
    select_now ++;
    select_now =  select_now % SELECT_SET_NUMS;
    ret = xw_lines_set_select(select_now);
    if(ret != 0){
        xw_logsrv_err("select set fail revale%d\n",ret);
        return ;
    }
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->image_cache = (char *)select_set[select_now].png_p;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

     return;
}


static void xw_line_lock_button_ldow(void *data)
{
    
    int ret = 0,value = 1;
    
    	
    ret  = Image_Msg_Send(IDSCAM_IMG_MSG_FREEZE	,(void *)(&value),4);

    //ret = xw_lines_set_lock(0);
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = XW_LINE_MANGER_BUTTON_LDOWN_COLOR ;
    bt->this_node->freshen_arrt = NEED_FRESHEN;


    return;
}

static void xw_line_clear_button_ldow(void *data)
{
    
    int ret = 0;
    
    ret  = xw_lines_cl_op_line(NULL);
    if(ret < 0)
        return ;

    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = XW_LINE_MANGER_BUTTON_LDOWN_COLOR ;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

    return;
}



static void xw_line_save_button_ldow(void *data)
{
    
    int ret = 0;
    window_node_button_t *bt = (window_node_button_t *)data; 
    bt->color = XW_LINE_MANGER_BUTTON_LDOWN_COLOR ;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    ret =  xw_save_line_data(LINE_SET_PARAMS_FILE_PATH);


    return;
}







static void usr_push_video_button_line(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
{

    window_node_button_t *bt =  (window_node_button_t *)data;
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
                SIZE_SET_NUMS + COLOR_SET_NUMS +  SELECT_SET_NUMS ));
    if(arry_set == NULL)
        return -2;
    
    memset(arry_set,0x0,((sizeof(struct xw_line_ui_set))*(ARRY_SET_NUMS + SIZE_SET_NUMS + 
                    COLOR_SET_NUMS +  SELECT_SET_NUMS )));

    size_set    = arry_set +  ARRY_SET_NUMS;
    color_set   = arry_set +  ARRY_SET_NUMS + SIZE_SET_NUMS;
    select_set  = arry_set +  ARRY_SET_NUMS + SIZE_SET_NUMS + COLOR_SET_NUMS;



    int i  = 0;

     char idbuf[10];

    for(i = 0 ;i < ARRY_SET_NUMS ; i++ ){
        arry_set[i].order = i+1;
        sprintf(idbuf,"%s_%d",XW_LINE_CHOICE_WINDOW_ID,i+1);
        arry_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&arry_set[i].w,&arry_set[i].h);
        xw_logsrv_debug("arry_set[%d].png_p:%p,arry_set[i].w:%d arry_set[i].h:%d png id:%s\n",i,
                arry_set[i].png_p,arry_set[i].w,arry_set[i].h,idbuf);
    }

    for(i = 0 ;i < SIZE_SET_NUMS ; i++ ){
        size_set[i].order = i+1;
        sprintf(idbuf,"%s_%d",XW_LINE_SET_SIZE_WINDOW_ID,i+1);
        size_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&size_set[i].w,&size_set[i].h);

    }
    //set color
    for(i = 0 ;i < COLOR_SET_NUMS ; i++ ){
        color_set[i].order = i+1;
        sprintf(idbuf,"%s_%d",XW_LINE_SET_COLOR_WINDOW_ID,i+1);
        color_set[i].png_p = xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&color_set[i].w,&color_set[i].h);
        
      
    }
    //select line
    for(i = 0 ; i < SELECT_SET_NUMS/2; i++){
        select_set[i].order = i+1;
        sprintf(idbuf,"%s_H%d",XW_LINE_SELECT_LINE_WINDOW_ID,i+1);
        select_set[i].png_p =  xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&select_set[i].w,&select_set[i].h);
        xw_logsrv_debug("select_set[%d].png_p:%p,select_set[i].w:%d select_set[i].h:%d png id:%s\n",i,
                select_set[i].png_p,select_set[i].w,select_set[i].h,idbuf);


    }
    for( ; i < SELECT_SET_NUMS; i++){
        select_set[i].order = i+1;
        sprintf(idbuf,"%s_V%d",XW_LINE_SELECT_LINE_WINDOW_ID,i - 7);
        select_set[i].png_p =  xw_get_window_png(idbuf);
        xw_get_png_hw(idbuf,&select_set[i].w,&select_set[i].h);
        xw_logsrv_debug("select_set[%d].png_p:%p,select_set[i].w:%d select_set[i].h:%d png id:%s\n",i,
                select_set[i].png_p,select_set[i].w,select_set[i].h,idbuf);
    }


    
    NULL;
//create button  show
    int ret  = 0;
    struct user_set_node_atrr  node_attr;
    memset(&node_attr,0x0,sizeof(struct user_set_node_atrr));
    node_attr.en_node       = 1;
    node_attr.en_freshen    = 0;
    node_attr.move_arrt     = 0;
    window_node_button_t    _button;
    memset(&_button,0x0,sizeof(window_node_button_t));

    ret = xw_get_node_window_param(XW_LINE_SHOW_WINDOW_ID,&_button.x,&_button.y,&_button.w,&_button.h);
    if(ret <  0){
        xw_logsrv_err("window:%s get x,yx,w,h fail\n",XW_LINE_SHOW_WINDOW_ID);
    }
    _button.color = XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  = XW_LINE_MANGER_BUTTON_SIZE;
    _button.video_set.mouse_left_down   = xw_line_show_button_ldow;
    _button.video_set.mouse_leave     =    xw_line_show_button_leave;
    _button.video_set.mouse_offset      = xw_line_show_button_offset;
    _button.user_video_freshen          = usr_push_video_button_line;
    memcpy(node_attr.node_id,XW_LINE_SHOW_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    //lock
    
    ret = xw_get_node_window_param(XW_LINE_LOCK_WINDOW_ID,&_button.x,&_button.y,&_button.w,&_button.h);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,w,h fail \n",XW_LINE_LOCK_WINDOW_ID);
    }
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  = XW_LINE_MANGER_BUTTON_SIZE;
    _button.image_cache = NULL;
    _button.video_set.mouse_left_down   = xw_line_lock_button_ldow;   //now change the fucntion is freezeing 
    _button.video_set.mouse_leave       = xw_line_show_button_leave; 
    _button.video_set.mouse_offset      = xw_line_show_button_offset;
    _button.user_video_freshen          = usr_push_video_button_line;
    memcpy(node_attr.node_id,XW_LINE_LOCK_WINDOW_ID,strlen(XW_LINE_LOCK_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);


    //create set arry 
    ret = xw_get_node_window_param(XW_LINE_CHOICE_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_CHOICE_WINDOW_ID);
    }

    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =  1;
    _button.image_cache = (char *)arry_set[arry_now].png_p;
    _button.w     = arry_set[arry_now].w;
    _button.h     = arry_set[arry_now].h;
    _button.video_set.mouse_left_down   =   xw_line_setarry_button_ldow;
    _button.video_set.mouse_offset      =   NULL; 
    _button.video_set.mouse_left_up     =   NULL;
    _button.video_set.mouse_leave       =   NULL;
    _button.user_video_freshen          =   NULL;// usr_push_video_button_line;
    
    memcpy(node_attr.node_id,XW_LINE_CHOICE_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(node_attr,_button);
   

    // set size
    ret = xw_get_node_window_param(XW_LINE_SET_SIZE_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_SET_SIZE_WINDOW_ID);
    }
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =  1;
    _button.image_cache = (char *)size_set[size_now].png_p;
    _button.w     = size_set[size_now].w;
    _button.h     = size_set[size_now].h;
    _button.video_set.mouse_left_down   =  xw_line_setsize_button_ldow;
    _button.video_set.mouse_left_up     =   NULL;
    _button.user_video_freshen          =   NULL;
    memcpy(node_attr.node_id,XW_LINE_SET_SIZE_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    

    // color set
    ret = xw_get_node_window_param(XW_LINE_SET_COLOR_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_SET_COLOR_WINDOW_ID);
    }
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =  1;
    _button.image_cache = (char *)color_set[color_now].png_p;
    _button.w     = color_set[color_now].w;
    _button.h     = color_set[color_now].h;
    _button.video_set.mouse_left_down = xw_line_setcolor_button_ldow;
    memcpy(node_attr.node_id,XW_LINE_SET_COLOR_WINDOW_ID,strlen(XW_LINE_SHOW_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    //return 0; 
   //select line
    ret = xw_get_node_window_param(XW_LINE_SELECT_LINE_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_SELECT_LINE_WINDOW_ID);
    }
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =  1;
    _button.image_cache = (char *)select_set[select_now].png_p;
    _button.w     = select_set[select_now].w;
    _button.h     = select_set[select_now].h;

    _button.video_set.mouse_left_down = xw_line_select_line_button_ldow;
    memcpy(node_attr.node_id,XW_LINE_SELECT_LINE_WINDOW_ID,strlen(XW_LINE_SELECT_LINE_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
   //clean line
    
    ret = xw_get_node_window_param(XW_LINE_CLEAR_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_CLEAR_WINDOW_ID);
    }
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =   XW_LINE_MANGER_BUTTON_SIZE;
    _button.image_cache = NULL;
    _button.video_set.mouse_left_down = xw_line_clear_button_ldow;
    _button.video_set.mouse_leave       = xw_line_show_button_leave; 
    _button.video_set.mouse_offset      = xw_line_show_button_offset;

    _button.user_video_freshen = usr_push_video_button_line;

    memcpy(node_attr.node_id,XW_LINE_CLEAR_WINDOW_ID,strlen(XW_LINE_CLEAR_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    
    //lines param save
       
    ret = xw_get_node_window_param(XW_LINE_SAVE_WINDOW_ID,&_button.x,&_button.y,NULL,NULL);
    if(ret < 0){
        xw_logsrv_err("window:%s get x,y,fail \n",XW_LINE_SAVE_WINDOW_ID);
    }
 
    _button.color =  XW_LINE_MANGER_BUTTON_LEAVE_COLOR;
    _button.size  =  XW_LINE_MANGER_BUTTON_SIZE;
    _button.image_cache = NULL;
    _button.video_set.mouse_left_down =  xw_line_save_button_ldow;
    memcpy(node_attr.node_id,XW_LINE_SAVE_WINDOW_ID, strlen(XW_LINE_SAVE_WINDOW_ID));
    Image_SDK_Create_Button(  node_attr,_button);
    

    return 0;

}
int xw_main_line_quit_show(void *data)
{
  if(arry_set) 
    free(arry_set );

    return 0;

}
