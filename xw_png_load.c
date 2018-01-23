#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "xw_png_load.h"
#include "image_png_put.h"
#include "xw_window_id_df.h"
#include "xw_logsrv.h"
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
                &(png_t[png_cnts].w),1);
    if(ret < 0){
       xw_logsrv_err("%s %d  path:%s ret:%d\n",__func__,__LINE__,png_path,ret);
        return -2;
    }
    xw_logsrv_debug("get window_id:%s png image  h:%d w:%d size:%d\n ",window_id,png_t[png_cnts].h,
            png_t[png_cnts].w,ret); 
    strcpy(png_t[png_cnts].window_id,window_id);
    png_t[png_cnts].p = offset_p;
    offset_p += ret;
    png_cnts++;
    return 0;

}

#define     PNG_MEM_ALL_SIZE    1024 * 1024*3
#define     WINDOW_PNG_NUMS     200


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
#define     WINDOW_LINE_MANGER_SETCOLORR    "/usr/local/bin/png/red.png"
#define     WINDOW_LINE_MANGER_SETCOLORG    "/usr/local/bin/png/green.png"
#define     WINDOW_LINE_MANGER_SETCOLORB    "/usr/local/bin/png/blue.png"
#define     WINDOW_LINE_MANGER_SETCOLORC    "/usr/local/bin/png/cyan.png"
#define     WINDOW_LINE_MANGER_SETCOLORY    "/usr/local/bin/png/yello.png"
#define     WINDOW_LINE_MANGER_SETCOLORV    "/usr/local/bin/png/violet.png"

//select line set 
#define     WINDOW_LINE_MANGER_SELECTH1    "/usr/local/bin/png/hori1.png"
#define     WINDOW_LINE_MANGER_SELECTH2    "/usr/local/bin/png/hori2.png"
#define     WINDOW_LINE_MANGER_SELECTH3    "/usr/local/bin/png/hori3.png"
#define     WINDOW_LINE_MANGER_SELECTH4    "/usr/local/bin/png/hori4.png"
#define     WINDOW_LINE_MANGER_SELECTH5    "/usr/local/bin/png/hori5.png"
#define     WINDOW_LINE_MANGER_SELECTH6    "/usr/local/bin/png/hori6.png"
#define     WINDOW_LINE_MANGER_SELECTH7    "/usr/local/bin/png/hori7.png"
#define     WINDOW_LINE_MANGER_SELECTH8    "/usr/local/bin/png/hori8.png"

#define     WINDOW_LINE_MANGER_SELECTV1    "/usr/local/bin/png/vert1.png"
#define     WINDOW_LINE_MANGER_SELECTV2    "/usr/local/bin/png/vert2.png"
#define     WINDOW_LINE_MANGER_SELECTV3    "/usr/local/bin/png/vert3.png"
#define     WINDOW_LINE_MANGER_SELECTV4    "/usr/local/bin/png/vert4.png"
#define     WINDOW_LINE_MANGER_SELECTV5    "/usr/local/bin/png/vert5.png"
#define     WINDOW_LINE_MANGER_SELECTV6    "/usr/local/bin/png/vert6.png"
#define     WINDOW_LINE_MANGER_SELECTV7    "/usr/local/bin/png/vert7.png"
#define     WINDOW_LINE_MANGER_SELECTV8    "/usr/local/bin/png/vert8.png"




void  xw_color_change_func(uint16_t *image,uint16_t size,uint16_t src_color,uint16_t det_color)
{
    if(image == NULL)
        return ;
    int i = 0;
    for(i = 0 ; i < size; i++)
    {
        if(image[i] == 0xd88 || image[i] ==  0x278)
            image[i] = 0xfd88;

    }
    return ;

}


struct  rgb{
  uint8_t  r;
  uint8_t  g;
  uint8_t  b;
  uint8_t  a;
};





static void  xw_png_load_1080(void);    //1920*1080
static void  xw_png_load_600(void);     //1024*600





