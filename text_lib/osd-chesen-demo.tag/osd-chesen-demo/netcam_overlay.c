/*
 * GoKe 
 * 20151211
 *
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "netcam_overlay.h"
#include "netcam_overlay_cfg.h"
#include "overlay.h"

static pthread_rwlock_t             overlay_lock;

static bool                         _overlay_chs = 0;


static inline int overlay_enter_lock(void)
{
	return pthread_rwlock_wrlock(&overlay_lock);
}

static int overlay_leave_lock(void)
{
	return pthread_rwlock_unlock(&overlay_lock);
}

static int netcam_video_get_channel_number()
{
    return 2;//main sub
}

//----ok---
static void netcam_OVERLAY_release_title(int vin){
    
    OVERLAY_CFG_HANLDE* _handle = NULL;
    //获取配置文件句柄
    _handle = overlay_cfg_get_hanlde();
    
    if(_handle == NULL)
        return ;
    int i;
	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    //如果已经开启了这个块的缓存，就需要释放句柄	
        if(NULL != OsdApiHandle->get_overlay_canvas(vin, i, _handle->_plan_info[i]._text.title)){
			OsdApiHandle->release_overlay(vin, i,_handle->_plan_info[i]._text.title);
		}
        _handle->_plan_info[i]._text.canvas = NULL;

	}
    //释放句柄
    overlay_cfg_free_hanlde();
    return;

}

//----ok---,擦除像素缓存空间，重新载入文本转为像素存放在像素缓存空间
static int netcam_OVERLAY_set_title(int vin)
{
    
    OVERLAY_CFG_HANLDE* _handle = NULL;
    
    _handle = overlay_cfg_get_hanlde();

      
    if(_handle == NULL)
        return ;
    PS_GK_ENC_VIDEO_OVERLAY     canvas = NULL;
    int i;
	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    	if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._text.enable &&
                   _handle->_plan_info[i]._text.canvas){
                canvas = (PS_GK_ENC_VIDEO_OVERLAY)_handle->_plan_info[i]._text.canvas;
                //erase
                canvas->erase_rect(canvas, 0, 0, 0, 0);
                //put text
                OVERLAY_put_text(canvas, 0, 0, _handle->_plan_info[i]._text.font_size,
                        _handle->_plan_info[i]._text.buf, 0);
            }
            canvas = NULL;
    }
	
  	for(i = 0; i < MAX_STREAM_NUMS; ++i){
        if(_handle->_plan_info[i].enable &&  _handle->_plan_info[i]._text.enable)
    	    //update osd aere
            OsdApiHandle->update_overlay(vin, i,_handle->_plan_info[i]._text.title);
    }
    overlay_cfg_free_hanlde();

	return 0;
}


//----ok---
static int netcam_overlay_show_text(int vin,  bool show_flag)
{
	
    OVERLAY_CFG_HANLDE* _handle = NULL;
    _handle = overlay_cfg_get_hanlde();
    if(_handle == NULL)
        return ;
    int i;
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
        if(_handle->_plan_info[i].enable &&  _handle->_plan_info[i]._text.enable)
    	    OsdApiHandle->show_overlay(vin, i, _handle->_plan_info[i]._text.title, show_flag);
    }
    overlay_cfg_free_hanlde();
    return 0;
}

//---ok---
static int netcam_overlay_show_clock(int vin,  bool show_flag)
{
	
    OVERLAY_CFG_HANLDE* _handle = NULL;
    _handle = overlay_cfg_get_hanlde();
    if(_handle == NULL)
        return ;
    int i;
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
        if(_handle->_plan_info[i].enable &&  _handle->_plan_info[i]._time.enable)
    	    OsdApiHandle->show_overlay(vin, i, _handle->_plan_info[i]._time.title, show_flag);
    }
    overlay_cfg_free_hanlde();
    return 0;
}
//---ok----
static int netcam_overlay_show_ch(int vin,  bool show_flag)
{
	
    OVERLAY_CFG_HANLDE* _handle = NULL;
    _handle = overlay_cfg_get_hanlde();
    if(_handle == NULL)
        return ;
    int i;
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
        if(_handle->_plan_info[i].enable &&  _handle->_plan_info[i]._ch.enable)
    	    OsdApiHandle->show_overlay(vin, i, _handle->_plan_info[i]._ch.title, show_flag);
    }
    overlay_cfg_free_hanlde();
    return 0;
}






//----ok---
//创建所有的本文空间，相当于４个编码通道＊１　＝　４，每层一个
static int netcam_OVERLAY_create_title(int vin)
{
	   
    OVERLAY_CFG_HANLDE* _handle = NULL;
    
    _handle = overlay_cfg_get_hanlde();

      
    if(_handle == NULL)
        return ;
    int i;
    //根据配置信息来决定是否要，创建一个像素缓存
    //创建的空间大小参数　＝　计划打印字符长度＊字体＊字体
	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    	if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._text.enable){
                _handle->_plan_info[i]._text.canvas =  
                    OsdApiHandle->create_overlay_canvas(  _handle->_plan_info[i]._text.font_size*10
                            /*_handle->_plan_info[i]._text.font_h*/ ,  _handle->_plan_info[i]._text.font_size+8);
            }
    }
    //第二步即是创建块句柄 
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
            if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._text.enable){

                OsdApiHandle->create_overlay(vin, i, _handle->_plan_info[i]._text.title, 
                        _handle->_plan_info[i]._text.x, _handle->_plan_info[i]._text.y,
                        (PS_GK_ENC_VIDEO_OVERLAY const)_handle->_plan_info[i]._text.canvas);
                OsdApiHandle->update_overlay(vin, i, _handle->_plan_info[i]._text.title);
            }

    }
    
    overlay_cfg_free_hanlde();
    netcam_OVERLAY_set_title(vin);  //if write chinese ,please modfiy here
	netcam_overlay_show_text(vin, 1);
    return 0;
}

