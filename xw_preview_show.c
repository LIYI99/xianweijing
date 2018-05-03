
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "image_sdk_core.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "image_argb_ayuv.h"
#include "xw_msg_prv.h"
#include "xw_logsrv.h"
#include "xw_window_def_func.h"
#include "xw_png_load.h"



#define  XW_IMAGE_ANY_PEVIEW_WIN_W          1920/2 + 20 
#define  XW_IMAGE_ANY_PEVIEW_WIN_H          1080/2 + 20
#define  XW_IMAGE_ANY_PEVIEW_NUMS           4
#define  XW_SMALL_IMAGE_W                   1920/2/2
#define  XW_SMALL_IMAGE_H                   1080/2/2
#define  XW_IAMGE_ANY_CUT_LINE              20

#define  XW_IMAGE_ONLY_PEVIEW_WIN_W         1920
#define  XW_IMAGE_ONLY_PEVIEW_WIN_H         1080

#define  XW_IMAGE_PEVIEW_CHACE                16


#define  XW_PERVIEW_DUBULE_CHECK             200000             //usec

typedef struct  preview_handle{
    uint8_t         preview_now_state;  // 0: close all  1: any preview 2: sigle preview
    uint16_t        *preview_buf_samll[XW_IMAGE_PEVIEW_CHACE];
    uint8_t         samll_cnt_nums;
    uint8_t         samll_cnt_nums_last;
    struct timeval  tv_s;
    uint16_t        *preview_buf_big[XW_IMAGE_PEVIEW_CHACE];
    uint8_t         big_cnt_nums;
    uint8_t         big_cnt_nums_last;
    struct timeval  tv_b;

}preview_handle_t;

preview_handle_t  *xw_preview_p = NULL;


struct argb_4{
   uint16_t  g:4;
   uint16_t  r:4;
   uint16_t  b:4;
   uint16_t  a:4;
};


static void argb_to_ayuv(uint16_t *argb, uint16_t *ayuv)
{
   
    struct argb_4  *gb = (struct argb_4 *)argb;

    uint8_t y,u,v,r,g,b,a;
#if 0
    r = gb->r;
    g = gb->g;
    b = gb->b;
    a = gb->a;

#endif
#if 1
    a = (*argb & 0xF000) >> 12;
    g = (*argb & 0x0F00)  >> 4;
    b = (*argb & 0x00F0)  >> 0;
    r = (*argb & 0x0000F) << 4;
#endif
    xw_logsrv_debug("a:%d r:%d g:%d b:%d\n",a,r,g,b);
    y = (( 66 * r + 129 * g +  25 * b + 128) >> 8) +  16;
    u = ((-38 * r -  74 * g + 112 * b + 128) >> 8) + 128;
    v = ((112 * r -  94 * g -  18 * b + 128) >> 8) + 128;
    y >>= 4;
    u >>= 4;
    v >>= 4;

    *ayuv = ((a << 12) | (y << 8) | (u << 4) | (v << 0));
    xw_logsrv_err("ayuv:%x\n",*ayuv);
    return ;
}


struct getmage_p{
    uint8_t     mode;          // 0:firtst get,1:next 2:prve  3: only singbit saml      
    uint8_t     order;          // sigl : 0 - 3 //,4:next 5:prve
    void*       magep_p[4];
    int         nums;
};