int     xw_png_load_all(int mode)
{
    int ret = 0;
    ret = xw_window_png_init(PNG_MEM_ALL_SIZE,  WINDOW_PNG_NUMS);
    if(ret != 0){
        xw_logsrv_err("xw-window_png init fail revalve:%d\n",ret);
        return -1;
    }
    
    if(mode == VRCEE_VOUT_MODE_1080)
    {
        xw_png_load_1080();
    }else if(mode ==VRCEE_VOUT_MODE_600)
    {
        xw_png_load_600();
    }else{
        xw_logsrv_err("the srcee mode not suppot mode:%d\n",mode);
        return -1;
    }

    return 0;
}

static void  xw_png_load_1080(void)

{ 
   
    int ret = 0;
    
    //add main menue
    ret = xw_window_png_add(WINDOW_MAIN_MENU_PATH,XW_MAIN_WINDOW_ID );
    //add top menu
    ret = xw_window_png_add(WINDOW_TOP_MENU_PATH,XW_TOP_MENU_WINDOW_ID);
     // return ;


    uint16_t *p = NULL,w,h;
    char id[10];
    //load line arry set
    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'1');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEA, id);
    //change color
    xw_get_png_hw(id,&w,&h);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'2');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEB, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'3');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEC, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);




    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'4');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICED, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);




    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'5');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEE, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'6');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEF, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'7');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEG, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'8');
    ret = xw_window_png_add( WINDOW_LINE_MANGER_CHOICEH, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    //load line size set

    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'1');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEA, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'2');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEB, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'3');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZEC, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'4');
    ret = xw_window_png_add(  WINDOW_LINE_MANGER_SETSIZED, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


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
    //load selcet png
    

    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H1");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH1 , id);
     p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    
    
    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H2");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH2 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H3");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH3 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H4");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH4 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H5");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH5 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H6");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH6 , id);

    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H7");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH7 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H8");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTH8 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V1");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV1 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V2");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV2 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V3");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV3 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V4");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV4 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V5");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV5 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V6");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV6 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V7");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV7 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V8");
    ret = xw_window_png_add(WINDOW_LINE_MANGER_SELECTV8 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);

    



    return ;

}


//top menu
#define     SWINDOW_TOP_MENU_PATH        "/usr/local/bin/pngs/top.png"
//main menu
#define     SWINDOW_MAIN_MENU_PATH       "/usr/local/bin/pngs/main.png"

//line manger
#define     SWINDOW_LINE_MANGER_CHOICEA    "/usr/local/bin/pngs/load_a.png"
#define     SWINDOW_LINE_MANGER_CHOICEB   "/usr/local/bin/pngs/load_b.png"
#define     SWINDOW_LINE_MANGER_CHOICEC    "/usr/local/bin/pngs/load_c.png"
#define     SWINDOW_LINE_MANGER_CHOICED    "/usr/local/bin/pngs/load_d.png"
#define     SWINDOW_LINE_MANGER_CHOICEE    "/usr/local/bin/pngs/load_e.png"
#define     SWINDOW_LINE_MANGER_CHOICEF    "/usr/local/bin/pngs/load_f.png"
#define     SWINDOW_LINE_MANGER_CHOICEG    "/usr/local/bin/pngs/load_g.png"
#define     SWINDOW_LINE_MANGER_CHOICEH    "/usr/local/bin/pngs/load_h.png"
//line size set
#define     SWINDOW_LINE_MANGER_SETSIZEA    "/usr/local/bin/pngs/size1.png"
#define     SWINDOW_LINE_MANGER_SETSIZEB    "/usr/local/bin/pngs/size2.png"
#define     SWINDOW_LINE_MANGER_SETSIZEC    "/usr/local/bin/pngs/size3.png"
#define     SWINDOW_LINE_MANGER_SETSIZED    "/usr/local/bin/pngs/size4.png"
//line color set
#define     SWINDOW_LINE_MANGER_SETCOLORR    "/usr/local/bin/pngs/red.png"
#define     SWINDOW_LINE_MANGER_SETCOLORG    "/usr/local/bin/pngs/green.png"
#define     SWINDOW_LINE_MANGER_SETCOLORB    "/usr/local/bin/pngs/blue.png"
#define     SWINDOW_LINE_MANGER_SETCOLORC    "/usr/local/bin/pngs/cyan.png"
#define     SWINDOW_LINE_MANGER_SETCOLORY    "/usr/local/bin/pngs/yello.png"
#define     SWINDOW_LINE_MANGER_SETCOLORV    "/usr/local/bin/pngs/violet.png"

