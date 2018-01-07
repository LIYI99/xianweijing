#include "object_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define     USEING_STATE    1
#define     FREE_STATE      0
     
object_pool_t*  object_pool_create(unsigned int object_size,unsigned int max,
        object_node_init func)
{
    
    
    if(object_size <= 0 || max <= 0)
        return NULL;
    
    object_pool_t *s = NULL;

    unsigned node_size = sizeof(struct qnode)+ object_size;
    s = (object_pool_t *)malloc(node_size*max +sizeof(object_pool_t));

    if(s == NULL)
        return NULL;
    memset(s,0x0,node_size*max+sizeof(object_pool_t));

    s->max =  max;
    s->pool = (void *)((void *)s + sizeof(object_pool_t));





    int i = 0;
    struct qnode *nodep = NULL,*temp =NULL;

    void *datap  = NULL;

    //int and pop data node in queue
    for( i = 0;  i < max ;i++){
        nodep =  (struct qnode *)(s->pool + node_size*i);

        datap = ((void *)nodep) + sizeof(struct qnode);
        if(func){
            func(datap);
        }
        if(s->head == NULL){
            s->head = nodep;
            s->end = nodep;
            s->head->next = nodep;
            //s->end->prev = nodep;
        }else{
            temp = s->head;
            s->head = nodep;
            s->head->next = temp;
            temp->prev = s->head;
        }
    }
    s->_init = func;
    s->object_size = object_size; 
    return  s;
}

void    object_pool_destory(object_pool_t *s){
    
    if(s != NULL)
        free(s);
    return;

}
//get
void*   object_pool_get(object_pool_t *s){
    

    //not have node 
   if(s->head == NULL && s->end == NULL)
        return NULL;
   
   struct qnode *node = node =  s->end;
    //only one
    if(s->end == s->head){
        s->end  = NULL;
        s->head = NULL;
    }else{
        s->end = node->prev;
        node->prev->next = NULL;
    }
    node->tab = USEING_STATE  ; //use
    s->usecnts++;
    
    void *p = ((void *)node) + sizeof(struct qnode);
    
    //memset(p,0x0,s->object_size) ;
    return  p;
    //((void *)node) + sizeof(struct qnode);

}


//free
void   object_pool_free(object_pool_t *s,void *p){
    
       
    if(s->_init){
        s->_init(p);
    }

    struct qnode  *node = NULL,*temp = NULL;
    node = (struct qnode *) ( p  - sizeof(struct qnode));
    
    //freed
    if(node->tab != USEING_STATE)
        return;
    //not node 
    if(s->end == NULL  && s->head == NULL){
        s->head = s->end = node;
        s->end->prev =  node;
    }else{
        temp = s->head;
        s->head = node;
        node->next = temp;
        temp->prev = node;
    }
    
    node->tab = FREE_STATE;
    s->usecnts--;

    return ;

}

void    object_pool_deinfo(object_pool_t *s,object_node_init func){
    
    unsigned int k = 0;
    struct qnode    *node = NULL;
    void *p = NULL;


    for(k = 0 ; k < s->max ; k++)
    {
        p = (s->pool+(k*s->object_size));
        node  =  (struct qnode *)p;
        printf("node sate:%d\n",node->tab);
        p = p+sizeof(struct qnode);
        func(p);
    }
    return ;


}

int     object_pool_reset(object_pool_t *s)
{
    if(!s)
        return -1;
    s->usecnts = 0;
    

    //reset mem
    memset(s->pool,0x0,(sizeof(struct qnode)+s->object_size)*s->max);

     //init and pop data node in queue
    int i = 0;
    struct qnode *nodep = NULL,*temp =NULL;
    void *datap  = NULL;

       for( i = 0;  i < s->max ;i++){
        nodep =  (struct qnode *)(s->pool + s->object_size*i);

        datap = ((void *)nodep) + sizeof(struct qnode);
        if(s->_init){
            s->_init(datap);
        }
        if(s->head == NULL){
            s->head = nodep;
            s->end = nodep;
            s->head->next = nodep;
            //s->end->prev = nodep;
        }else{
            temp = s->head;
            s->head = nodep;
            s->head->next = temp;
            temp->prev = s->head;
        }
    }

    return 0;
}