static void any_preview_freshen(void *data,uint16_t *fbbuf,
        int scree_w,int scree_h)
{
    
    if(xw_preview_p == NULL )        
        return ;
    
   

    window_node_menu_t *mt  =  (window_node_menu_t *)data;
    int i,k,j,nh,nw;
    uint16_t *imagep = NULL;
    
    //clear
    if(mt->this_node->freshen_arrt == NEED_CLEAR)
    {
        for(k = mt->y ; k < (mt->h + mt->y) ;k ++){
            for(j = mt->x ;j < mt->x + mt->w ;j++)
                *(fbbuf + scree_w*k + j) = 0x0;
        }

        return ;

    }
    
    if(xw_preview_p->preview_now_state != 1)
        return;
    
    if(xw_preview_p->samll_cnt_nums == 0)
        return;
    //freshen
    for(i = 0; i < xw_preview_p->samll_cnt_nums ; i ++)
    {
        imagep = xw_preview_p->preview_buf_samll[i];
        xw_logsrv_err(" any putures preview freshen id:%d image:%p,all counts:%d\n",i,imagep,xw_preview_p->samll_cnt_nums);
#if 0
        // if(imagep == NULL)
        //   break;
#endif
        nw = i%2;
        nh = i/2;

        if(imagep != NULL){
            for( k = (mt->y + nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE )) ; 
                    k < (mt->y + XW_SMALL_IMAGE_H +  nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE ))  ; k++)
            {
                for(j = (mt->x + nw*(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )); 
                        j < (mt->x + XW_SMALL_IMAGE_W + nw *(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )) ; j++)
                {
                    *(fbbuf + scree_w * k + j) = *imagep++;       
                }
            }
        }else{
            for( k = (mt->y + nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE )) ; 
                    k < (mt->y + XW_SMALL_IMAGE_H +  nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE ))  ; k++)
            {
                for(j = (mt->x + nw*(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )); 
                        j < (mt->x + XW_SMALL_IMAGE_W + nw *(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )) ; j++)
                {
                    *(fbbuf + scree_w * k + j) = 0x0; //clear       
                }
            }

        }    
        xw_preview_p->preview_buf_samll[i] = NULL;

    
    }

    {
        xw_preview_p->preview_buf_samll[0] = NULL;
        xw_preview_p->preview_buf_samll[1] = NULL;
        xw_preview_p->preview_buf_samll[2] = NULL;
        xw_preview_p->preview_buf_samll[3] = NULL;
    }

    xw_preview_p->samll_cnt_nums_last =  xw_preview_p->samll_cnt_nums;
    xw_preview_p->samll_cnt_nums = 0;
    return ;
}


static void only_preview_freshen(void *data,uint16_t *fbbuf,
        int scree_w,int scree_h)
{
    
    window_node_menu_t *mt  =  (window_node_menu_t *)data;
    int i,k,j;
    uint16_t *imagep = NULL;
   
    //clear
    if(mt->this_node->freshen_arrt == NEED_CLEAR )    {
        for(k = mt->y ; k < mt->h + mt->y ;k ++){
            for(j = mt->x ;j < mt->x + mt->w ;j++)
                *(fbbuf + scree_w*k + j) = 0x0;
        }
        mt->this_node->video_state = CLEAR_STATE;
        return ;

    }

    if(xw_preview_p->big_cnt_nums == 0)
        return ;

    if(xw_preview_p->preview_now_state != 2)
        return ;

    imagep = xw_preview_p->preview_buf_big[0];

    for(k = mt->y ; k < mt->h ;k ++){
            for(j = mt->x ;j < mt->x + mt->w ;j++)
                *(fbbuf + scree_w*k + j) = *imagep++;;
    }
    // xw_logsrv_debug("test xxxxx k:%d j:%d\n",k,j); 
    mt->this_node->video_state = VIDEO_STATE;

    //xw_preview_p->preview_buf_big[0] = NULL;
    xw_preview_p->big_cnt_nums_last = xw_preview_p->big_cnt_nums;
    //xw_preview_p->big_cnt_nums = 0;
    return ;
}

static void any_preview_mouse_ldown(void *data){
    
   

    //now not support big puture preivew
    return ;

    struct  timeval tv1;
    // signle check not run 
    gettimeofday(&tv1,NULL);
    if(xw_preview_p->tv_s.tv_usec == 0){
        xw_preview_p->tv_s = tv1;
        return ;
    }
    // check time longer
    if(((tv1.tv_sec - xw_preview_p->tv_s.tv_sec)*1000000 + tv1.tv_usec - xw_preview_p->tv_s.tv_usec) 
            > XW_PERVIEW_DUBULE_CHECK  ){
        xw_preview_p->tv_s = tv1;
        return ;
    }
    xw_preview_p->tv_s = tv1;

    xw_logsrv_debug("test dbuale check ldown\n");

    window_node_menu_t *mt  =  (window_node_menu_t *)data;
    
    uint16_t mousex = 0,mousey = 0,n =0;
    mousex = mt->this_node->mouse_data.x;
    mousey = mt->this_node->mouse_data.y;
    
    if(mousex >= mt->x && mousex <= (mt->x + mt->w/2) && mousey >= mt->y && mousey <= (mt->y + mt->h/2))
        n = 0;
    else if(mousex >= (mt->x + mt->w/2) && mousex <= (mt->x + mt->w) && mousey >= mt->y && mousey <= (mt->y + mt->h/2) )
        n = 1;
    else if(mousex >= mt->x  && mousex <= (mt->x + mt->w/2) && mousey >= (mt->y + mt->y/2) && mousey <= (mt->y + mt->h) )
        n = 2;
    else 
        n = 3;
    //test
    if(xw_preview_p->samll_cnt_nums_last >= 0 )
    {
        
        // if get data fuc ,data save xw_preveiew_p

        if(xw_preview_p->preview_buf_big[0] == NULL)
            return ;

        // close any preview windows
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,NEED_CLEAR);
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,0);
        // open big windows
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,1);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,NEED_FRESHEN);
        xw_preview_p->preview_now_state = 2; 
    }else{
        
    }

    return ;
}


