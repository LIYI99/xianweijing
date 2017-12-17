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

#define     PNG_MEM_ALL_SIZE    1024 * 1024*2
#define     WINDOW_PNG_NUMS     100


//top menu
#define     WINDOW_TOP_MENU_PATH        "/usr/local/bin/png/top.png"
//main menu
#define     WINDOW_MAIN_MENU_PATH       "/usr/local/bin/png/main.png"

//line manger
#define     WINDOW_LINE_MANGER_CHOICEA    "/usr/local/bin/png/load_a.png"
#define     WINDOW_LINE_MANGER_CHOICEB   "/usr/local/bin/png/load_b.png"
#define     WINDOW_LINE_MANGER_CHOICEC    "/usr/local/bin/png/load_c.png"
#define     WINDOW_LINE_MANGER_CHOICED    "/usr/local/bin/png/load_d.png"
#define     WINDOW_LINE_MANGER_CHOICEE    "/usr/local/bin/png/load_e.png"
#define     WINDOW_LINE_MANGER_CHOICEF    "/usr/local/bin/png/load_f.png"
#define     WINDOW_LINE_MANGER_CHOICEG    "/usr/local/bin/png/load_g.png"
#define     WINDOW_LINE_MANGER_CHOICEH    "/usr/local/bin/png/load_h.png"
//line size set
#define     WINDOW_LINE_MANGER_SETSIZEA    "/usr/local/bin/png/size1.png"
#define     WINDOW_LINE_MANGER_SETSIZEB    "/usr/local/bin/png/size2.png"
#define     WINDOW_LINE_MANGER_SETSIZEC    "/usr/local/bin/png/size3.png"
#define     WINDOW_LINE_MANGER_SETSIZED    "/usr/local/bin/png/size4.png"
//line color set
#define     WINDOW_LINE_MANGER_SETCOLORR    "/usr/local/bin/png/read.png"
#define     WINDOW_LINE_MANGER_SETCOLORG    "/usr/local/bin/png/green.png"
#define     WINDOW_LINE_MANGER_SETCOLORB    "/usr/local/bin/png/bule.png"
#define     WINDOW_LINE_MANGER_SETCOLORC    "/usr/local/bin/png/cyan.png"
#define     WINDOW_LINE_MANGER_SETCOLORY    "/usr/local/bin/png/yello.png"
#define     WINDOW_LINE_MANGER_SETCOLORV    "/usr/local/bin/png/violet.png"

















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
    
    char id[10];
    //load line arry set
    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'1');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEA, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'2');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEB, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'3');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEC, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'4');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICED, id);
    
    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'5');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEE, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'6');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEF, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'7');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEG, id);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'8');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEH, id);
    //load line size set
    
    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'1');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEA, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'2');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEB, id);
    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'3');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEC, id);
    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'4');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZED, id);

    //load line color set
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'1');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORR, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'2');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORG, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'3');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORB, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'4');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORY, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'5');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORC, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'6');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETCOLORV, id);
   
    return ret;

}

void   xw_png_destory(void)
{
    
    if(png_t)
        free(png_t);
    png_t = NULL;
    return;

}



uint16_t *xw_get_window_png(char *window_id)
{
    if(png_t == NULL || window_id == NULL)
        return NULL;
    int i  = 0 ;
    for(i = 0; i < png_cnts ; i++){
    
        if(strcmp(window_id,png_t[i].window_id) == 0){
            //printf("window_id:%s image buf mem:%p \n",window_id,png_t[i].p);
            return png_t[i].p;
        }
    }
    return NULL;
}

void  xw_get_png_hw(char *window_id,uint16_t *w,uint16_t *h){
    
    if(png_t == NULL || window_id == NULL)
        return ;
    int i = 0;
    for(i = 0; i < png_cnts ; i++){
    
        if(strcmp(window_id,png_t[i].window_id) == 0)
        {
            *w = png_t[i].w;
            *h = png_t[i].h;
            break;
        }

    }

    return;

}


