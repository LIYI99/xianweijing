#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

#include "gk_device_init.h"
#include "gk_image_sdk_new.h"
#include "xw_png_load.h"
#include "xw_main_menu_show.h"
#include "xw_top_menu_show.h"

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

    if(window->x != 1510){
        window->this_node->freshen_arrt = NEED_FRESHEN;
        window->x = 1510;
        window->y = 50;
        window->h = 954;
        window->w = 396;
        window->this_node->en_submenu = 1;
        Image_SDK_Set_Node_En_Freshen("Aaf",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aag",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aah",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aaj",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aak",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aam",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aae",NEED_FRESHEN);
        Image_SDK_Set_Node_En_Freshen("Aad",NEED_FRESHEN);


        printf("man menu offset set \n");
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
    bt->color = 0xEEEF;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
}

static  void  mouse_offset_button_func_v1(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xF00F;
    bt->this_node->freshen_arrt = NEED_FRESHEN;

}

static  void  mouse_ldown_button_func_v1(void *data)
{
    
    window_node_button_t *bt  =  (window_node_button_t*)(data);
    bt->color = 0xF000;
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

    bt->now_value = (bt->this_node->mouse_data.x+15 - bt->x)*bt->max_value/bt->w;
    bt->this_node->freshen_arrt = NEED_FRESHEN;
    

    //text window
    char text[5];
    sprintf( text,"%d",bt->now_value);
    printf("text set %s\n",text);
    Image_SDK_Set_Text_Node_Text(bt->text_id, text,2);

    return;


}


static void* time_thread_start(void *data){
    
    
    struct user_set_node_atrr  node_attr;
    int ret = 0;
    int size_w = 8,size_h = 16;
    
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;

    memcpy(node_attr.node_id,"Aaf",3);
    //year
    window_node_text_t _text;
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1625+size_w;
    _text.y = 632+size_h;
    _text.lens = 4;
    _text.win_color = 0xff00;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    //ret = Image_SDK_Set_Text_Node_Text("Aae", "30",2);
    
    //moon
    memcpy(node_attr.node_id,"Aag",3);
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1682+size_w;
    _text.y = 632+size_h;
    _text.lens = 2;
    _text.win_color = 0xf0f0;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //day
    memcpy(node_attr.node_id,"Aah",3);
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1722 + size_w;
    _text.y = 632 + size_h;
    _text.lens = 2;
    _text.win_color = 0xf00f;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    
    //housr
    memcpy(node_attr.node_id,"Aaj",3);
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1768 + size_w;
    _text.y = 632 + size_h;
    _text.lens = 2;
    _text.win_color = 0xf00f;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //minues
    memcpy(node_attr.node_id,"Aak",3);
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1807 +size_w;
    _text.y = 632 +size_h;
    _text.lens = 2;
    _text.win_color = 0xf00f;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);
    //sec
    memcpy(node_attr.node_id,"Aam",3);
    memset(&_text,0x0 ,sizeof(window_node_text_t));
    _text.x = 1846 +size_w;
    _text.y = 632 + size_h;
    _text.lens = 2;
    _text.win_color = 0xf00f;
    _text.text_color = 0x3A39; 
    ret = Image_SDK_Create_Text(node_attr,_text);

    //get time and push
    time_t  tp =0 ;
    
    struct  tm  *time_now,last_time;
    
    memset(&last_time,0x0,sizeof(struct tm));

    char *year = (char *)malloc(128);
    
    last_time.tm_mon = 13;
    last_time.tm_hour = 25;
    last_time.tm_min = 61;
    while(1){

        time(&tp);
        time_now = localtime(&tp);
        if(last_time.tm_year != time_now->tm_year){
            sprintf(year,"%d",time_now->tm_year+1900);
            ret = Image_SDK_Set_Text_Node_Text("Aaf", year,4);
        }
        //printf("tm_mon:%d\n",time_now->tm_mon);
        if(last_time.tm_mon != time_now->tm_mon){
            sprintf(year,"%d",time_now->tm_mon+1);
            ret = Image_SDK_Set_Text_Node_Text("Aag", year,2);
        }
        if(last_time.tm_mday != time_now->tm_mday){   
            sprintf(year,"%d",time_now->tm_mday);
            ret = Image_SDK_Set_Text_Node_Text("Aah", year,2);
        }
        if(last_time.tm_hour != time_now->tm_hour){
            sprintf(year,"%d",time_now->tm_hour);
            ret = Image_SDK_Set_Text_Node_Text("Aaj", year,2);
        }
        if(last_time.tm_min != time_now->tm_min){
            sprintf(year,"%d",time_now->tm_min);
            ret = Image_SDK_Set_Text_Node_Text("Aak", year,2);
        }
        if(last_time.tm_sec != time_now->tm_sec){
            sprintf(year,"%d",time_now->tm_sec);
            ret = Image_SDK_Set_Text_Node_Text("Aam", year,2);
        }
        last_time = *time_now;
        usleep(800000);
    }
    

}


int main(int argc,char **argv)
{
    /*
     *  root  = 'A'
     *  other = 'A.....'
     *
     * */
    //devcie init 
    int ret = 0;
    ret = gk_device_init(NULL); //ok
    start_read_venc_thread(); //ok 
    
    //////////load all png///////////
    xw_png_load_all();
    
    //////////init image handle//////
    Image_SDK_Init();
   
    //create top menu
    ret = xw_top_menu_show(NULL); 
    //create main menu
    ret = xw_main_menu_show(NULL);
    // 
    Image_SDK_Run();

    //
    Image_SDK_deInit();



    return 0;
}
