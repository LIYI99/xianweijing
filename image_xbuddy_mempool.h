#ifndef __IMAGE_XBUDDY_MEMPOOL_H__
#define __IMAGE_XBUDDY_MEMPOOL_H__

#include <stdlib.h>

#ifdef __cplusplus
    extern "C"{
#endif


typedef struct buddy_mem_s     buddymem_t;
buddymem_t*    buddymem_create(int block_size,int nums);
void    buddymem_destroy( buddymem_t*  s);
void*   buddymem_alloc(buddymem_t *s, int size);
void    buddymem_free(buddymem_t* s, void *p);
int     buddymem_size(buddymem_t* s, void *p);
void    buddymem_dump(buddymem_t* s);


#ifdef __cplusplus
}
#endif

#endif


