#ifndef __IMAGE_PNG_PUT_H__
#define __IMAGE_PNG_PUT_H__

#include <stdint.h>


#ifdef  __cplusplus
    extern  "C"{
#endif
    
    


int  image_png_load_rgba_16bit(char *path,uint16_t *mem,uint32_t *h,uint32_t *w,uint8_t flags);




#ifdef  __cplusplus
}
#endif


#endif

