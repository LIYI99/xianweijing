#include <stdlib.h>
//#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "image_xbuddy_mempool.h"



struct buddy_mem_s{
    unsigned  int nums;
    unsigned  int block_size;
    unsigned  int buf_size;
    void*             p;
    void*             end;
    unsigned  int longest[1]; 
};

#define     LEFT_LEAF(index) ((index) * 2 + 1)
#define     RIGHT_LEAF(index) ((index) * 2 + 2)
#define     PARENT(index) ( ((index) + 1) / 2 - 1)

#define     IS_POWER_OF_S(x) (!((x)&((x)-1)))
#define     MAX(a, b) ((a) > (b) ? (a) : (b))


static unsigned int fixsize(unsigned int size) {
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size+1;
}

buddymem_t* buddymem_create( int block_size,int nums )
{

    buddymem_t* s = NULL;
    unsigned int    node_size = 0,k = 0;
    if( !IS_POWER_OF_S(nums)){
        for(;nums > 1;k++){
            nums = nums >>1;
        }
        nums = 1;
        nums = nums << k;
 //       printf("nums is not 2x ,now change nums is:%d \n",nums);
    }

    if(!IS_POWER_OF_S(block_size)){
        block_size = fixsize(block_size);
     //   printf("block_size:%d nums:%d\n",block_size,nums);
    }



    if( nums < 1 || block_size < sizeof(int) || !IS_POWER_OF_S(nums))
        return NULL;

    s =  (buddymem_t *)malloc(sizeof(buddymem_t) + 
            nums*2*sizeof(unsigned int ) + block_size*nums);
    if(s == NULL)
        return NULL;

    memset(s,0x0,sizeof(buddymem_t));
    node_size = block_size *nums*2;

    s->nums = nums;
    s->block_size = block_size;
    s->buf_size = block_size*nums;

    s->p = (void *)s + sizeof(buddymem_t) + sizeof(unsigned int )*nums*2;
    s->end = s->p + s->buf_size;

    //printf("s->p :%p sizeof(unsigned int ):%d\n",s->p,sizeof(unsigned int )); 
    int i;
    for(i = 0; i < 2*nums -1 ; i++){
        if(IS_POWER_OF_S(i + 1))
            node_size /= 2;
        s->longest[i] = node_size;
        //printf("index:%d node_size:%d\n",i,node_size);
    }

    return s;

}

void    buddymem_destroy( buddymem_t*  s)
{

    if(s)
        free(s);
    return;

}
void*   buddymem_alloc(buddymem_t *s, int size)
{


    if(size <= 0)
        return NULL;

    unsigned int index = 0,node_size = 0,offset = 0;

    if(s->longest[index] < size)
        return NULL;
    
    if(size < s->block_size)
        size = s->block_size;
    else if(!IS_POWER_OF_S(size))
        size = fixsize(size);
    else 
        size = s->block_size;

   
    node_size =  s->buf_size ;

    for( ; node_size != size; node_size /= 2 )
    {
        if (s->longest[LEFT_LEAF(index)] >= size)
            index = LEFT_LEAF(index);
        else
            index = RIGHT_LEAF(index);
    }
    s->longest[index] = 0;
    offset = (index + 1) * node_size - ( s->buf_size);
    while(index) 
    {
        index = PARENT(index);
        s->longest[index] = 
            MAX(s->longest[LEFT_LEAF(index)], s->longest[RIGHT_LEAF(index)]);
    }

    return (void *)(s->p + offset);

}


void    buddymem_free(buddymem_t* s, void *p){

    if(s == NULL || p == NULL)
        return ;
    if(p > s->end || p < s->p)
        return;

    unsigned int index = 0, offset = 0,node_size = s->block_size;

    offset = p - s->p;
    index = offset /s->block_size + s->nums -1;

    for (; s->longest[index] ; index = PARENT(index)) {
        node_size *= 2;
        if (index == 0)
            return;
    }
    s->longest[index] = node_size;
    unsigned int left_longest = 0,right_longest = 0;
    while ( index ) 
    {
        index = PARENT(index);
        node_size *= 2;
        left_longest = s->longest[LEFT_LEAF(index)];
        right_longest = s->longest[RIGHT_LEAF(index)];
        if (left_longest + right_longest == node_size) 
            s->longest[index] = node_size;
        else
            s->longest[index] = MAX(left_longest, right_longest);
    }

    return;
}


int     buddymem_size(buddymem_t* s, void *p)
{

    if(s == NULL || p == NULL)
        return 0;
    if(p > s->p + s->block_size*s->nums)
        return 0;
    if(p < s->p)
        return 0;

    unsigned int node_size = s->block_size;
    unsigned int index = 0,offset = 0;

    offset  = (p - s->p)/s->block_size ;
    for (index = offset + s->nums - 1; s->longest[index] ; index = PARENT(index))
        node_size *= 2;

    return  node_size;
}


void    buddymem_dump(buddymem_t* s){

    return ;


}