static void any_next_mouse_ldown(void *data)
{

    window_node_menu_t  *mt  = (window_node_menu_t *)data;
    mt->this_node->freshen_arrt = NEED_FRESHEN;
    int ret  = 0 ;
    struct getmage_p mage_p;
    mage_p.mode = 1;  //next any buffer
    mage_p.nums = 0;
    mage_p.order= 0;
    ret  = Image_Msg_Get(IDSCAM_IMG_MSG_GET_CAPTURE_POINT,(void *)&mage_p,sizeof(struct getmage_p));
    if(ret < 0)
        return ;
    if(mage_p.nums == 0)
        return ;
    for(ret  = 0 ; ret < mage_p.nums;ret++)
    {
            xw_preview_p->preview_buf_samll[ret] = (uint16_t *)mage_p.magep_p[ret];
    }
    xw_preview_p->samll_cnt_nums = mage_p.nums;
    xw_preview_p->preview_now_state = 1;
    Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,NEED_FRESHEN);

    return ;
}

static void any_prev_mouse_ldown(void *data)
{

    window_node_menu_t  *mt  = (window_node_menu_t *)data;
    mt->this_node->freshen_arrt = NEED_FRESHEN;
    int ret  = 0 ;
    struct getmage_p mage_p;
    mage_p.mode = 2;  //next any buffer
    mage_p.nums = 0;
    mage_p.order= 0;
    ret  = Image_Msg_Get(IDSCAM_IMG_MSG_GET_CAPTURE_POINT,(void *)&mage_p,sizeof(struct getmage_p));
    if(ret < 0)
        return ;
    if(mage_p.nums == 0)
        return ;
    for(ret  = 0 ; ret < mage_p.nums;ret++)
    {
            xw_preview_p->preview_buf_samll[ret] = (uint16_t *)mage_p.magep_p[ret];
    }
    xw_preview_p->samll_cnt_nums = mage_p.nums;
    xw_preview_p->preview_now_state = 1;
    Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,NEED_FRESHEN);

    return ;
}



static void only_preview_mouse_ldown(void *data)
{
    
    return;

    
    struct  timeval tv1;

    // signle check not run 
    gettimeofday(&tv1,NULL);
    if(xw_preview_p->tv_b.tv_usec == 0){
        xw_preview_p->tv_b = tv1;
        return ;
    }
    // check time longer
    if(((tv1.tv_sec - xw_preview_p->tv_b.tv_sec)*1000000 + tv1.tv_usec - xw_preview_p->tv_b.tv_usec) 
            > XW_PERVIEW_DUBULE_CHECK  ){
        xw_preview_p->tv_b = tv1;
        return ;
    }
    xw_preview_p->tv_b = tv1;
    
    window_node_menu_t *mt  =  (window_node_menu_t *)data;
    
    uint16_t mousex = 0,mousey = 0;
    mousex = mt->this_node->mouse_data.x;
    mousey = mt->this_node->mouse_data.y;
    
    if(mousex < mt->x + mt->w/2){
        //get prev image

    }else{
       // get next image;
    }
    
    // if get image suecssec set freshen
    

    return ;
}




