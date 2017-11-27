#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "gk_device_init.h"
#include "gk_image_sdk_new.h"


static  void  mouse_leave_botton_func_v2(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xFF80;
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

    if(window->x != 1600){
        window->this_node->freshen_arrt = NEED_FRESHEN;
        window->x = 1600;
        window->y = 100;
        window->h = 800;
        window->w = 300;
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

// record snap button 
static  void  mouse_leave_button_func_v1(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xF00F;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
}

static  void  mouse_offset_button_func_v1(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xBBCA;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

}

static void usr_push_video_button(void *data ,uint16_t *fbbuf,int scree_w ,int scree_h)
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

//bar func 

static  void  mouse_check_bar_func_v1(void *data)
{
    
    window_node_bar_t *bt  =  (window_node_bar_t*)(data);
    
    if(bt->this_node->mouse_data.x < bt->x)
        return;
   
    printf(" check bar value \n");

    bt->now_value = (bt->this_node->mouse_data.x - bt->x)*bt->max_value/bt->w;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    

    //text window
    char text[5];
    sprintf( text,"%d",bt->now_value);
    Image_SDK_Set_Text_Node_Text(bt->text_id, text,2);

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

    // three button
    window_node_button_t    bt;
    memset(&bt,0x0,sizeof(window_node_button_t));
    
    memcpy(node_attr.node_id,"Aaa",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    
    bt.size =  2;
    bt.x = 1620;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xf00f;
    bt.video_set.mouse_offset =  mouse_offset_button_func_v1;
    bt.video_set.mouse_leave =    mouse_leave_button_func_v1;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    bt.user_video_freshen = usr_push_video_button;

    ret = Image_SDK_Create_Button( node_attr,bt);
    
    //
    memcpy(node_attr.node_id,"Aab",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    bt.x = 1725;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xf00f;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Button( node_attr,bt);
    
    memcpy(node_attr.node_id,"Aac",3);
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    bt.x = 1830;
    bt.y = 120;
    bt.w = 50;
    bt.h = 50;
    bt.color = 0xf00f;
    bt.video_set.mouse_left_down = NULL;//mouse_ldown_botton_func;
    bt.video_set.mouse_left_up = NULL;//mouse_lup_botton_func;
    ret = Image_SDK_Create_Button( node_attr,bt);
   
  


    
    //test bar
    memcpy(node_attr.node_id,"Aad",3);

    window_node_bar_t   _bar;
    memset(&_bar,0x0 ,sizeof(window_node_bar_t));
    _bar.x = 1620;
    _bar.y = 800;
    _bar.w = 200;
    _bar.h = 5;
    _bar.bar_color = 0x3A39; 
    _bar.max_value = 100;
    _bar.now_value = 30;
    _bar.video_set.mouse_left_down = mouse_check_bar_func_v1;
    ret = Image_SDK_Create_Bar(node_attr,_bar);
    memcpy(_bar.text_id,"Aae",3);

    // test add text put
    memcpy(node_attr.node_id,"Aae",3);

    window_node_text_t _text;
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1830;
    _text.y = 800;
    _text.lens = 2;
    _text.win_color = 0xf00f;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    ret = Image_SDK_Set_Text_Node_Text("Aae", "30",2);


    Image_SDK_Run();

    //
    Image_SDK_deInit();



    return 0;
}
