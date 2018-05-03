#include <unistd.h>
#include "xw_main_start.h"
#include "gk_device_init.h"
#include "image_sdk_core.h"
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
#include "xw_msg_prv.h"
#include "xw_window_def_func.h"
#include "xw_update.h"
#include "xw_key_get.h"

static void  main_sdk_params_init( sdk_init_param_t *params,srcee_mode_type type)
{
    
    if(type  == SRCEE_MODE_1080)
    {
        params->font_h          =    16;
        params->font_w          =    8;
        params->srcee_w         =    1920;
        params->srcee_h         =    1080;
        params->base_srcee_w    =    1920;   //now device not modfily
        params->base_srcee_h    =    1080;
        params->color_fmt       =    16;
    }else if(type  == SRCEE_MODE_600){
        params->font_h          =    8;
        params->font_w          =    8;
        params->srcee_w         =    1024;
        params->srcee_h         =    600;
        params->base_srcee_w    =    1920;   //now device not modfily
        params->base_srcee_h    =    1080;
        params->color_fmt       =    16;
    }
    
    params->mouse_cahce = xw_get_window_png(MOUSE_ID);
    params->mouse_cahce_v2 = xw_get_window_png(MOUSE_HANDLE_ID);
    xw_get_png_hw(MOUSE_ID,&params->mouse_w,&params->mouse_h);
    xw_get_png_hw(MOUSE_HANDLE_ID,&params->mouse_w_v2,&params->mouse_h_v2);

    xw_logsrv_err("debug mouse_cahce:%p mouse_cahce_v2:%p\n",params->mouse_cahce,
            params->mouse_cahce_v2);

    return;
}


static uint8_t     main_loop_quit = 0;
int     xw_main_ui_start(int times,srcee_vout_mode mode,IDS_DEV_MODE devmode){

	int ret = 0;
	//init logsvr
	ret = xw_logsrv_init("/mnt/para/log.txt",6);

	//init def 
	srcee_mode_type type  = *((srcee_mode_type *)&mode);
	ret  = xw_window_def_params_init(type);
	//start msg connet
	ret =  Image_Msg_Start();
	if(ret < 0){
		xw_logsrv_err("connet the srv fail \n");
	}
	//init ayunv table
	// image_ayuv_talbe_init(RGBA8888_AYUV4444);
	image_ayuv_talbe_init(RGBA8888_AYUV1555);

	//load all UI
	//if(devmode == HDMI_DEV_ONLY) 
	xw_png_load_all(type);



	//init sdk params
	sdk_init_param_t    params;
	memset(&params,0x0,sizeof(params));
	main_sdk_params_init(&params,type);

	Image_SDK_Init(&params);
	//create top menu
	if(devmode == HDMI_DEV_ONLY)
		ret = xw_top_menu_show(NULL); 
	//create text put window
	ret = xw_text_prompt_box_show(NULL);
	//create update menu
	if(devmode == HDMI_DEV_ONLY)
		ret = xw_update_show(NULL);

	if(devmode == HDMI_DEV_ONLY){		    	
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
	}
	
	
	//not block
	Image_SDK_Run();
	if(devmode == HDMI_KEY_DEV)
		xw_key_show(NULL);
		

	int times_cnt = 0 ;

	for(;;)
	{
		if(main_loop_quit)
			break;
		sleep(2);
		if(times < 0)
		{
			continue;
		}
		times_cnt += 2;
		if(times_cnt > times)
			break;
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
	xw_window_def_params_deinit();

	return 0;
}

int xw_main_ui_quit(void)
{
    main_loop_quit = 1;
    return 0;
}

int     xw_main_ui_start_update(void)
{
    xw_update_enble(NULL);

    return 0;
}
int     xw_main_ui_quit_update(void)
{
    xw_update_quit(NULL);
    return 0;
}

int xw_main_ui_push_update_sclce(int vuale)
{
    if(vuale < 0)
        return 0;
    return xw_update_push_sc(vuale);
}

int xw_main_ui_push_message(char *s,int msec)
{
	 
    xw_text_promt_put(s,msec);
	return 0;

}