//select line set 
#define     SWINDOW_LINE_MANGER_SELECTH1    "/usr/local/bin/pngs/hori1.png"
#define     SWINDOW_LINE_MANGER_SELECTH2    "/usr/local/bin/pngs/hori2.png"
#define     SWINDOW_LINE_MANGER_SELECTH3    "/usr/local/bin/pngs/hori3.png"
#define     SWINDOW_LINE_MANGER_SELECTH4    "/usr/local/bin/pngs/hori4.png"
#define     SWINDOW_LINE_MANGER_SELECTH5    "/usr/local/bin/pngs/hori5.png"
#define     SWINDOW_LINE_MANGER_SELECTH6    "/usr/local/bin/pngs/hori6.png"
#define     SWINDOW_LINE_MANGER_SELECTH7    "/usr/local/bin/pngs/hori7.png"
#define     SWINDOW_LINE_MANGER_SELECTH8    "/usr/local/bin/pngs/hori8.png"

#define     SWINDOW_LINE_MANGER_SELECTV1    "/usr/local/bin/pngs/vert1.png"
#define     SWINDOW_LINE_MANGER_SELECTV2    "/usr/local/bin/pngs/vert2.png"
#define     SWINDOW_LINE_MANGER_SELECTV3    "/usr/local/bin/pngs/vert3.png"
#define     SWINDOW_LINE_MANGER_SELECTV4    "/usr/local/bin/pngs/vert4.png"
#define     SWINDOW_LINE_MANGER_SELECTV5    "/usr/local/bin/pngs/vert5.png"
#define     SWINDOW_LINE_MANGER_SELECTV6    "/usr/local/bin/pngs/vert6.png"
#define     SWINDOW_LINE_MANGER_SELECTV7    "/usr/local/bin/pngs/vert7.png"
#define     SWINDOW_LINE_MANGER_SELECTV8    "/usr/local/bin/pngs/vert8.png"



static void  xw_png_load_600(void)

{ 
    
   int ret; 
    //add main menue
    ret = xw_window_png_add(SWINDOW_MAIN_MENU_PATH,XW_MAIN_WINDOW_ID );
    //add top menu
    ret = xw_window_png_add(SWINDOW_TOP_MENU_PATH,XW_TOP_MENU_WINDOW_ID);
     // return ;


    uint16_t *p = NULL,w,h;
    char id[10];
    //load line arry set
    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'1');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEA, id);
    //change color
    xw_get_png_hw(id,&w,&h);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'2');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEB, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);

    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'3');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEC, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);




    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'4');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICED, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);




    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'5');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEE, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'6');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEF, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'7');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEG, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_CHOICE_WINDOW_ID,'8');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_CHOICEH, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    //load line size set

    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'1');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETSIZEA, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'2');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETSIZEB, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'3');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETSIZEC, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%c",XW_LINE_SET_SIZE_WINDOW_ID,'4');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETSIZED, id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    //load line color set
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'1');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORR, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'2');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORG, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'3');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORB, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'4');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORY, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'5');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORC, id);
    
    sprintf(id,"%s_%c",XW_LINE_SET_COLOR_WINDOW_ID,'6');
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SETCOLORV, id);
    //load selcet png
    

    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H1");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH1 , id);
     p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    
    
    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H2");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH2 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H3");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH3 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H4");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH4 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H5");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH5 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H6");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH6 , id);

    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H7");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH7 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"H8");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTH8 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V1");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV1 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V2");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV2 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V3");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV3 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);



    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V4");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV4 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V5");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV5 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V6");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV6 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V7");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV7 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);


    sprintf(id,"%s_%s",XW_LINE_SELECT_LINE_WINDOW_ID,"V8");
    ret = xw_window_png_add(SWINDOW_LINE_MANGER_SELECTV8 , id);
    p = xw_get_window_png(id);
    xw_color_change_func(p,w*h,0x112,0xfeee);

    
    return ;

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
            //xw_logsrv_err("window_id:%s image buf mem:%p \n",window_id,png_t[i].p);
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