int xw_preview_show(void *data)
{
    
    if(xw_preview_p != NULL)
        return -1;
    
    xw_preview_p = (preview_handle_t *)malloc(sizeof(preview_handle_t));
    if(xw_preview_p == NULL)
        return -2;
    memset(xw_preview_p,0x0,sizeof(preview_handle_t));
    
    window_node_menu_t            _mt;
    struct user_set_node_atrr     _attr;

    memset(&_mt,0x0,sizeof(window_node_menu_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    //Anys
    
    xw_get_node_window_param(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,&_mt.x,&_mt.y,&_mt.w,&_mt.h);
    _mt.color = 0x0;
    _mt.user_video_freshen = any_preview_freshen;
    _mt.video_set.mouse_left_down = any_preview_mouse_ldown;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_ANY_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);
    
    xw_get_node_window_param(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,&_mt.x,&_mt.y,&_mt.w,&_mt.h);
    _mt.color = 0x0;
    _mt.user_video_freshen = only_preview_freshen;
    _mt.video_set.mouse_left_down = only_preview_mouse_ldown;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);
   
    //create preivew next button
    memset(&_mt,0x0,sizeof(window_node_menu_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));



    //create preivew prev button
    xw_get_node_window_param(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID,&_mt.x,&_mt.y,NULL,NULL);
    xw_get_png_hw(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID,&_mt.w,&_mt.h);
    xw_logsrv_err("next x:%d y:%d w:%d h:%d \n",_mt.x,_mt.y,_mt.w,_mt.h);
    
    _mt.image_cache =  (char *)xw_get_window_png(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID);
    _mt.color = 0x0;
    _mt.user_video_freshen = NULL;
    _mt.video_set.mouse_left_down = any_next_mouse_ldown;
;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);
    
    
    xw_get_node_window_param(XW_PERVIEW_IMAGE_APER_WINDOW_ID,&_mt.x,&_mt.y,NULL,NULL);
    xw_get_png_hw(XW_PERVIEW_IMAGE_APER_WINDOW_ID,&_mt.w,&_mt.h);
    xw_logsrv_err("per x:%d y:%d w:%d h:%d \n",_mt.x,_mt.y,_mt.w,_mt.h);

    _mt.image_cache =  (char *)xw_get_window_png(XW_PERVIEW_IMAGE_APER_WINDOW_ID);
    _mt.color = 0x0;
    _mt.user_video_freshen = NULL;
    _mt.video_set.mouse_left_down = any_prev_mouse_ldown;
;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_APER_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_APER_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);



    return 0;




}


void xw_preview_close(void)
{

    int x = 0;
    xw_preview_cl_op((void *)&x);
    return ;
}

int xw_preview_cl_op(void *data)
{
    
    int px = 0;
    if(data){
        px  = *((int *)data);
    }else{
        px = 2;
    }
    struct getmage_p mage_p;
    mage_p.nums = 0;
    
    int ret  = 0;
    
    if(xw_preview_p == NULL)
        return -1;
    
    if(px == 0 && xw_preview_p->preview_now_state == 0)
        return 0;


    if(xw_preview_p->preview_now_state == 0){
        
       
        //open any
        //get data buf save the handle 
        mage_p.mode = 0 ; //friset get
        ret  = Image_Msg_Get(IDSCAM_IMG_MSG_GET_CAPTURE_POINT,(void *)&mage_p,sizeof(struct getmage_p));
        if(ret < 0)
        {
            xw_logsrv_err("not get image proint\n");
            return -2;    
        }
        for(ret  = 0 ; ret < mage_p.nums;ret++)
        {
            xw_preview_p->preview_buf_samll[ret] = (uint16_t *)mage_p.magep_p[ret];
        }
        xw_preview_p->samll_cnt_nums = mage_p.nums;
        //set node open
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANY_WINDOW_ID ,1);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,  NEED_FRESHEN);
        
        //next and per ui
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID ,1);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID,  NEED_FRESHEN);
        
        //per node
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_APER_WINDOW_ID ,1);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_APER_WINDOW_ID,  NEED_FRESHEN);

        xw_preview_p->preview_now_state = 1;
    }else if(xw_preview_p->preview_now_state == 1){
       //close any
   
        //next node
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID ,0);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANEXT_WINDOW_ID,  NEED_CLEAR);
        
        //per node
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_APER_WINDOW_ID ,0);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_APER_WINDOW_ID,  NEED_CLEAR);


        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,NEED_CLEAR);
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,0);
        xw_preview_p->preview_now_state = 0;

    }else if(xw_preview_p->preview_now_state == 2){
       //close sigle
       Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,NEED_CLEAR);
       Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,0);
       xw_preview_p->preview_now_state = 0;

    }else{

      //debug info
    }

    return 0;

}

int xw_preview_get_state(void){

    if(xw_preview_p == NULL)
        return 0;
    return  xw_preview_p->preview_now_state;
} 

int xw_preview_quit(void *data)
{
   if( xw_preview_p)
       free(xw_preview_p)
           ;
   xw_preview_p = NULL;
    
   return 0;


}


