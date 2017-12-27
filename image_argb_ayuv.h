
#ifndef __IMAGE_ARGB_AYUV_H__
#define __IMAGE_ARGB_AYUV_H__
#include <stdint.h>

#define     ARGB4444_AYUV4444   1
#define     RGBA4444_AYUV4444   2
#define     RGBA8888_AYUV4444   3
#define     AGBR8888_AYUV4444   4
#define     RGB565_YUV565       5



    int  image_ayuv_talbe_init(int  format);
    void  inline  image_argb4444_to_ayuv(uint16_t    argb, uint16_t *ayuv);
    void   inline  image_rgba8888_to_ayuv(uint32_t argb,uint16_t *ayuv);
    void image_ayuv_talbe_deinit(void);


#endif



