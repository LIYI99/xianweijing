#ifndef _OVERLAY_H_
#define _OVERLAY_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "adi_osd.h"

typedef uint32_t OVERLAY_TEXT_STYLE_t;
//前景到透明
#define OVERLAY_TEXT_STYLE_FOREGROUND_TRANSPARENT (1<<0)
//前景到白色
#define OVERLAY_TEXT_STYLE_FOREGROUND_WHRITE (1<<1)
//前景到黑色
#define OVERLAY_TEXT_STYLE_FOREGROUND_BLACK (1<<2)
//背景到透明
#define OVERLAY_TEXT_STYLE_BACKGROUND_TRANSPARENT (1<<4)
//背景到白色
#define OVERLAY_TEXT_STYLE_BACKGROUND_WHRITE (1<<5)
//背景到黑色
#define OVERLAY_TEXT_STYLE_BACKGROUND_BLACK (1<<6)
//关闭透明
#define OVERLAY_TEXT_STYLE_ENCLOSED_TRANSPARENT (1<<8)
//前闭白
#define OVERLAY_TEXT_STYLE_ENCLOSED_WHRITE (1<<9)
//关闭黑
#define OVERLAY_TEXT_STYLE_ENCLOSED_BLACK (1<<10)
//默认是前景白色背景透明
#define OVERLAY_TEXT_STYLE_DEFAULT \
        (OVERLAY_TEXT_STYLE_FOREGROUND_WHRITE\
        |OVERLAY_TEXT_STYLE_BACKGROUND_TRANSPARENT)

#define GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(_pixel, _a, _r, _g, _b) \
    do{\
        _pixel.alpha = (_a), _pixel.red = (_r), _pixel.green = (_g), _pixel.blue = (_b);\
    }while(0)

typedef enum OVERLAY_FONT_SIZE {
    kOVERLAY_FONT_SIZE_16 = (16),
    kOVERLAY_FONT_SIZE_20 = (20),
    kOVERLAY_FONT_SIZE_24 = (24),
    kOVERLAY_FONT_SIZE_32 = (32),
}enOVERLAY_FONT_SIZE;


typedef union  {
    uint32_t argb8888;
    struct {
        uint8_t blue, green, red, alpha;
    };
}ST_GK_ENC_VIDEO_OVERLAY_PIXEL, *PS_GK_ENC_VIDEO_OVERLAY_PIXEL;
typedef struct GK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT {
    uint32_t rmask, gmask, bmask, amask;
}ST_GK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT, *PS_GK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT;


typedef struct  GK_ENC_VIDEO_OVERLAY{

	size_t width, height;
	ST_GK_ENC_VIDEO_OVERLAY_PIXEL_FORMAT pixel_format;
	void* pixels;

	int (*put_pixel)(struct GK_ENC_VIDEO_OVERLAY* canvas, int x, int y,
		ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
	int (*get_pixel)(struct GK_ENC_VIDEO_OVERLAY* canvas, int x, int y,
		PS_GK_ENC_VIDEO_OVERLAY_PIXEL ret_pixel);
	bool (*match_pixel)(struct GK_ENC_VIDEO_OVERLAY* canvas, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel1, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel2);
	int (*put_rect)(struct GK_ENC_VIDEO_OVERLAY* canvas, int x, int y,
		size_t width, size_t height,ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
	int (*fill_rect)(struct GK_ENC_VIDEO_OVERLAY* canvas, int x, int y,
		size_t width, size_t height, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
	int (*erase_rect)(struct GK_ENC_VIDEO_OVERLAY* canvas, int x, int y, size_t width, size_t height);

}ST_GK_ENC_VIDEO_OVERLAY, *PS_GK_ENC_VIDEO_OVERLAY;

/*overlay parameters.*/
typedef struct
{
    char name[32];
    GADI_OSD_AreaMappingT       areaMapping;
    GADI_OSD_AreaParamsT        areaParams;
    PS_GK_ENC_VIDEO_OVERLAY     canva;

}GK_ENC_OverlayParams;

typedef struct GK_OSD_API_HD {
    // overlay
    PS_GK_ENC_VIDEO_OVERLAY (*create_overlay_canvas)(size_t width, size_t height);
    PS_GK_ENC_VIDEO_OVERLAY (*load_overlay_canvas)(const char *bmp24_path);
    void (*release_overlay_canvas)(PS_GK_ENC_VIDEO_OVERLAY canvas);

    int (*create_overlay)(int vin, int stream, const char* overlay_name,
            float x, float y, PS_GK_ENC_VIDEO_OVERLAY const canvas);
    int (*release_overlay)(int vin, int stream, const char* overlay_name);

    PS_GK_ENC_VIDEO_OVERLAY (*get_overlay_canvas)(int vin, int stream, const char* overlay_name);

    int (*show_overlay)(int vin, int stream, const char* overlayName, bool showFlag);
    int (*update_overlay)(int vin, int stream, const char* overlay_name);

}ST_GK_OSD_API_HD, *PS_GK_OSD_API_HD;

///////////////cover//////////////////////
int OVERLAY_load_font(enOVERLAY_FONT_SIZE font_size, const char* asc_font, const char* gb2312_font);


extern PS_GK_OSD_API_HD         OsdApiHandle;
extern GADI_SYS_HandleT         osdHandle;


ssize_t OVERLAY_put_text(PS_GK_ENC_VIDEO_OVERLAY canvas,
	int x, int y, enOVERLAY_FONT_SIZE font_size, const char *text, OVERLAY_TEXT_STYLE_t style);
#endif

