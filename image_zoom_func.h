#ifndef __IMAGE_ZOOM_FUNC_H__
#define __IMAGE_ZOOM_FUNC_H__
#include<stdint.h>

typedef struct image_zoom{
    uint16_t    inwidth;
    uint16_t    inheight;
    uint16_t    *inbuf;
    uint16_t    outwidth;
    uint16_t    outheight;
    uint16_t    *outbuf;
    

}image_zoom_t;

int     iamge_zoom_func(image_zoom_t *data);


#endif
