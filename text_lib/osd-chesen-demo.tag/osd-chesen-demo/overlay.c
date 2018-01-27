#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/stat.h>
#include "overlay.h"
#include "adi_pm.h"


#define SDK_ALIGNED_LITTLE_ENDIAN(__val, __align) \
	(((__val) / (__align)) * (__align))
#define SDK_ALIGNED_BIG_ENDIAN(__val, __align) \
	SDK_ALIGNED_LITTLE_ENDIAN((__val) + (__align) - 1, (__align))
#define SLOG(info,fmt, arg...) printf(fmt, ##arg)



#define GK7101_MAX_OVERLAY_NUMS     (OSD_PLANE_NUM * OSD_AREA_NUM)
#define MAX_VENC_CHN_NUM_QQZM_TEST  2
#define GK7101_CLUT_MAX_COLOUR_NUM  (256)
//位图定义
typedef struct BIT_MAP_FILE_HEADER
{
    char type[2]; // "BM" (0x4d42)
    uint32_t file_size;
    uint32_t reserved_zero;
    uint32_t off_bits; // data area offset to the file set (unit. byte)
    uint32_t info_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes; // 0 - 1
    uint16_t bit_count; // 0 - 1
    uint32_t compression; // 0 - 1
    uint32_t size_image; // 0 - 1
    uint32_t xpels_per_meter;
    uint32_t ypels_per_meter;
    uint32_t clr_used;
    uint32_t clr_important;
}__attribute__((packed)) BIT_MAP_FILE_HEADER_t; //
//argb8888　定义
/*colour look-up table struct of RGB domain.*/
typedef union
{
    uint32_t  argb8888;
    struct {
        unsigned char   b;
        unsigned char   g;
        unsigned char   r;
        unsigned char   a;
    };
}GK_RGBColFormat;
//YUYa设表定义
/*colour look-up table struct of YUV domain.*/
typedef struct
{
    unsigned char   v;
    unsigned char   u;
    unsigned char   y;
    unsigned char   a;
}GK_YUVColFormat;

GADI_SYS_HandleT    osdHandle       = NULL;
PS_GK_OSD_API_HD    OsdApiHandle    = NULL;

static uint8_t*     _asc_16     = NULL; // bytes 128 x 16 x 1 size 8x16
static uint8_t*     _asc_20     = NULL; // bytes 128 x 20 x 2 size 10x20
static uint8_t*     _asc_24     = NULL; // bytes 128 x 24 x 2 size 12x24
static uint8_t*     _asc_32     = NULL; // bytes 128 x 32 x 2 size 16x32

static uint8_t*     _gb2312_16 = NULL; // bytes 87 x 94 x 16 x 2 size 16x16
static uint8_t*     _gb2312_20 = NULL; // bytes 87 x 94 x 20 x 3 size 20x20
static uint8_t*     _gb2312_24 = NULL; // bytes 87 x 94 x 24 x 3 size 24x24
static uint8_t*     _gb2312_32 = NULL; // bytes 87 x 94 x 32 x 4 size 32x32
static uint8_t      _def_alpha = 64;

static OVERLAY_TEXT_STYLE_t     _def_sytle = OVERLAY_TEXT_STYLE_DEFAULT;
/*overlay.*/
static ST_GK_ENC_VIDEO_OVERLAY  overlayCanvas[GK7101_MAX_OVERLAY_NUMS];
static GK_ENC_OverlayParams     overlays[OSD_PLANE_NUM][OSD_AREA_NUM];




static PS_GK_ENC_VIDEO_OVERLAY gk_enc_create_overlay_canvas(size_t width,size_t height);
static PS_GK_ENC_VIDEO_OVERLAY gk_enc_get_overlay_canvas(int vin,int stream, const char* overlay_name);
static PS_GK_ENC_VIDEO_OVERLAY gk_enc_load_overlay_canvas(const char*bmp24_path);
static void gk_enc_release_overlay_canvas(PS_GK_ENC_VIDEO_OVERLAY canvas);
static int gk_enc_create_overlay(int vin, int stream, const char* overlay_name,
    float x, float y, PS_GK_ENC_VIDEO_OVERLAY const canvas);
static int gk_enc_release_overlay(int vin, int stream, const char* overlay_name);
static int gk_enc_show_overlay(int vin, int stream, const char* overlayName,
                                                                  bool showFlag);
static int gk_enc_update_overlay(int vin, int stream, const char* overlay_name);
static int overlay_canvas_put_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
                              int x, int y, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
static int overlay_canvas_get_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
                         int x, int y, ST_GK_ENC_VIDEO_OVERLAY_PIXEL* ret_pixel);
static bool overlay_canvas_match_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
     ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel1, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel2);
static int overlay_canvas_put_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
  int x, int y, size_t width, size_t height,ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
static int overlay_canvas_fill_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
 int x, int y, size_t width, size_t height, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel);
static int overlay_canvas_erase_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
                                      int x, int y, size_t width, size_t height);

/**
 *
 *  gk osd 操作与刷新示意过程
 *
 *　　初始化创建打开adi osd 句柄
            |
 *          |
 *   　 打开字库
 *          |
 *          |
 *     获取需要打印的信息（时间或文件或其它）
 *          |
 *          |
 *  　　将文本匹配字库初始化为像素位图（常做一buffer保存像素数据）
 *          ｜
 *          ｜
 *      将像素位图转化为色表，写入osd空间
 *          |
 *          |
 *      设置osd参数使能osd显示，显示数据（如果是文本这个过程只需要做一次，
 *      如果是时间则从三步起，来回不停的做）
*       
 *    set osd arerparms
 *          
 */



