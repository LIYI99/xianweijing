
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "image_sdk_core.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "image_argb_ayuv.h"
#include "xw_logsrv.h"

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


static void any_preview_freshen(void *data,uint16_t *fbbuf,
        int scree_w,int scree_h){
    
    if(xw_preview_p == NULL )        return ;
   
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
#if 1 //test mode
    uint16_t    test_color;
    uint32_t    incolor = 0xff0000ff; //read



    image_rgba8888_to_ayuv(incolor,&test_color); 
    for(i = 0; i < 4 ; i ++){
        
        nw = i%2;
        nh = i/2;
        if(i == 1){
            incolor = 0xffff0000; //bule 
            image_rgba8888_to_ayuv(incolor,&test_color); 
        
            //test_color = 0xff00;
        }else if(i == 2){
            incolor = 0xff00ff00; //green
            image_rgba8888_to_ayuv(incolor,&test_color); 
            //test_color = 0xf0f0;
        }else if(i == 3){
            incolor = 0xffffffff; 
            image_rgba8888_to_ayuv(incolor,&test_color); 
            //test_color =0xffff;
        }
//        xw_logsrv_debug("ayuv:0x%x",test_color);

        for( k = (mt->y + nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE )) ; 
                k < (mt->y + XW_SMALL_IMAGE_H +  nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE ))  ; k++){
            for(j = (mt->x + nw*(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )); 
                    j < (mt->x + XW_SMALL_IMAGE_W + nw *(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )) ; j++){
                *(fbbuf + scree_w * k + j) = test_color;       
            }
        }    

    
    }

    xw_preview_p->samll_cnt_nums_last =  xw_preview_p->samll_cnt_nums;
    xw_preview_p->samll_cnt_nums = 0;

#else
    //freshen
    for(i = 0; i < xw_preview_p->samll_cnt_nums ; i ++){
        imagep = xw_preview_p->preview_buf_samll[i];
        if(imagep == NULL)
            break;
        nw = i%2;
        nh = i/2;


        for( k = (mt->y + nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE )) ; 
                k < (mt->y + XW_SMALL_IMAGE_H +  nh*(XW_SMALL_IMAGE_H + XW_IAMGE_ANY_CUT_LINE ))  ; k++){
            for(j = (mt->x + nw*(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )); 
                    j < (mt->x + XW_SMALL_IMAGE_W + nw *(XW_SMALL_IMAGE_W + XW_IAMGE_ANY_CUT_LINE )) ; j++){
                *(fbbuf + scree_w * k + j) = *imagep++;       
            }
        }    
        xw_preview_p->preview_buf_samll[i] = NULL;

    
    }
    xw_preview_p->samll_cnt_nums_last =  xw_preview_p->samll_cnt_nums;
    xw_preview_p->samll_cnt_nums = 0;
#endif
    return ;
}


static void only_preview_freshen(void *data,uint16_t *fbbuf,
        int scree_w,int scree_h){
    
    window_node_menu_t *mt  =  (window_node_menu_t *)data;
    int i,k,j,nh,nw;
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
    xw_logsrv_debug("test xxxxx k:%d j:%d\n",k,j); 
    mt->this_node->video_state = VIDEO_STATE;

    //xw_preview_p->preview_buf_big[0] = NULL;
    xw_preview_p->big_cnt_nums_last = xw_preview_p->big_cnt_nums;
    //xw_preview_p->big_cnt_nums = 0;
    return ;
}

static void any_preview_mouse_ldown(void *data){
    
    
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
    if(xw_preview_p->samll_cnt_nums_last >= 0 ){
        
        // if get data fuc ,data save xw_preveiew_p
#if 1
        
        if(n == 0)
            xw_preview_p->preview_buf_big[0] = (uint16_t *)xw_get_window_png("testprivew-1ID");
        else if(n == 1){
             xw_preview_p->preview_buf_big[0] = (uint16_t *)xw_get_window_png("testprivew-2ID");
        }else if(n == 2){
            xw_preview_p->preview_buf_big[0] = (uint16_t *)xw_get_window_png("testprivew-3ID");
        }else if(n == 3){
            xw_preview_p->preview_buf_big[0] = (uint16_t *)xw_get_window_png("testprivew-4ID");
        }
        

        xw_preview_p->big_cnt_nums = 1;
#endif
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
        
        //not doing
    }
    //add get data buf func

    //
    return ;
}

static void only_preview_mouse_ldown(void *data)
{
    
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
    _mt.x = XW_PERVIEW_IMAGE_ANY_WINDOW_X; 
    _mt.y = XW_PERVIEW_IMAGE_ANY_WINDOW_Y;
    _mt.w = XW_IMAGE_ANY_PEVIEW_WIN_W ;
    _mt.h = XW_IMAGE_ANY_PEVIEW_WIN_H ; 
    _mt.color = 0x0;
    _mt.user_video_freshen = any_preview_freshen;
    _mt.video_set.mouse_left_down = any_preview_mouse_ldown;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_ANY_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);
    
    _mt.x = XW_PERVIEW_IMAGE_ONLY_WINDOW_X; 
    _mt.y = XW_PERVIEW_IMAGE_ONLY_WINDOW_Y;
    _mt.w = XW_IMAGE_ONLY_PEVIEW_WIN_W ;
    _mt.h = XW_IMAGE_ONLY_PEVIEW_WIN_H ; 
    _mt.color = 0x0;
    _mt.user_video_freshen = only_preview_freshen;
    _mt.video_set.mouse_left_down = only_preview_mouse_ldown;
    memcpy(_attr.node_id,XW_PERVIEW_IMAGE_ONLY_WINDOW_ID,strlen(XW_PERVIEW_IMAGE_ONLY_WINDOW_ID ) );
    Image_SDK_Create_Menu(_attr,_mt);
    

    //create preivew next button


    //create preivew prev button





    return 0;




}

int xw_preview_cl_op(void *data)
{
    if(xw_preview_p == NULL)
        return -1;
    if(xw_preview_p->preview_now_state == 0){
        //open any
        //get data buf save the handle 
       
        //set node open
        Image_SDK_Set_Node_En(XW_PERVIEW_IMAGE_ANY_WINDOW_ID ,1);
        Image_SDK_Set_Node_En_Freshen(XW_PERVIEW_IMAGE_ANY_WINDOW_ID,  NEED_FRESHEN);
        xw_preview_p->preview_now_state = 1;
    }else if(xw_preview_p->preview_now_state == 1){
       //close any
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


int xw_preview_quit(void *data)
{
   if( xw_preview_p)
       free(xw_preview_p)
           ;
   xw_preview_p = NULL;
    
   return 0;


}


