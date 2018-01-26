#ifndef __XW_PNG_LOAD_H__
#define __XW_PNG_LOAD_H__
#include<stdint.h>

#define     VRCEE_VOUT_MODE_1080    0       //1920* 1080
#define     VRCEE_VOUT_MODE_600     1       //1024*600

#define     MOUSE_HANDLE_ID         "mouse_h"
#define     MOUSE_ID                "mouse_i"





int    xw_png_load_all(int mode);
void   xw_png_destory(void);

uint16_t *xw_get_window_png(char *window_id);
void  xw_get_png_hw(char *window_id,uint16_t *w,uint16_t *h);
void  xw_color_change_func(uint16_t *image,uint16_t size,uint16_t src_color,uint16_t det_color);





#endif