void OVERLAY_API_init()
{
    /*check parameters*/
    if(OsdApiHandle != NULL)
    {
        SLOG(ERR,"gk_enc_init has been called.");
        return ;
    }
    /*alloc memory.*/
    //分配空间
    OsdApiHandle  = (PS_GK_OSD_API_HD)gadi_sys_malloc(sizeof(ST_GK_OSD_API_HD));
    if(OsdApiHandle == NULL)
    {
        SLOG(ERR,"gk_enc_init malloc failed.");
        return ;
    }
    //初始化回调函数
    /*overlay canvas callback*/
    //创建一个osd块的缓存的回调
    OsdApiHandle->create_overlay_canvas  = gk_enc_create_overlay_canvas;
    //向块载的缓存入一个位图的方法
    OsdApiHandle->load_overlay_canvas    = gk_enc_load_overlay_canvas;
    //复位块的缓存
    OsdApiHandle->release_overlay_canvas = gk_enc_release_overlay_canvas;
    //创建一个块句柄，必须带上一个缓存空间，即由第一个方法创建的句柄
    OsdApiHandle->create_overlay         = gk_enc_create_overlay;
    //复位一个块
    OsdApiHandle->release_overlay        = gk_enc_release_overlay;
    //获取一个流中的一个块的缓存的回调
    OsdApiHandle->get_overlay_canvas     = gk_enc_get_overlay_canvas;
    //显示或开启块的方法
    OsdApiHandle->show_overlay           = gk_enc_show_overlay;
    //刷新块
    OsdApiHandle->update_overlay         = gk_enc_update_overlay;
    
    return;

}

//
//
//创建一个缓存一次将所有字库数据载入内存
static int overlay_load_font_mem(const char* file_name, uint8_t **ret_mem)
{
	struct stat file_stat={0};
	FILE* fid = NULL;
	if(0 == stat(file_name, &file_stat)){
		fid = fopen(file_name, "rb");
		if(NULL != fid){
			if(NULL != *ret_mem){
				free(*ret_mem);
			}
			*ret_mem = calloc(file_stat.st_size, 1);
			fread(*ret_mem, 1, file_stat.st_size, fid);
			fclose(fid);
			fid = NULL;
			return 0;
		}
	}
	return -1;
}

