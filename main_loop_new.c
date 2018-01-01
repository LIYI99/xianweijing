#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


#include "gk_device_init.h"
#include "gk_image_sdk_new.h"
#include "xw_png_load.h"
#include "xw_main_menu_show.h"
#include "xw_top_menu_show.h"
#include "xw_date_show.h"
#include "xw_line_show.h"
#include "xw_main_line_manger_show.h"
#include "xw_main_isp_show.h"
#include "xw_logsrv.h"
#include "image_argb_ayuv.h"
#include "xw_text_prompt_box.h"
#include "xw_preview_show.h"


#include "xw_test_freshen.h"

int main(int argc,char **argv)
{
    /*
     *  root  = 'A'
     *  other = 'A.....'
     *
     * */
    //devcie init 
    int ret = 0;
    ret = xw_logsrv_init(NULL,6);
    


    ret = gk_device_init(NULL); //ok
    start_read_venc_thread(); //ok 
    //init ayunv table
    image_ayuv_talbe_init(RGBA8888_AYUV4444);
    //image_ayuv_talbe_init(RGB565_YUV565);
    //////////load all png///////////
    xw_png_load_all();
    
    //////////init image handle//////
    Image_SDK_Init();

#if 1
    //create top menu
    ret = xw_top_menu_show(NULL); 
    //create text put window
    ret = xw_text_prompt_box_show(NULL);
    //create main menu
     ret = xw_main_menu_show(NULL);
    // create date
    xw_date_show_thread(NULL);
    // create lines
    xw_line_show_all(NULL);
    // create lines manger
    xw_main_line_manger_show(NULL);
    xw_main_isp_show(NULL);
    xw_preview_show(NULL);
 


#else

    xw_test_freshen_show();
    //test_32_png_image_put();
    xw_line_show_all(NULL);
#endif

    Image_SDK_Run();
    
    int i =0 ;
    for(; ;i++){
        sleep(1);

    }
    xw_preview_quit(NULL);
    xw_main_line_quit_show(NULL);
    xw_date_quit_thread(NULL); 
    xw_main_isp_quit_show(NULL);
    xw_line_quit_all(NULL);
    xw_text_prompt_box_quit(NULL);
    
    image_ayuv_talbe_deinit();
    Image_SDK_deInit();
    xw_logsrv_deinit();
    xw_png_destory();



    return 0;
}
