#ifndef _NETCAM_OVERLAY_H_
#define _NETCAM_OVERLAY_H_


#define OSD_TILE_X  0
#define OSD_TILE_y  0
#define OSD_TIME_X  200
#define OSD_TIME_y  200

typedef unsigned int OVERLAY_TEXT_STYLE_t;
#if 0
#define OVERLAY_TEXT_STYLE_FOREGROUND_TRANSPARENT (1<<0)
#define OVERLAY_TEXT_STYLE_FOREGROUND_WHRITE (1<<1)
#define OVERLAY_TEXT_STYLE_FOREGROUND_BLACK (1<<2)
#define OVERLAY_TEXT_STYLE_BACKGROUND_TRANSPARENT (1<<4)
#define OVERLAY_TEXT_STYLE_BACKGROUND_WHRITE (1<<5)
#define OVERLAY_TEXT_STYLE_BACKGROUND_BLACK (1<<6)
#define OVERLAY_TEXT_STYLE_ENCLOSED_TRANSPARENT (1<<8)
#define OVERLAY_TEXT_STYLE_ENCLOSED_WHRITE (1<<9)
#define OVERLAY_TEXT_STYLE_ENCLOSED_BLACK (1<<10)
#define OVERLAY_TEXT_STYLE_DEFAULT \
        (OVERLAY_TEXT_STYLE_FOREGROUND_WHRITE\
         OVERLAY_TEXT_STYLE_ENCLOSED_BLACK)
        //|OVERLAY_TEXT_STYLE_BACKGROUND_TRANSPARENT)
#endif

//时间刷新线程函数
void* task_update_overlay_clock(void *data);
//重启时间函数
void netcam_OVERLAY_reload_clock(int vin);
//重置所有文本块函数
void netcam_OVERLAY_reload_title(int vin);
//重置所有通道块函数
void netcam_OVERLAY_reload_ch(int vin);
//启动函数数，在集成中可以用这个函数开启ＯＳＤ功能
void netcam_OVERLAY_start(void* data);


#endif

