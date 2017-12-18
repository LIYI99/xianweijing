#ifndef __XW_PNG_LOAD_H__
#define __XW_PNG_LOAD_H__
#include<stdint.h>

int    xw_png_load_all(void);
void   xw_png_destory(void);

uint16_t *xw_get_window_png(char *window_id);
void  xw_get_png_hw(char *window_id,uint16_t *w,uint16_t *h);



#endif
