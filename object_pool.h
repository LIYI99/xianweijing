#ifndef __OBJECT_POOL_H__
#define __OBJECT_POOL_H__

#include <pthread.h>

#ifdef __cplusplus
    extern "C"{
#endif



typedef struct object_pool_s object_pool_t;
typedef void (*object_node_init)(void *data);

struct  qnode {
    unsigned char tab; 
    struct qnode    *prev;
    struct qnode    *next;
};

struct object_pool_s {
    unsigned int        object_size;
    unsigned int        max;
    unsigned int        usecnts;
    object_node_init    _init;

    struct  qnode       *head;
    struct  qnode       *end;
    void                *pool;

};



object_pool_t*  object_pool_create(unsigned int object_size,unsigned int max,
        object_node_init func);
void    object_pool_destory(object_pool_t *s);
void*   object_pool_get(object_pool_t *s);
void   object_pool_free(object_pool_t *s,void* p);

void    object_pool_deinfo(object_pool_t *s,object_node_init func);




#ifdef __cplusplus
}
#endif




#endif