//载入字库
int OVERLAY_load_font(enOVERLAY_FONT_SIZE font_size, const char* asc_font, const char* gb2312_font)
{
	uint8_t **asc_mem = NULL, **gb2312_mem = NULL;
    //获取地址常量的地址	
	switch(font_size){
		case kOVERLAY_FONT_SIZE_16:
			asc_mem = &_asc_16;
			gb2312_mem = &_gb2312_16;
			break;

		case kOVERLAY_FONT_SIZE_20:
			asc_mem = &_asc_20;
			gb2312_mem = &_gb2312_20;
			break;

		case kOVERLAY_FONT_SIZE_24:
			asc_mem = &_asc_24;
			gb2312_mem = &_gb2312_24;
			break;

		case kOVERLAY_FONT_SIZE_32:
			asc_mem = &_asc_32;
			gb2312_mem = &_gb2312_32;
			break;

		default:
			return -1;
	}
    //加载字库到缓存
	if(0 == overlay_load_font_mem(asc_font, asc_mem)
		&& 0 == overlay_load_font_mem(gb2312_font, gb2312_mem))
	{
		return 0;
	}
    //失败则释放空间
	free(*asc_mem);
	free(*gb2312_mem);
	return -1;
	
}
ssize_t OVERLAY_put_text(PS_GK_ENC_VIDEO_OVERLAY canvas,
	int x, int y, enOVERLAY_FONT_SIZE font_size, const char *text, OVERLAY_TEXT_STYLE_t style)
{

	int i = 0, ii = 0, iii = 0;
	int const x_base = x;
	int const y_base = y;
	
	char *ch = (char *)text; // at the beginning of the text
	size_t asc_width = 0;
	size_t gb2312_width = 0;
	size_t text_width = 0, text_height = 0;
   //前景色，背景色，关闭色
	ST_GK_ENC_VIDEO_OVERLAY_PIXEL fg_color, bg_color, enclosed_color;
	bool is_enclosed = false;

	if(!canvas){
		return -1;
	}
    //字体判断字体库是否打开	
	switch(font_size){
		case kOVERLAY_FONT_SIZE_16:
			asc_width = 8;
			gb2312_width = 16;
			if(!_asc_16 || !_gb2312_16){
				return -1;
			}
			break;

		case kOVERLAY_FONT_SIZE_20:
			asc_width = 10;
			gb2312_width = 20;
			if(!_asc_20 || !_gb2312_20){
				return -1;
			}
			break;

		case kOVERLAY_FONT_SIZE_24:
			asc_width = 12;
			gb2312_width = 24;
			if(!_asc_24 || !_gb2312_24){
				return -1;
			}
			break;

		case kOVERLAY_FONT_SIZE_32:
			asc_width = 16;
			gb2312_width = 32;
			if(!_asc_32 || !_gb2312_32){
				return -1;
			}
			break;
			
		default:
			// not support font size
			return -1;
	}
	//字体高＝于，字体本身与像素缓存本身中小的那个
	text_height = font_size < canvas->height ? font_size : canvas->height;

    //如果传入的顔色值参为0 则采用默认前背景色,黑/白
	if(!style){
		style = _def_sytle;
	}

	// check style
	//if(style & OVERLAY_TEXT_STYLE_BACKGROUND_TRANSPARENT){
	//	GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(bg_color, 0, 255, 255, 0);
    //    printf("ooooooooooooooooooooooooooooooo %s, %d\n", __func__, __LINE__);
	//}else if(style & OVERLAY_TEXT_STYLE_BACKGROUND_WHRITE){
	//	GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(bg_color, _def_alpha, 255, 255, 255);
    //    printf("ooooooooooooooooooooooooooooooo %s, %d\n", __func__, __LINE__);
	//}else if(style & OVERLAY_TEXT_STYLE_BACKGROUND_BLACK){
	//	GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(bg_color, _def_alpha, 0, 0, 0);
    //    printf("ooooooooooooooooooooooooooooooo %s, %d\n", __func__, __LINE__);
	//}
    
    //初始化一个背景像素为黑色
	GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(bg_color, _def_alpha, 0, 0, 0);
	
    //获取前景色
	if(style & OVERLAY_TEXT_STYLE_FOREGROUND_TRANSPARENT){
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(fg_color, 0, 255, 0, 255);
	}else if(style & OVERLAY_TEXT_STYLE_FOREGROUND_WHRITE){
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(fg_color, _def_alpha ^ 0x11, 255, 255, 255);
	}else if(style & OVERLAY_TEXT_STYLE_FOREGROUND_BLACK){
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(fg_color, _def_alpha ^ 0x11, 0, 0, 0);
	}
    //获取背景色
	if(style & OVERLAY_TEXT_STYLE_ENCLOSED_TRANSPARENT){
		is_enclosed = true;
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(enclosed_color, 0, 0, 255, 255);
	}else if(style & OVERLAY_TEXT_STYLE_ENCLOSED_WHRITE){
		is_enclosed = true;
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(enclosed_color, _def_alpha ^ 0x22, 255, 255, 255);
	}else if(style & OVERLAY_TEXT_STYLE_ENCLOSED_BLACK){
		is_enclosed = true;
		GK_ENC_VIDEO_OVERLAY_PIXEL_RGB(enclosed_color, _def_alpha ^ 0x22, 0, 0, 0);
	}

	// the font item stride bytes number, align 8bits / 1byte
	while('\0' != *ch)
	{
		if(*ch < 0x7f)  //英文
		{
			// ascii
			int const stride_bytes = (asc_width + 7) / 8;
			int const asc_code = *ch;

			// check width limited
			if(x + asc_width > canvas->width){
				break;
			}
			
			for(i = 0; i < font_size; ++i){
				int stride_pixel = 0;
				for(ii = 0; ii < stride_bytes && stride_pixel < asc_width; ++ii){
					off_t const offset_byte = asc_code * font_size * stride_bytes + i * stride_bytes + ii;
					for(iii = 0; iii < 8 && stride_pixel < asc_width; ++iii){
						uint8_t const actived_px = 0x80>>iii;//(1<<(8-1-iii));
				        //通过与运算来确定前景与背景色如果为１则为前景点阵，０则为背景点阵	
						if((kOVERLAY_FONT_SIZE_16 == font_size && (_asc_16[offset_byte] & actived_px))
							|| (kOVERLAY_FONT_SIZE_20 == font_size && (_asc_20[offset_byte] & actived_px))
							|| (kOVERLAY_FONT_SIZE_24 == font_size && (_asc_24[offset_byte] & actived_px))
							|| (kOVERLAY_FONT_SIZE_32 == font_size && (_asc_32[offset_byte] & actived_px))){
							canvas->put_pixel(canvas, x + ii * 8 + iii, y + i, fg_color);
						}else{
							canvas->put_pixel(canvas, x + ii * 8 + iii, y + i, bg_color);
						}
						++stride_pixel;
					}
				}
			}
			x += asc_width;
			ch += 1;
		}
		else if(*ch > 0xa0) //中文
		{
			int const stride_bytes = (gb2312_width + 7) / 8;
			// get qu code and wei code
			int const qu_code = ch[0] - 0xa0 - 1; // 87
			int const wei_code = ch[1] - 0xa0 - 1; // 94

			if(6 == qu_code){
				// russian
			}
			else
			{
				for(i = 0; i < font_size; ++i)
				{
					int stride_pixel = 0;
					for(ii = 0; ii < stride_bytes && stride_pixel < gb2312_width; ++ii)
					{
						off_t const offset_byte = (qu_code * 94 + wei_code) * font_size * stride_bytes + i * stride_bytes + ii;
						for(iii = 0; iii < 8 && stride_pixel < gb2312_width; ++iii)
						{
							uint8_t const actived_px = (0x80>>iii);
						    //方法 如英文雷同	
							if((kOVERLAY_FONT_SIZE_16 == font_size && (_gb2312_16[offset_byte] & actived_px))
								|| (kOVERLAY_FONT_SIZE_20 == font_size && (_gb2312_20[offset_byte] & actived_px))
								|| (kOVERLAY_FONT_SIZE_24 == font_size && (_gb2312_24[offset_byte] & actived_px))
								|| (kOVERLAY_FONT_SIZE_32 == font_size && (_gb2312_32[offset_byte] & actived_px))){
								canvas->put_pixel(canvas, x + ii * 8 + iii, y + i, fg_color);
							}else{
								canvas->put_pixel(canvas, x + ii * 8 + iii, y + i, bg_color);
							}
							++stride_pixel;
						}
					}
				}
			}
			x += gb2312_width;
			ch += 2;
		}
	}

	text_width = x - x_base;
    //这里需要关闭一个什么顔色？暂时未明白
	if(is_enclosed){
		for(i = 0; i < text_height - 1 ; ++i){
			for(ii = 0; ii < text_width - 1 ; ++ii){
				ST_GK_ENC_VIDEO_OVERLAY_PIXEL center_color;
				canvas->get_pixel(canvas, x_base + ii, y_base + i, &center_color);
				if(0 == i || text_height - 1 == i
					|| 0 == ii || text_width - 1 == ii){
					// on the edage
					if(canvas->match_pixel(canvas, fg_color, center_color)){
						canvas->put_pixel(canvas, x_base + ii, y_base + i, enclosed_color);
					}
				}else{
					ST_GK_ENC_VIDEO_OVERLAY_PIXEL right_color, bottom_color;
					
					canvas->get_pixel(canvas, x_base + ii + 1, y_base + i, &right_color); // to the right
					canvas->get_pixel(canvas, x_base + ii, y_base + i + 1, &bottom_color); // to the bottom

					if(canvas->match_pixel(canvas, bg_color, center_color)){
						// background
						if(canvas->match_pixel(canvas, fg_color, right_color)
							|| canvas->match_pixel(canvas, fg_color, bottom_color)){
							canvas->put_pixel(canvas, x_base + ii, y_base + i, enclosed_color);
						}
					}else if(canvas->match_pixel(canvas, fg_color, center_color)){
						// forground
						if(canvas->match_pixel(canvas, bg_color, right_color)){
							canvas->put_pixel(canvas, x_base + ii + 1, y_base + i, enclosed_color);
						}
						if(canvas->match_pixel(canvas, bg_color, bottom_color)){
							canvas->put_pixel(canvas, x_base + ii, y_base + i + 1, enclosed_color);
						}
					}
				}
			}
		}
	}
	
	return x - x_base; // write put size
}
static GK_ENC_OverlayParams* lookup_overlay_by_name(int stream, const char* name)
{
    int cnt;
    GK_ENC_OverlayParams *overlayTemp = NULL;

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) || (name == NULL))
    {
        SLOG(ERR,"lookup overlay by name: input invalid parameters.\n");
        return NULL;
    }

    for(cnt=0; cnt<OSD_AREA_NUM; cnt++)
    {
        overlayTemp =  &(overlays[stream][cnt]);
        if((overlayTemp->canva != NULL) && (strcmp(overlayTemp->name, name) == 0))
        {

            return overlayTemp;
        }
    }

    return NULL;
}
static inline uint32_t argb8888_to_uvya8888(uint32_t argb8888)
{
    uint32_t valu;
    GK_RGBColFormat rgb;
    GK_YUVColFormat yuv;

    rgb.argb8888 = argb8888;

	yuv.y = (u8)((257 * rgb.r + 504 * rgb.g + 98 * rgb.b + 16000)/1000);
	yuv.u = (u8)((439 * rgb.b - 291 * rgb.g - 148 * rgb.r + 128000)/1000);
	yuv.v = (u8)((439 * rgb.r - 368 * rgb.g - 71 * rgb.b + 128000)/1000);
	yuv.a = 0;
	if(rgb.a) {
		yuv.a = rgb.a | 0x80;
	}

    valu = ((yuv.a << 24) | (yuv.y << 16) | (yuv.u<< 8) | yuv.v);

    return valu;
}

