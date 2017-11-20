#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gk_device_init.h"
#include "gk_image_sdk.h"

extern image_sdk_t  *sdk_handle;

static  void  mouse_leave_botton_func_v2(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t*)(data);
    bt->color = 0xFF80;
}

static  void  mouse_leave_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t*)(data);
    bt->color = 0xF00F;
}

static  void  mouse_offset_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t*)(data);
    bt->color = 0xBBCA;
}

static  void  mouse_rdown_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);
    bt->color = 0xFE41;
}

static  void  mouse_rup_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);
    bt->color = 0xFBE4;
}

static  void  mouse_lup_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);
    bt->color = 0x7497;
}

static  void  mouse_lup_botton_func_v2(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);
    bt->color = 0x7497;
    bt->en_follow = 0;
}


static  void  mouse_ldown_botton_func(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);                 
    bt->color = 0xFD79;
}


static  void  mouse_ldown_botton_func_v2(void *data)
{
    
    image_sdk_button_t *bt  =  (image_sdk_button_t *)(data);                 
    bt->color = 0xFD79;
    bt->en_follow = 1;
}


static  void  mouse_lup_line_func_v2(void *data)
{
    
    image_sdk_line_t *bt  =  (image_sdk_line_t *)(data);
    bt->color = 0x7497;
    bt->en_follow = 0;
}


static  void  mouse_ldown_line_func_v2(void *data)
{
    
//    printf("data:%p\n",data);
    image_sdk_line_t *bt  =  (image_sdk_line_t *)(data);            
    bt->color = 0xFD79;
    bt->en_follow = 1;
}


int main(int argc,char **argv)
{
    
    
    int ret = 0;
    ret = gk_device_init(NULL); //ok
    start_read_venc_thread(); //ok 
    
    Image_SDK_Init();
    
    
    image_sdk_button_t bt;
    
    memset(&bt,0x0,sizeof(image_sdk_button_t));
    bt.x = 700;
    bt.y = 100;
    bt.w = 40;
    bt.h = 40;
    bt.color = 0xf00f;
    bt.mouse_offset = mouse_offset_botton_func;
    bt.mouse_leave = mouse_leave_botton_func;
    bt.mouse_left_down = mouse_ldown_botton_func;
    bt.mouse_left_up = mouse_lup_botton_func;


    bt.x = 700;
    bt.y = 400;
    bt.w = 120;
    bt.h = 120;
    bt.color = 0xf00f;
    ret = Image_SDK_Create_Button(bt);
    bt.mouse_leave = mouse_leave_botton_func_v2; 
    bt.x = 750;
    bt.y = 450;
    bt.w = 120;
    bt.h = 120;
    bt.color = 0xFF80;
    ret = Image_SDK_Create_Button(bt);



    ret = Image_SDK_Create_Button(bt);
    
    bt.x = 800;
    bt.y = 100;
    bt.w = 40;
    bt.h = 40;
    bt.color = 0xf00f;
    ret = Image_SDK_Create_Button(bt);


    bt.x = 900;
    bt.y = 100;
    bt.w = 40;
    bt.h = 40;
    bt.color = 0xf00f;
    bt.mouse_left_down = mouse_ldown_botton_func_v2;
    bt.mouse_left_up = mouse_lup_botton_func_v2;
    ret = Image_SDK_Create_Button(bt);

   

   
    image_sdk_line_t    lt;

    memset(&lt,0x0,sizeof(image_sdk_line_t));

    lt.start_x = 20;
    lt.start_y = 800;
    lt.end_y = 800;
    lt.end_x = 1900;
    lt.size = 1;
    lt.color = 0xf00f;
    lt.mouse_left_down = mouse_ldown_line_func_v2;
    lt.mouse_left_up = mouse_lup_line_func_v2;
    
    ret = Image_SDK_Create_Line(lt);



    memset(&lt,0x0,sizeof(image_sdk_line_t));

    lt.start_x = 20;
    lt.start_y = 800;
    lt.end_y = 800;
    lt.end_x = 1900;
    lt.size = 1;
    lt.color = 0xf00f;
    lt.mouse_left_down = mouse_ldown_line_func_v2;
    lt.mouse_left_up = mouse_lup_line_func_v2;
    
    ret = Image_SDK_Create_Line(lt);
    

    memset(&lt,0x0,sizeof(image_sdk_line_t));

    lt.start_x = 20;
    lt.start_y = 950;
    lt.end_y = 950;
    lt.end_x = 1900;
    lt.size = 1;
    lt.color = 0xf00f;
    lt.mouse_left_down = mouse_ldown_line_func_v2;
    lt.mouse_left_up = mouse_lup_line_func_v2;
    
    ret = Image_SDK_Create_Line(lt);



    Image_SDK_Run();

    //
    Image_SDK_deInit();



    return 0;
}
