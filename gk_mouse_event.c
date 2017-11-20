#include "gk_mouse_event.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MOUSE_STEP      4
#define DEBUG           1

static  GK_MOUSE_DATA  s_event;
static  GK_MOUSE_T     mouse_t;
int  gk_mouse_open(char *path,unsigned int scree_w,unsigned int scree_h,unsigned int speed,unsigned int 
        mouse_size)
{
    if(path == NULL)
        return -1;
   
    if(scree_w <= 0 || scree_h <= 0)
        return -1;
 
    if(speed < 0 || speed > 30 )
        speed = 1;
    memset(&mouse_t,0x0,sizeof(GK_MOUSE_T));
    
    mouse_t.scree_h = scree_h;
    mouse_t.scree_w = scree_w;

    mouse_t.mouse_step_h = speed * MOUSE_STEP;
    mouse_t.mouse_step_w = mouse_t.mouse_step_h *mouse_t.scree_w / mouse_t.scree_h;
    mouse_t.speed = speed;
    mouse_t.mouse_size = mouse_size;
    s_event.x = 0;
    s_event.y = 0;
    s_event.event = GK_MOUSE_OFFSET;

    int fd;
    fd = open(path,O_RDONLY);
    if(fd < 0 )
        return -1;
#ifdef DEBUG
    printf("screew:%d scree_h:%d speed_w:%d mouse speed_h:%d dev path:%s\n",
            mouse_t.scree_w,mouse_t.scree_h,mouse_t.mouse_step_w ,mouse_t.mouse_step_h,path);
#endif
    mouse_t.fd = fd;

    return fd;

}

void gk_mouse_close(int fd){
    
    if(fd <= 0)
        return;
    close(fd);
    memset(&mouse_t,0x0 ,sizeof(GK_MOUSE_T));
    return;

}

int  gk_mouse_read_data(int fd,GK_MOUSE_DATA *data)
{

    
    
    int re;
    char buf[120];
    memset(buf,0x0,120);
    re = read(fd,buf,120);
    if(re < 0 )
        return -1;
    if(re > 3)
        printf("get mouse data :%d\n",re);

    GK_MOUSE_DATA  sevent;
    
    uint8_t  type,x,y,z;
    
    type = buf[0]&0x7;
    
    if( buf[1] > 0 ){
        data->x += (128 > buf[1]) ? mouse_t.mouse_step_w : ( 0 - mouse_t.mouse_step_w); 
    }
    
    if( buf[2] > 0 ){
        data->y += (128 > buf[2]) ? (0 - mouse_t.mouse_step_h ) : ( mouse_t.mouse_step_h);
    }

    if(buf[3] > 0){
        ;//data->z += (10 > buf[3]) ? mouse_t.mouse_step_w : ( 0 - mouse_t.mouse_step_w);
    }
    
    data->x = (1 > data->x) ? 0 : data->x;
    data->y = (1 > data->y) ? 0 : data->y;
    
    data->x = ((mouse_t.scree_w - mouse_t.mouse_size) <= 
            data->x) ? (mouse_t.scree_w - mouse_t.mouse_size) : data->x;
    
    data->y = ((mouse_t.scree_h - mouse_t.mouse_size) <= 
            data->y) ? (mouse_t.scree_h - mouse_t.mouse_size) : data->y;
    
    if( type == 0)
    {
        if(s_event.event == GK_MOUSE_OFFSET)
            data->event = GK_MOUSE_OFFSET;
        else if(s_event.event == GK_MOUSE_LEFT_DOWN)
            data->event = GK_MOUSE_LEFT_UP;
        else if(s_event.event == GK_MOUSE_RIGHT_DOWN)
            data->event = GK_MOUSE_RIGHT_UP;
        else
            data->event = GK_MOUSE_OFFSET;
    
    }else if (type == 1){
       data->event = GK_MOUSE_LEFT_DOWN;
    }else if (type == 2){
        
        data->event = GK_MOUSE_RIGHT_DOWN;
    }
    s_event =  *data;
#ifdef DEBUG_
    printf(" sencod:events type:%d x:%d y:%d  x->bu[1]:%d,y->buf[2]\n",data->event,data->x,data->y,buf[1],buf[2]);
#endif
    return  0 ;

}