//由于argb8888获取vuya色表地址的偏移
static int get_colour_index(GADI_OSD_AreaMappingT *areaMapping, uint32_t argb8888)
{
    int cnt,  maxColNum;
    uint32_t *clutAddr;
    uint32_t vuya8888;

    /*bgra888 to uvya8888*/
    vuya8888 = argb8888_to_uvya8888(argb8888);

    maxColNum = areaMapping->clutSize / sizeof(uint32_t);
    clutAddr  = (uint32_t*)areaMapping->clutStartAddr;

    /*find out the colour index. format: RGBA.*/
    for(cnt=0; cnt< maxColNum; cnt++)
    {
        /*match colour, return the colour index.*/
        if(vuya8888 == *(clutAddr + cnt))
        {
            return cnt;
        }
    }

    /*no this colour in CLUT, insert this colour in CLUT.*/
    /*colour 0 is 0x00000000.*/
    for(cnt=1; cnt< maxColNum; cnt++)
    {
        if(*(clutAddr + cnt) == 0)
        {
            /*insert colour in CLUT, return the colour index.*/
            *(clutAddr + cnt) = vuya8888;
            return cnt;
        }
    }

    SLOG(ERR,"No colour space, only support 256 colour!!!\n");

    return -1;
}
/*overlay*/
//创建一个像素缓存句柄
static PS_GK_ENC_VIDEO_OVERLAY gk_enc_create_overlay_canvas(size_t width,size_t height)
{
    int cnt = 0;
    PS_GK_ENC_VIDEO_OVERLAY  canvaTemp = NULL;

    /*check input parameters.*/
    if((width == 0) || (height == 0))
    {
        SLOG(ERR,"create overlay canvas input invalid parameters.\n");
        return NULL;
    }
    //编历这个全局数组来寻找当仍未使用的一个成员
    for(cnt = 0; cnt < GK7101_MAX_OVERLAY_NUMS; cnt++)
    {
        canvaTemp = &(overlayCanvas[cnt]);
        if((canvaTemp->width  == 0)  &&
           (canvaTemp->height == 0)  &&
           (canvaTemp->pixels == NULL))
        { //如果找到
            //为其属性，长，高赋值并对齐
            canvaTemp->width  = SDK_ALIGNED_BIG_ENDIAN(width, 2); // aligned to 2 pixel
            canvaTemp->height = SDK_ALIGNED_BIG_ENDIAN(height, 2);
            /*support 256 colour(8bit).*/
            //设置顔色屏bi数值来表示支持色位宽，，当前设为256因为０屏bi位
            canvaTemp->pixel_format.rmask = 0x00;
            canvaTemp->pixel_format.gmask = 0x00;
            canvaTemp->pixel_format.bmask = 0x00;
            canvaTemp->pixel_format.amask = 0x00;
            /*canva callback function.*/
            //为这个句柄挂载针对这个即将创建的buffer的一些操作方法
            canvaTemp->put_pixel   = overlay_canvas_put_pixel; //输入一个像素
            canvaTemp->get_pixel   = overlay_canvas_get_pixel; //获取一个像素
            canvaTemp->match_pixel = overlay_canvas_match_pixel; //匹配一个像素
            canvaTemp->put_rect    = overlay_canvas_put_rect; //输出一个区域
            canvaTemp->fill_rect   = overlay_canvas_fill_rect; //填充一个区域
            canvaTemp->erase_rect  = overlay_canvas_erase_rect; //擦除整个区域
            /*canva buffer.*/
            //创建buffer　＊４因为３２位色
            canvaTemp->pixels = calloc(canvaTemp->width * canvaTemp->height * \
                                        sizeof(uint32_t), 1);
            
            return canvaTemp;
        }
    }

    SLOG(ERR,"create canva failed: not free canva.\n");

    return NULL;
}

