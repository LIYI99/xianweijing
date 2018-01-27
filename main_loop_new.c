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
#include "xw_main_start.h"

int main(int argc,char **argv)
{
    /*
     *  root  = 'A'
     *  other = 'A.....'
     *
     * */

    int  times = 60;
    if(argv[1] != NULL){
        times = atoi(argv[1]);
        printf("run times:%d\n",times);
        //xw_logsrv_debug("run times:%d \n",times);
    }
    

    int ret = 0;
    //init losgsrv
    //int device
    ret = gk_device_init(NULL); //ok
    ret =start_read_venc_thread(); //ok
   
    xw_main_ui_start(times,0);

#if 0
    //init logsvr
    ret = xw_logsrv_init(NULL,6);
    //start msg connet
    ret =  Image_Msg_Start();
    if(ret < 0){
        xw_logsrv_err("connet the srv fail \n");
    }
    //init ayunv table
    image_ayuv_talbe_init(RGBA8888_AYUV4444);
    //load all UI
    xw_png_load_all();
    //////////init image sdk//////
    Image_SDK_Init();

#if 1

    //create top menu
    ret = xw_top_menu_show(NULL); 
    //create text put window
    ret = xw_text_prompt_box_show(NULL);
    //create main menu
     ret = xw_main_menu_show(NULL);
    // create date
    xw_date_show_thread(NULL);v
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
    //not block
    Image_SDK_Run();
    
    for(;times > 0;){
        sleep(3);
        times -= 3;
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


#endif

    return 0;
}