void netcam_OVERLAY_reload_title(int vin)
{
	//
    //复位
    netcam_OVERLAY_release_title(vin);
	//创建`
    netcam_OVERLAY_create_title(vin);
}
//////////////////////////////////////////////////////////////////
///////////// osd_clock////////////////////
////////////////////////////////////////////////////////////////

static void netcam_overlay_update_clock(time_t t) // per second
{
    
    OVERLAY_CFG_HANLDE*         _handle = NULL;
    PS_GK_ENC_VIDEO_OVERLAY     canvas = NULL;
    
    _handle = overlay_cfg_get_hanlde();
    if(_handle == NULL)
        return;


    int i;
	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    	if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._time.enable &&
                   _handle->_plan_info[i]._time.canvas){
                canvas = (PS_GK_ENC_VIDEO_OVERLAY)_handle->_plan_info[i]._time.canvas;
                canvas->erase_rect(canvas, 0, 0, 0, 0);
            }
            canvas = NULL;
    }
	overlay_cfg_free_hanlde();

    char text_buf[128]   = {""};

	struct tm tm_now = {0};

	{
		
        char* weekday_map_chs[] = {"ÐÇÆÚÈÕ", "ÐÇÆÚÒ»", "ÐÇÆÚ¶þ", "ÐÇÆÚÈý", "ÐÇÆÚËÄ", "ÐÇÆÚÎå", "ÐÇÆÚÁù",};
		char* weekday_map_eng[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT",};

		int     year = 0, month = 0, mday = 0, hour = 0, min = 0, sec = 0;
        char    date_buf[64]    = {""},
                time_buf[64]    = {""}; 
               		
        const char* meridiem    = "";
		const char* weekday     = "";

		// get system time
		localtime_r(&t, &tm_now);
        // the date
		year    = tm_now.tm_year + 1900;
		month   = tm_now.tm_mon + 1;
		mday    = tm_now.tm_mday;
		hour    = tm_now.tm_hour;
		min     = tm_now.tm_min;
		sec     = tm_now.tm_sec;
        
        switch(0)
		{
			default:
			case 0:
				snprintf(date_buf, sizeof(date_buf), "%04d/%02d/%02d", year, month, mday);
				break;

			case 1:
				snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d", month, mday, year);
				break;

			case 2:
				snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d", mday, month, year);
				break;
		}

		if(_overlay_chs)
		{
			
			if(hour < 6){
				meridiem = "Áè³¿";
			}else if(hour < 12){
				meridiem = "ÉÏÎç";
			}else if(hour < 18){
				meridiem = "ÏÂÎç";
			}else if(hour < 20){
				meridiem = "°øÍí";
			}else{
				meridiem = "ÍíÉÏ";
			}

			switch(1){
				default:
				case 0:
					snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", hour, min, sec);
					break;
				case 1:
					snprintf(time_buf, sizeof(time_buf), "%s%d:%02d:%02d", meridiem, hour < 12 ? hour : hour - 12, min, sec);
					break;
			}
			weekday = weekday_map_chs[tm_now.tm_wday];
		}
		else
		{
			 
			if(hour < 12){
				meridiem = "AM";
			}else{
				meridiem = "PM";
			}

			switch(1){
				default:
				case 0:
					snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", hour, min, sec);
					break;
				case 1:
					snprintf(time_buf, sizeof(time_buf), "%d:%02d:%02d%s", hour < 12 ? hour : hour - 12, min, sec, meridiem);
					break;
			}
			weekday = weekday_map_eng[tm_now.tm_wday];
		}
			
        snprintf(text_buf, sizeof(text_buf), "%s %s %s", date_buf, time_buf, weekday);
        printf("text_buf:%s\n",text_buf);
    }
	// put text
    

    _handle = overlay_cfg_get_hanlde();
    if(_handle == NULL)
        return;
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._time.enable &&
                   _handle->_plan_info[i]._time.canvas){
                canvas = (PS_GK_ENC_VIDEO_OVERLAY)_handle->_plan_info[i]._time.canvas;
                OVERLAY_put_text(canvas, 0, 0, _handle->_plan_info[i]._time.font_size, 
                        text_buf, 0);

            }
            canvas = NULL;
    }

	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._time.enable){
            
		    OsdApiHandle->update_overlay(0, i,  _handle->_plan_info[i]._time.title);
        }
    }

    overlay_cfg_free_hanlde();

    netcam_overlay_show_clock(0, 1);
}