static PS_GK_ENC_VIDEO_OVERLAY gk_enc_get_overlay_canvas(int vin,int stream, const char* overlay_name)
{
    GK_ENC_OverlayParams* overlayTemp = NULL;

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) || (overlay_name == NULL))
    {
        SLOG(ERR,"get overlay canva: input invalid parameters.\n");
        return NULL;
    }

    overlayTemp = lookup_overlay_by_name(stream, overlay_name);
    if(overlayTemp ==  NULL)
    {
        SLOG(ERR,"overlay name:%s id:%d has not been create.\n", overlay_name, stream);
        return NULL;
    }

    return overlayTemp->canva;
}
static PS_GK_ENC_VIDEO_OVERLAY gk_enc_load_overlay_canvas(const char*bmp24_path)
{
    int ret = 0;
    int i = 0, j = 0, bmpWidth = 0, bmpHeight = 0;
	uint32_t dataSize = 0;
    char *lineOffset = NULL;
    FILE *bmpFd = NULL;
    char *canvas_cache = NULL;
    BIT_MAP_FILE_HEADER_t bmpHeader;

    PS_GK_ENC_VIDEO_OVERLAY canvas = NULL;
    ST_GK_ENC_VIDEO_OVERLAY_PIXEL canvas_pixel;

    /*check input parameters.*/
    if(bmp24_path == NULL)
    {
        SLOG(ERR,"load overlay canvas input invalid parameters.\n");
        return NULL;
    }

    SLOG(PRI,"load overlay canvas: fileName:%s.\n",bmp24_path);

    bmpFd = fopen(bmp24_path, "rb");

    if(bmpFd ==  NULL)
    {
        SLOG(ERR,"load overlay canvas: can not open fileName:%s.\n",bmp24_path);
        return NULL;
    }

    ret = fread(&bmpHeader, 1, sizeof(bmpHeader), bmpFd);

    if(sizeof(bmpHeader) != ret)
    {
        fclose(bmpFd);
        SLOG(ERR,"load overlay canvas: load fileName:%s error.\n",bmp24_path);
        return NULL;
    }

    if((bmpHeader.type[0] == 'B') && (bmpHeader.type[1] == 'M') &&
        ((bmpHeader.bit_count == 24) || (bmpHeader.bit_count == 32)))
    {
        SLOG(PRI,"IMAGE %dx%d size=%d offset=%d filesize=%d info=%d\n", \
                 bmpWidth, bmpHeight, bmpHeader.size_image, \
                 bmpHeader.off_bits, bmpHeader.file_size,\
                 bmpHeader.info_size);

        bmpWidth = bmpHeader.width;
        bmpHeight = bmpHeader.height;

        if(bmpHeader.size_image == 0)
        {
            dataSize = bmpHeader.file_size - bmpHeader.off_bits;
        }
        else
        {
            dataSize = bmpHeader.size_image;
        }
        canvas_cache = calloc(dataSize, 1);

        /*load image to buf.*/
        if(0 == fseek(bmpFd, bmpHeader.off_bits, SEEK_SET))
        {
            ret = fread(canvas_cache, 1, dataSize, bmpFd);
        }
        fclose(bmpFd);
        bmpFd = NULL;

        /*load to canvas.*/
        canvas = gk_enc_create_overlay_canvas(bmpWidth, bmpHeight);
        for(i = 0; i < bmpHeight; ++i)
        {
            if(bmpHeader.bit_count == 24)
            {
                lineOffset = canvas_cache + SDK_ALIGNED_BIG_ENDIAN(3 *
                                             bmpWidth, 4) * (bmpHeight - 1 - i) + 2;
                for(j = 0; j < bmpWidth; ++j)
                {
                    char *const column_offset = lineOffset + 3 * j;

                    canvas_pixel.alpha = 0xff;
                    canvas_pixel.red   = column_offset[0];
                    canvas_pixel.green = column_offset[1];
                    canvas_pixel.blue  = column_offset[2];

                    canvas->put_pixel(canvas, j, i, canvas_pixel);
                }
            }
            else/*32*/
            {
                lineOffset = canvas_cache + SDK_ALIGNED_BIG_ENDIAN(4 *
                            bmpWidth, 4) * (bmpHeight - 1 - i) + 2;
                for(j = 0; j < bmpWidth; ++j)
                {
                    char *const column_offset = lineOffset + 4 * j;

                    canvas_pixel.alpha = column_offset[0];
                    canvas_pixel.red = column_offset[1];
                    canvas_pixel.green = column_offset[2];
                    canvas_pixel.blue = column_offset[3];

                    canvas->put_pixel(canvas, j, i, canvas_pixel);
                }
            }
        }

        /*free the canvas cache.*/
        free(canvas_cache);
        canvas_cache = NULL;

        return canvas;
    }

    fclose(bmpFd);
    SLOG(ERR,"load overlay canvas: fileName:%s error.\n",bmp24_path);
    return NULL;
}

