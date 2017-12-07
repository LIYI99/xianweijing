#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "xw_png_load.h"
#include "image_png_put.h"
#include "xw_window_id_df.h"
#define  PNG_LOAD_ALIGN             4

typedef struct  xw_window_png{
    char        window_id[10];
    uint32_t    w;
    uint32_t    h;
    uint16_t    *p;
}xw_window_png_t;

static xw_window_png_t     *png_t = NULL;
static uint16_t            *offset_p = NULL;
static uint16_t            png_cnts = 0 ;

static int   xw_window_png_init(int all_size,  int nums_window)

{
    
    if(png_t != NULL || nums_window <= 0 )
        return -1;
    png_t  = (xw_window_png_t *)malloc(sizeof(xw_window_png_t) *nums_window + all_size);
    if(png_t == NULL)
        return -2;

    memset(png_t,0x0,sizeof(xw_window_png_t)* nums_window);
    offset_p = (uint16_t *)(((char *)png_t) + sizeof(xw_window_png_t)*nums_window) ;   
    
    return 0;  

}

static int xw_window_png_add(char *png_path,char *window_id)
{

    if(png_path == NULL || window_id == NULL)
        return -1;
    
    int ret  = 0;
    ret =   image_png_load_rgba_16bit(png_path,offset_p,&(png_t[png_cnts].h),
                &(png_t[png_cnts].w));
    if(ret < 0){
       printf("%s %d ret:%d\n",__func__,__LINE__,ret);
        return -2;
    }
    printf("get window_id:%s png image  h:%d w:%d size:%d\n ",window_id,png_t[png_cnts].h,
            png_t[png_cnts].w,ret); 
    strcpy(png_t[png_cnts].window_id,window_id);
    png_t[png_cnts].p = offset_p;
    offset_p += ret;
    png_cnts++;
    return 0;

}

#define     PNG_MEM_ALL_SIZE    1024 * 1024
#define     WINDOW_PNG_NUMS     100


#define     WINDOW_TOP_MENU_PATH    "/usr/local/bin/png/top.png"
#define     WINDOW_MAIN_MENU_PATH   "/usr/local/bin/png/main.png"


int     xw_png_load_all(void)
{
    int ret = 0;
    ret = xw_window_png_init(PNG_MEM_ALL_SIZE,  WINDOW_PNG_NUMS);
    if(ret != 0)
        return -1;
    //add main menue
    ret = xw_window_png_add(WINDOW_MAIN_MENU_PATH,XW_MAIN_WINDOW_ID );
    //add top menu
    ret = xw_window_png_add(WINDOW_TOP_MENU_PATH,XW_TOP_MENU_WINDOW_ID);


    return ret;

}

void   xw_png_destory(void)
{
    
    if(png_t)
        free(png_t);
    png_t = NULL;
    return;

}



uint16_t *get_window_png_mem(char *window_id)
{
    int i  =0 ;
    for(i = 0; i < png_cnts ; i++){
    
        if(strcmp(window_id,png_t[i].window_id) == 0)
            return png_t[i].p;
    }
    return NULL;
}



