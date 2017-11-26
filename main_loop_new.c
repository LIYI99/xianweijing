#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gk_device_init.h"
#include "gk_image_sdk_new.h"


static  void  mouse_leave_botton_func_v2(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xFF80;
}

static  void  mouse_leave_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xF00F;
}

static  void  mouse_offset_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xBBCA;
}

static  void  mouse_rdown_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    bt->color = 0xFE41;
}

static  void  mouse_rup_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    bt->color = 0xFBE4;
}

static  void  mouse_lup_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    bt->color = 0x7497;
}

static  void  mouse_lup_botton_func_v2(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);
    bt->color = 0x7497;
}


static  void  mouse_ldown_botton_func(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);                 
    bt->color = 0xFD79;
}


static  void  mouse_ldown_botton_func_v2(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t *)(data);                 
    bt->color = 0xFD79;
   }


static  void  mouse_lup_line_func_v2(void *data)
{
    
    window_node_line_t *bt  =  (window_node_line_t *)(data);
    bt->color = 0x7497;
   }


static  void  mouse_ldown_line_func_v2(void *data)
{
    
    //printf("data:%p\n",data);
    window_node_line_t *bt  =  (window_node_line_t *)(data);            
    bt->color = 0xFD79;
}


// chen hua memu


static  void  mouse_offset_func_bian_meun(void *data){

    window_node_menu_t *window = (window_node_menu_t *)data;

    if(window->x != 1500){
        window->this_node->freshen_arrt = NEED_FRESHEN;
        window->x = 1500;
        window->y = 100;
        window->h = 800;
        window->w = 400;
        window->this_node->en_submenu = 1;
    }
    
    
    return;
}

  
static  void  mouse_leave_func_bian_meun(void *data){

    printf("menu set claer \n"); 
    window_node_menu_t *window = (window_node_menu_t *)data;
    window->x = 1880;
    window->y = 100;
    window->h = 800;
    window->w = 40;
    window->this_node->freshen_arrt = NEED_CLEAR;
    window->this_node->en_submenu = 0;
    
    return;
}
  




int main(int argc,char **argv)
{
    /*
     *  root  = 'A'
     *  other = 'A.....'
     *
     * */
    
    int ret = 0;
    ret = gk_device_init(NULL); //ok
    start_read_venc_thread(); //ok 
    
    Image_SDK_Init();
    
    window_node_menu_t    mt;
    struct user_set_node_atrr  node_attr;
    memset(&mt,0x0,sizeof(window_node_menu_t));
    
    memset(&node_attr,0x0,sizeof(struct user_set_node_atrr));
    
    memcpy(node_attr.node_id,"Aa",2);
    
    node_attr.en_node = 1;
    node_attr.en_freshen = 0;
    node_attr.move_arrt = 0;
    mt.x = 1880;
    mt.y = 100;
    mt.w = 100;
    mt.h = 800;
    mt.color = 0xf00f;
    mt.video_set.mouse_offset =  mouse_offset_func_bian_meun;
    mt.video_set.mouse_leave =   mouse_leave_func_bian_meun;
    mt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    mt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Menu( node_attr,mt);

    //button
    window_node_button_t    bt;
    memset(&bt,0x0,sizeof(window_node_button_t));
    
    memcpy(node_attr.node_id,"Aaa",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    bt.x = 1520;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xFE41;
    bt.video_set.mouse_offset =  NULL;
    bt.video_set.mouse_leave =   NULL;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Button( node_attr,bt);
    
    //
    memcpy(node_attr.node_id,"Aab",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    bt.x = 1600;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xFE41;
    bt.video_set.mouse_offset =  NULL;
    bt.video_set.mouse_leave =   NULL;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Button( node_attr,bt);
    
    memcpy(node_attr.node_id,"Aac",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    bt.x = 1700;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xFE41;
    bt.video_set.mouse_offset =  NULL;
    bt.video_set.mouse_leave =   NULL;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Button( node_attr,bt);
    

    
    //memcpy(node_attr.node_id,"Ab",2);

    
    
    Image_SDK_Run();

    //
    Image_SDK_deInit();



    return 0;
}