static void gk_enc_release_overlay_canvas(PS_GK_ENC_VIDEO_OVERLAY canvas)
{

    if(canvas)
    {
		canvas->width  = 0;
		canvas->height = 0;
		if(canvas->pixels)
        {
            /*thie canvas buffer alloc form memory, so it need to free in this API.*/
			free(canvas->pixels);
			canvas->pixels = NULL;
		}
	}
}

static int get_venc_width_height(int streamid, unsigned short* w, unsigned short *h){
    
    if(streamid == 0){
        *w =  1920;
        *h = 1080;

    }else if(streamid == 1){
        *w = 576;
        *h = 704;
    }else if(streamid == 3){
        *w = 1920;
        *h = 1080;

    }else
        *w = 1920;
        *h = 1080;

    return 0;
}

/*  创建ＯＳＤ块句柄　osd块句柄与像素缓存句柄的关系是：
 *  　
 *  osd块句柄　{
 *      内存映身的osd地址空间，色表地址间
 *      osd设置参数
 *      像素缓存句柄指针
 *  　}
 * 
 * 在这个函数里要实现 根据，保存参数，并对即将操作的空间做adi内存映射，并将其值保存下来。
 * */
static int gk_enc_create_overlay(int vin, int stream, const char* overlay_name,
    float x, float y, PS_GK_ENC_VIDEO_OVERLAY const canvas)
{
    int cnt = 0;
    int canvasX = 0, canvasY  = 0;
    unsigned short venc_w = 0, venc_h = 0;
    size_t canvasWidth = 0, canvasHeight = 0;
    GADI_OSD_AreaIndexT   areaIndex;  //空间区或参数结构体，主要是一个流及一个区哉ＩＤ
    GK_ENC_OverlayParams* overlayTemp = NULL;
    //gk7101_encode_stream_params* streamPar;

    SLOG(PRI,"gk_enc_create_overlay: Width=%d,Hight=%d,X=%f,Y=%f.\n",\
        canvas->width ,canvas->height, x, y);

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) ||
       (overlay_name == NULL) ||
       (canvas == NULL))
    {
        SLOG(ERR,"create overlay: input invalid parameters.\n");
        return -1;
    }
    //如果已创建
    if(lookup_overlay_by_name(stream, overlay_name) != NULL)
    {
        SLOG(ERR,"overlay name:%s has been used.\n", overlay_name);
        return -1;
    }
    //这个函数用来获取当前编码数的宽高以便计算实际x，y 座标偏移像素值
    get_venc_width_height(stream, &venc_w, &venc_h);
    /*alloc overlay area.
     *  cnt < 3*/
    for(cnt=0; cnt<OSD_AREA_NUM; cnt++)
    {
        overlayTemp =  &(overlays[stream][cnt]);
        if(overlayTemp->canva == NULL) //找到一个空闲区域
        {
            canvasX = (typeof(canvasX))(x * venc_w); //计算x,y实际坐标
            canvasY = (typeof(canvasY))(y * venc_h);
            if(canvas->width > venc_w)
            {   
                //如果像素宽大于了视频编码宽
                canvasWidth  = venc_w;
            }
            else
            {
                canvasWidth  = canvas->width;
            }

            if(canvas->height > venc_h)
            {
                canvasHeight  = venc_h;
            }
            else
            {
                canvasHeight  = canvas->height;
            }

            /*adjustment,最终计算x,y座标*/
            if(canvasY + canvasHeight > venc_h)
            {
                canvasY = venc_h - canvasHeight;
            }
            if(canvasX + canvasWidth > venc_w)
            {
                canvasX = venc_w - canvasWidth;
            }
            SLOG(PRI,"Width=%d,Hight=%d,X=%d,Y=%d, venc_h:%d, venc_w:%d.\n",canvasWidth,canvasHeight,\
                canvasX, canvasY, venc_w,venc_h);

            /*alignment*/
            //将最终确定的值做一个四字节对齐
            canvasX = SDK_ALIGNED_BIG_ENDIAN(canvasX, 4);
            canvasY = SDK_ALIGNED_BIG_ENDIAN(canvasY, 4);
            //32对齐是即于３２位字库的一个兼容
            canvasWidth  = SDK_ALIGNED_LITTLE_ENDIAN(canvasWidth, 32);
            canvasHeight = SDK_ALIGNED_BIG_ENDIAN(canvasHeight, 4);

            /*get overlay area addr.*/
            //给空间句柄赋值，用来映射参数
            areaIndex.areaId  = cnt;
            areaIndex.planeId = stream;
            if(gadi_osd_get_area_mapping(osdHandle, areaIndex, \
                                      &overlayTemp->areaMapping) != GADI_OK)
            {
                SLOG(ERR,"map overlay area stream:%d,area%d failed\n", stream, cnt);
                return -1;
            }
            //将数据保存下来
            overlayTemp->canva = canvas;
            overlayTemp->areaParams.planeId = stream;
            overlayTemp->areaParams.areaId  = cnt;
            overlayTemp->areaParams.width   = canvasWidth;
            overlayTemp->areaParams.height  = canvasHeight;
            overlayTemp->areaParams.offsetX = canvasX;
            overlayTemp->areaParams.offsetY = canvasY;
            /*set overlay disable when creating.*/
            overlayTemp->areaParams.enable = 0;


            SLOG(PRI,"overlay name:%s has been create. sream:%d\n",overlay_name,stream);
            SLOG(PRI,"Width=%d,Hight=%d,X=%d,Y=%d.\n",canvasWidth,canvasHeight,\
                canvasX, canvasY);

            strcpy(overlayTemp->name, overlay_name);

            return 0;
        }
    }

    SLOG(PRI,"there is no more overlay area, max number:%d\n",OSD_AREA_NUM);
    return -1;
}
//由于像素缓存创建的是一个全局数组，所有此处的释放只是将
//块句柄中挂载的像素缓存句柄置为ＮＵＬＬ,然后清空当前这个
//句柄的所有参数，这个句柄的参数包括：ＯＳＤ空间映射地址
//OSD设置参数及一个像素缓存句柄
//
static int gk_enc_release_overlay(int vin, int stream, const char* overlay_name)
{
    GK_ENC_OverlayParams* overlayTemp = NULL;

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) || (overlay_name == NULL))
    {
        SLOG(ERR,"release overlay: input invalid parameters.\n");
        return -1;
    }

    overlayTemp = lookup_overlay_by_name(stream, overlay_name);
    if(overlayTemp ==  NULL)
    {
        SLOG(ERR,"overlay name:%s has not been create.\n", overlay_name);
        return -1;
    }

    /*only clear the canvas is ok*/
    overlayTemp->canva = NULL;
    gadi_sys_memset(overlayTemp, 0, sizeof(GK_ENC_OverlayParams));

    return 0;
}