void* task_update_overlay_clock(void *data)
{
	for(;;){
        
        netcam_overlay_update_clock(time(NULL));
        usleep(500000);
      //  printf("--------run----clock\n");
    }
}

void netcam_OVERLAY_reload_ch(int vin){
    return;
}

static int netcam_OVERLAY_create_clock(int vin)
{
	
    
    OVERLAY_CFG_HANLDE* _handle = NULL;
    
    _handle = overlay_cfg_get_hanlde();

    if(_handle == NULL)
        return -1;
    
    int i;

	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    	if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._time.enable){
                _handle->_plan_info[i]._time.canvas =  
                    OsdApiHandle->create_overlay_canvas(  _handle->_plan_info[i]._time.font_size*
                            _handle->_plan_info[i]._time.font_h ,  _handle->_plan_info[i]._time.font_size);
            }
    }
    
    for(i = 0; i < MAX_STREAM_NUMS; ++i){
            if(_handle->_plan_info[i].enable && _handle->_plan_info[i]._time.enable){

                OsdApiHandle->create_overlay(vin, i, _handle->_plan_info[i]._time.title, 
                        _handle->_plan_info[i]._time.x, _handle->_plan_info[i]._time.y,
                        (PS_GK_ENC_VIDEO_OVERLAY const)_handle->_plan_info[i]._time.canvas);
                OsdApiHandle->update_overlay(vin, i, _handle->_plan_info[i]._time.title);
            }

    }
    overlay_cfg_free_hanlde();

    int clock_enable = 1;//gokeinside
	netcam_overlay_show_clock(vin,1);
    
    pthread_t   clock_thread_id;
    printf("--------------start------clock thread----------\n");
    pthread_create(&clock_thread_id,NULL,task_update_overlay_clock,NULL); 

   	return 0;

}

static void netcam_OVERLAY_release_clock(int vin)
{
	OVERLAY_CFG_HANLDE* _handle = NULL;
    _handle = overlay_cfg_get_hanlde();
    
    if(_handle == NULL)
        return ;
    int i;
	for(i = 0; i < MAX_STREAM_NUMS; ++i){
	    	
        if(NULL != OsdApiHandle->get_overlay_canvas(vin, i, _handle->_plan_info[i]._time.title)){
			OsdApiHandle->release_overlay(vin, i,_handle->_plan_info[i]._time.title);
		}
        _handle->_plan_info[i]._time.canvas = NULL;

	}
    overlay_cfg_free_hanlde();
    return;
   
    return ;

}

void netcam_OVERLAY_reload_clock(int vin)
{
	printf("func:%s line:%d \n","_reload_clock",__LINE__);

    netcam_OVERLAY_release_clock(vin);
	netcam_OVERLAY_create_clock(vin);
}

static int OSD_handle_init(void){
   
    if(osdHandle != NULL)
        return -1;
    int retVal = gadi_osd_init();
    osdHandle = gadi_osd_open(&retVal);
    printf("func:%s line:%d \n","OSD_handle_init",__LINE__);
    return 0;
}

/*
 *  ----------osd 设计示意-----------
 *
 *  打开配置信息
 *      |
 *      |
 *  打开osd句柄
 *      |
 *      |
 *  打开osd区域操作句柄
 *      |
 *      |
 *  载入字库
 *      |
 *      |
 *  根据配置信息载入ＯＳＤ信息，时间信息需要不停的刷新
 *
 *
 * */


#define     Font16_asc_path "/font/asc16"
#define     Font32_asc_path "/font/asc32"
#define     Font16_hzk_path "/font/hzk16"
#define     Font32_hzk_path "/font/hzk32"

//OSD功能启动函数,如需要开启动ＯＳＤ功能，直接调用这个函数可以获得ＯＳＤdemo效果
int netcam_overlay_start(void *data)
{
    int ret  = 0;
    //OSD配置信息加载
    ret = overlay_cfg_hanlde_init(); 
    if(ret < 0){
        printf("----init cfg handle fail \n");
        return -1;
    }
    //init osd handle
    //主要用于打ADIOSDHanlde
    OSD_handle_init();

    //init osd api handle
    //用于初始化osd各操作方法
    OVERLAY_API_init();

    //载入字库，16位及32位
    OVERLAY_load_font(16, Font16_asc_path, Font16_hzk_path);
    OVERLAY_load_font(32, Font32_asc_path, Font32_hzk_path);
    //
    //加载文本osd
    netcam_OVERLAY_reload_title(0);
    
    //test clock ,need ctread thread run task_func
    //启动时间osd
    netcam_OVERLAY_reload_clock(0);
    //通道ＩＤ　osd，暂未开启。
    netcam_OVERLAY_reload_ch(0);
    return 0; 
}