static int gk_enc_show_overlay(int vin, int stream, const char* overlayName,
                                                                  bool showFlag)
{
    GK_ENC_OverlayParams* overlayTemp = NULL;
    GADI_OSD_AreaParamsT* areaParams = NULL;

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) || (overlayName == NULL))
    {
        SLOG(ERR,"show overlay: input invalid parameters.\n");
        return -1;
    }

    overlayTemp = lookup_overlay_by_name(stream, overlayName);
    if(overlayTemp ==  NULL)
    {
        SLOG(ERR,"overlay name:%s has not been create.\n", overlayName);
        return -1;
    }

    areaParams = &(overlayTemp->areaParams);
    areaParams->enable = showFlag;
    if(gadi_osd_set_area_params(osdHandle, areaParams) != GADI_OK)
    {
        SLOG(ERR,"show overlay name:%s failed.\n", overlayName);
        printf("planedId:%d, areaId:%d, width:%d, height:%d, offsetX:%d, offsetY:%d\n", 
                areaParams->planeId, areaParams->areaId, areaParams->width, areaParams->height, areaParams->offsetX, areaParams->offsetY);
        return -1;
    }

    return 0;
}
//刷新块空间,主要过程就是根据像素缓存的数据，重写ＯＳＤ空间的顔色表
static int gk_enc_update_overlay(int vin, int stream, const char* overlay_name)
{
    int      retVal;
    uint8_t  colIndex;
    uint32_t argb8888;
    uint32_t pixelCnt, cnt, ignCnt, offset, i;
    uint32_t *pixelAddr   = NULL;
    uint8_t  *overlayAddr = NULL;
    GK_ENC_OverlayParams* overlayTemp = NULL;
    uint32_t colArgb[GK7101_CLUT_MAX_COLOUR_NUM] = {0};
    uint8_t  colTab[GK7101_CLUT_MAX_COLOUR_NUM] = {0};

    /*check input parameters.*/
    if((stream > MAX_VENC_CHN_NUM_QQZM_TEST) || (overlay_name == NULL))
    {
        SLOG(ERR,"show overlay: input invalid parameters.\n");
        return -1;
    }

    overlayTemp = lookup_overlay_by_name(stream, overlay_name);
    if(overlayTemp ==  NULL)
    {
        SLOG(ERR,"overlay name:%s has not been create.\n", overlay_name);
        return -1;
    }
    //把要操作的空间先保存下来，
	pixelAddr   = (uint32_t*)overlayTemp->canva->pixels;
    overlayAddr = (uint8_t*)overlayTemp->areaMapping.areaStartAddr;
    //计算像素个数
    pixelCnt = overlayTemp->canva->width * overlayTemp->canva->height;
    //test
   // memset(overlayAddr,0x1,pixelCnt);
    
    //return 0;

    /*write CLUT & overlay buffer.*/
    offset = 0;
    ignCnt = 0;
    for(cnt=0; cnt<pixelCnt; )
    {
        //第一步是取出像数,当ＯＳＤ块空间大于，像素缓存空间就
        if(overlayTemp->areaParams.width >= overlayTemp->canva->width)
        {
            argb8888 = *(pixelAddr + cnt);
            cnt++;
        }
        else//小于就。。栽掉一部份内容。
        {
            argb8888 = *(pixelAddr + cnt);
            /*ignore the data cross.*/
            if(ignCnt == overlayTemp->areaParams.width) //对齐栽掉
            {
                cnt += overlayTemp->canva->width - overlayTemp->areaParams.width;
                ignCnt = 0;
                continue;
            }
            ignCnt++;
            cnt++;
        }

        /*look out the col table.*/
        colIndex = 0x00;
        for(i=0; i<GK7101_CLUT_MAX_COLOUR_NUM; i++)
        {
            if(argb8888 == colArgb[i])
            {
                colIndex = colTab[i]; //如果量0,即黑色,这情况不需２５６循环
                break;
            }
            //如果第一位不是０就进空间获取色表偏移，黑色就是０偏移位
            if((colArgb[i] == 0x00000000) && (colTab[i] == 0x00) && (i != 0))
            {
                retVal = get_colour_index(&(overlayTemp->areaMapping), argb8888);
                if(retVal < 0 )
                {
                    colIndex = 0x00;
                    break;
                }
                else
                {
                    colTab[i]  = retVal;
                    colArgb[i] = argb8888;
                    colIndex   = retVal;
                    break;
                }
            }
        }
        *(overlayAddr + offset) = colIndex;  //此处存放的仅是色表地址偏移位
        offset++;
    }

    return 0;
}
static int overlay_canvas_put_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
                              int x, int y, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel)
{
    uint32_t *pixelAddr = NULL;

    /*check input parameters.*/
    if((canvas == NULL) || ((canvas->pixels == NULL)) ||
       ((x >= canvas->width) && (y >= canvas->height)))
    {
        SLOG(PRI,"canvas get pixel: input invalid parameters.\n");
        return -1;
    }

	pixelAddr = (uint32_t*)canvas->pixels;

	*(pixelAddr + canvas->width * y + x) = pixel.argb8888;

	return 0;

}
static int overlay_canvas_get_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
                         int x, int y, ST_GK_ENC_VIDEO_OVERLAY_PIXEL* ret_pixel)
{
    uint32_t *pixelAddr = NULL;
    uint32_t argb8888;

    /*check input parameters.*/
    if((canvas == NULL) || (canvas->pixels == NULL) || (ret_pixel == NULL) ||
       ((x >= canvas->width) && (y >= canvas->height)))
    {
        SLOG(PRI,"canvas get pixel: input invalid parameters.\n");
        return -1;
    }

    pixelAddr  = (uint32_t*)canvas->pixels;
    argb8888   = *(pixelAddr + canvas->width * y + x);
    ret_pixel->argb8888  = argb8888;
    return 0;
}

static bool overlay_canvas_match_pixel(PS_GK_ENC_VIDEO_OVERLAY canvas,
     ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel1, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel2)
{
    return (pixel1.argb8888 == pixel2.argb8888);
}

static int overlay_canvas_put_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
  int x, int y, size_t width, size_t height,ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel)
{
    uint32_t *pixelAddr = NULL;
    uint32_t *pixelPos = NULL;
    uint32_t argb8888;
    int i, j;

    /*check input parameters.*/
    if((canvas == NULL) || ((canvas->pixels == NULL)) ||
       ((x >= canvas->width) && (y >= canvas->height)))
    {
        SLOG(PRI,"canvas put rect: input invalid parameters.\n");
        return -1;
    }

    if(x + width >= canvas->width)
    {
        width = canvas->width - x;
    }
    if(y + height >= canvas->height)
    {
        height = canvas->height - y;
    }

    argb8888 = pixel.argb8888;

    /*get the pixel start addr.*/
	pixelAddr = ((uint32_t*)canvas->pixels) + canvas->width * y + x;

	for(i = 0; i < height; ++i)
    {
    	pixelPos = pixelAddr + i * canvas->width;
    	if((i == 0) || (i == height - 1))
        {
    		/*put one line.*/
    		for(j = 0; j < width; ++j)
            {
    			*pixelPos = argb8888;
    		}
    	}
        else
        {
    		/*put 2 dots.*/
    		pixelPos[0] = pixelPos[width - 1] = argb8888;
    	}
    }

    return 0;
}

static int overlay_canvas_fill_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
 int x, int y, size_t width, size_t height, ST_GK_ENC_VIDEO_OVERLAY_PIXEL pixel)
{
    uint32_t *pixelAddr = NULL;
    uint32_t *pixelPos = NULL;
    uint32_t argb8888;
    int i, j;


    /*check input parameters.*/
    if((canvas == NULL) || ((canvas->pixels == NULL)) ||
       ((x >= canvas->width) && (y >= canvas->height)))
    {
        SLOG(PRI,"canvas fill rect: input invalid parameters.\n");
        return -1;
    }

    if(x + width >= canvas->width)
    {
        width = canvas->width - x;
    }
    if(y + height >= canvas->height)
    {
        height = canvas->height - y;
    }

    argb8888 = pixel.argb8888;

    /*get the pixel start addr.*/
	pixelAddr = ((uint32_t*)canvas->pixels) + canvas->width * y + x;

    for(i = 0; i < height; ++i)
    {
        pixelPos = pixelAddr + i * canvas->width;
        for(j = 0; j < width; ++j)
        {
            *pixelPos++ = argb8888;
        }
    }

    return 0;
}

static int overlay_canvas_erase_rect(PS_GK_ENC_VIDEO_OVERLAY canvas,
                                      int x, int y, size_t width, size_t height)
{
    uint32_t *pixelAddr = NULL;
    uint32_t *pixelPos = NULL;
    int i, j;

    /*check input parameters.*/
    if((canvas == NULL) || ((canvas->pixels == NULL)) ||
       ((x >= canvas->width) && (y >= canvas->height)))
    {
        SLOG(PRI,"canvas erase rect: input invalid parameters.\n");
        return -1;
    }

    if(x + width >= canvas->width)
    {
        width = canvas->width - x;
    }
    if(y + height >= canvas->height)
    {
        height = canvas->height - y;
    }

    /*get the pixel start addr.*/
	pixelAddr = (uint32_t*)canvas->pixels + canvas->width * y + x;

    for(i = 0; i < height; ++i)
    {
        pixelPos = pixelAddr + i * canvas->width;
        for(j = 0; j < width; ++j)
        {
            *pixelPos++ = 0x00000000;
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////

