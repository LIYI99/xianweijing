
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <pthread.h>

#include "gk_image_sdk_new.h"
#include "image_text_put.h"
#include "xw_logsrv.h"
#include "image_zoom_func.h"
//device info set 
#define     VO_SCREE_W          1920
#define     VO_SCREE_H          1080
#define     VO_DISP_FPS         25
#define     VO_COLOR_FMT        16
#define     GK_DEVICE_MOUSE     "/dev/input/mouse0"
#define     GK_DEVICE_FB        "/dev/fb0"
#define     GK_MOUSE_SPEED      1

//image disp info set
#define     DISP_OBJ_MAX        200
#define     MOUSE_SIZE          30

//font set

#define     FONT_SIZE_T         16
#define     FONT_PATH_T         "/usr/local/bin/font/asc16"


#define     XW_GOKE_OPTTIMIZE   1






image_sdk_t             *sdk_handle = NULL;
static  pthread_t       mouse_thread_id, process_thread_id;

static void*  _image_fb_init(char *dev_path,int *fd);
static void*  _image_fb_deinit(void *mmap_p,int fd);
//static void   _image_fb_push(int xoffset,int yoffset);

//findkey
static int      node_id_level_re(const char *node_id);

// mouse image load
static void     _image_mouse_image_load(window_node_mouse_t *mouse);
static void     _image_mouse_image_deload(window_node_mouse_t *mouse);
static void*    _image_mouse_event_read_thread(void *data);


//handle data proess
//static void*    _image_sdk_handle_data_process(void *data);

//prcoess node limit rect func
static int  get_limit_rect_for_node_freshen(window_node_t *node);
static int  compler_limit_rect_ture(uint16_t x,uint16_t y,limit_rect_t *limit);
static void inline  limit_rect_free(limit_rect_t *limit);



static inline void      debug_node_id(window_node_t *node){

    int i;
    for(i = 0; i < MENU_LEVEL ; i++)
        if(node->node_id[i] >= 'A' && node->node_id[i] <=  'z')
            printf("%c",node->node_id[i]);
    printf("\n");
}

static void     _image_mouse_image_load(window_node_mouse_t *mouse)
{

#define     MOUSE_IMAGE_PATH_1  NULL
#define     MOUSE_IMAGE_PATH_2  NULL

    //16bit color
    //
    mouse->image_cache = (char *)buddymem_alloc(sdk_handle->mems, mouse->size *mouse->size * sdk_handle->color_fmt / 8 *2);
    if(mouse->image_cache == NULL){
        xw_logsrv_err("the sdk mempool not ougeht\n");
        //mouse->image_cache = (char *)malloc(mouse->size *mouse->size * sdk_handle->color_fmt / 8 *2);
    }
    if(mouse->image_cache == NULL)
    {
        
        xw_logsrv_err("mouse load fail ");
        return ;
    }
     
    mouse->save_cache = (char *)buddymem_alloc(sdk_handle->mems, mouse->size *mouse->size * sdk_handle->color_fmt / 8 *2);
    if(mouse->save_cache == NULL){
         xw_logsrv_err("the sdk mempool not ougeht\n");
        // mouse->save_cache = (char *)malloc(mouse->size *mouse->size * sdk_handle->color_fmt/8)
    }


    int i,k,j,r;
    uint16_t    *buf = NULL;
    r = mouse->size / 2;

    mouse->color = 0xbbca;
    if(MOUSE_IMAGE_PATH_1 == NULL || MOUSE_IMAGE_PATH_2 == NULL){
        //build color mouse
        buf = (uint16_t *)mouse->image_cache;
        for(k = 0 ,j = 0; k < mouse->size; k ++ ){
            for(i = 0; i < mouse->size; i++,j++)
                if(((k-r)*(k-r) + (i-r)*(i-r)) <= r *r)
                {
                    buf[j] = mouse->color;

                }else{
                    buf[j] = 0;
                }
        }    
        buf = (uint16_t *)(mouse->image_cache + mouse->size* mouse->size*2);

        for(k = 0 ,j = 0; k < mouse->size; k ++ ){
            for(i = 0; i < mouse->size; i++,j++)
                if(((k-r)*(k-r) + (i-r)*(i-r)) <= r *r)
                {
                    buf[j] = 0xFF80;

                }else{
                    buf[j] = 0;
                }
        }



    }else{
        //load image to buf
    }
    mouse->image_p = mouse->image_cache;    
    return ;


}

static void     _image_mouse_image_deload(window_node_mouse_t *mouse)
{

    if(mouse == NULL)
        return;

    buddymem_free(sdk_handle->mems, mouse->image_cache);
    buddymem_free(sdk_handle->mems, mouse->save_cache);
    //free(mouse->image_cache);
    //free(mouse->save_cache);

    return ;

}





//ok
void    Image_SDK_Init(void){

    if(sdk_handle != NULL)
        return ;
    sdk_handle =  (image_sdk_t *)malloc(sizeof(image_sdk_t));
    if(sdk_handle == NULL)
        return;
    memset(sdk_handle ,0x0,sizeof(image_sdk_t));

    sdk_handle->en = 1;
    sdk_handle->scree_w = VO_SCREE_W;
    sdk_handle->scree_h = VO_SCREE_H;
    sdk_handle->color_fmt = VO_COLOR_FMT;
    sdk_handle->disp_fps = VO_DISP_FPS;    

    //open fb fd
    sdk_handle->mmap_p = _image_fb_init(GK_DEVICE_FB,&sdk_handle->video_fd);
    if(sdk_handle->video_fd < 0)
        goto ERR1;
    //open mouse fd 
    sdk_handle->mouse_fd =  gk_mouse_open(GK_DEVICE_MOUSE,VO_SCREE_W,
            VO_SCREE_H,GK_MOUSE_SPEED,MOUSE_SIZE);

    if(sdk_handle->mouse_fd < 0){
        xw_logsrv_err("not use mouse\n");
    }

    //create window node pool
    sdk_handle->window_node_pool =  
        object_pool_create(sizeof(window_node_t),DISP_OBJ_MAX+1,NULL);
    if(sdk_handle->window_node_pool == NULL)
        goto ERR2;

    //create image meadie obj pool
    unsigned int obj_size = 0;
    obj_size = sizeof(window_node_button_t) > sizeof(window_node_menu_t)? 
        sizeof(window_node_button_t) : sizeof(window_node_menu_t);
    obj_size = obj_size > sizeof(window_node_line_t)? obj_size : sizeof(window_node_line_t);

    obj_size = obj_size > sizeof(window_node_text_t)? obj_size : sizeof(window_node_text_t);

    sdk_handle->object_pool = object_pool_create(obj_size,DISP_OBJ_MAX+1,NULL);

    if(sdk_handle->object_pool == NULL)
        goto ERR3;
    
    //now not use
#if 0
    sdk_handle->limit_rect_pool  =  object_pool_create(sizeof(limit_rect_t),DISP_OBJ_MAX+1,NULL);
    if(sdk_handle->limit_rect_pool == NULL)
        goto ERR4;
#endif

#define     SDK_MEM_BLOCK_SIZE      128
#define     SDK_MEM_BLOCK_NUMS      1024

    sdk_handle->mems = buddymem_create(SDK_MEM_BLOCK_SIZE,SDK_MEM_BLOCK_NUMS );
    if(sdk_handle->mems == NULL)
        goto ERR4;

    //init root node
    window_node_t *root = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    memset(root, 0x0 ,sizeof(window_node_t));
    memcpy(root->node_id,"A",1);
    root->en_node = 1;
    sdk_handle->root = root;
    xw_logsrv_debug("window root:%p\n",root);
    //add mouse window node
    window_node_mouse_t *mouse = NULL;
    
    //mouse = (window_node_mouse_t *)malloc(sizeof(window_node_mouse_t));
    mouse = (window_node_mouse_t *)buddymem_alloc(sdk_handle->mems,sizeof(window_node_mouse_t));
    if(mouse == NULL){
        xw_logsrv_err("fisrt get obj fail???\n");
    }

    memset(mouse,0x0,sizeof(window_node_mouse_t));
    mouse->size = MOUSE_SIZE;
    _image_mouse_image_load(mouse);
    sdk_handle->mouse = mouse;

    //en sdk handle
    sdk_handle->en = 1;

    //load font lib
    int ret = 0; 
    ret = image_text_lib_init(FONT_SIZE_T,FONT_PATH_T);
    if(ret < 0)
        xw_logsrv_err("load font lib fail ret:%d\n",ret);

    return;

ERR4:
    object_pool_destory(sdk_handle->object_pool);

ERR3:
    object_pool_destory(sdk_handle->window_node_pool);

ERR2:
    if(sdk_handle->mouse_fd > 0)
        gk_mouse_close(sdk_handle->mouse_fd); 

    _image_fb_deinit(sdk_handle->mmap_p,sdk_handle->video_fd);

ERR1:
    free(sdk_handle);
    sdk_handle = NULL;
    return ;
}


//ok
void    Image_SDK_deInit(void)
{
    if(sdk_handle == NULL)
        return;
    sdk_handle->en = 0;
    
    //free thread
    pthread_join(mouse_thread_id,NULL);
    pthread_join(process_thread_id,NULL);
    //close font lib
    image_text_lib_deinit();
    //free object pool
    object_pool_destory(sdk_handle->window_node_pool);
    object_pool_destory(sdk_handle->object_pool);
    //free mouse buf
    _image_mouse_image_deload(sdk_handle->mouse);
    //free(sdk_handle->mouse);
    
    //free mem pool
    buddymem_destroy(sdk_handle->mems);

    //close dev
    _image_fb_deinit(sdk_handle->mmap_p,sdk_handle->video_fd);
    if(sdk_handle->mouse_fd > 0)
        gk_mouse_close(sdk_handle->mouse_fd); 
    //frre sdk handle
    free(sdk_handle);
    sdk_handle = NULL;
    return;
}


//ok
static int   node_id_level_re(const char *node_id)
{
 
    //return 0;
    if(node_id == NULL)
        return -1;
    int i;
    for(i = 0 ;i  < MENU_LEVEL ; i++){
        if( (node_id[i] >= 'A' && node_id[i] <= 'Z' )|| (node_id[i] <='z' && node_id[i] >= 'a'))
            continue;
        else
            break;

        //       ( node_id[i] < 'A'   || node_id[i] > 'z' )
          //      break;
    }

    return i;
}
//ok
static inline window_node_t  *key_find_node(window_node_t *father,char key,int level)
{
    window_node_t *temp = father->s_head;
    // xw_logsrv_err("father->shead:%p,father:%p,root:%p\n",temp,father,sdk_handle->root);
    for (;temp != NULL; temp = temp->next)
    {
        //    xw_logsrv_err("temp->node_id[2]:%c\n",temp->node_id[1]); 

        if(temp->node_id[level] ==  key)
            break;
    }
    return temp;
}
//ok
static  window_node_t  *find_father_node(char *node_id,int level){

    if(level == 2){
        xw_logsrv_debug("return father node:%p\n",sdk_handle->root);
        return sdk_handle->root;
    }

    window_node_t *temp  = sdk_handle->root;

    int k = 1;
    for (k = 1 ; k < level -1 && temp != NULL; k ++ ){
        temp = key_find_node(temp,node_id[k],k);
    }
    return temp;
}
//ok
static int  window_node_inster(window_node_t *node){

    int level ;
    //get window level
    if(!node)
        return -1;

    level = node_id_level_re(node->node_id);
    if(level < 2){
        xw_logsrv_err("inster node:%p,inster node node_id:%p\n",node,node->node_id);
       // xw_logsrv_err(" node_id write erro node->node_id:%c%c%c\n",node->node_id[0],
         //       node->node_id[1],node->node_id[2]);
         
        return -1;
    }

    //get window father window
    //xw_logsrv_err("get level:%d\n",level);
    window_node_t *ftemp = NULL,*same = NULL;
    ftemp = find_father_node(node->node_id,level);
    if(ftemp == NULL){
        xw_logsrv_err("not find father node ftemp:%p sdk_handle->root:%p level:%d\n",ftemp,
                sdk_handle->root,level);
        return -2;
    }
    //find same window
    same = key_find_node(ftemp,node->node_id[level -1],level -1);
    if(same != NULL){
        xw_logsrv_err("a same node ,you set name error \n");
        return -3;
    }
    //inster node 
    node->f_node = ftemp;

    if(ftemp->s_head == NULL){
        ftemp->s_head = node;
        ftemp->s_end = node;
        return 0;
    }

    node->next = NULL;
    node->prev = node->f_node->s_end;
    node->prev->next = node;
    node->f_node->s_end = node;

#if 0
    node->next = ftemp->s_head;
    node->prev = NULL;
    ftemp->s_head->prev = node;
    ftemp->s_head = node;

#endif
    return 0;
}

//ok
int    Image_SDK_Create_Button( struct user_set_node_atrr attr,
        window_node_button_t _button){

    if(sdk_handle == NULL)
        return -2;

    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){

        return -3;
    }
    xw_logsrv_debug("linenode:%p \n",lq);

    window_node_button_t * button = NULL;
    button = (window_node_button_t *)object_pool_get(sdk_handle->object_pool);
    if(button == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }

    memcpy((void *)lq->node_id ,(void *)attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,(void *)lq);
        object_pool_free(sdk_handle->object_pool,(void *)button);
        return -6;
    }

    *button =  _button;
    button->this_node = lq;
    if(_button.video_set.data == NULL)
        button->video_set.data  = (void *)button;

    if(button->x > sdk_handle->scree_w){
        button->x = sdk_handle->scree_w - button->w;
        xw_logsrv_err("you set x > srcee_w\n");
    }
    if(button->y > sdk_handle->scree_h){
        button->y = sdk_handle->scree_h -button->h;
        xw_logsrv_err("you set y > srcee_h\n");
    }
    if(button->w > sdk_handle->scree_w){
        button->w = 60;
        xw_logsrv_err("you set w > srcee_w,now set 60 pixl\n");

    }
    if(button->h > sdk_handle->scree_h){
        button->h = 30;
        xw_logsrv_err("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = attr.en_node;
    lq->freshen_arrt = attr.en_freshen;
    lq->win_type = OBJECT_BUTTION;
    lq->window = (void *)button;
    lq->mouse_garp_reflect = attr.mouse_garp_reflect;
    return 0;



}
int    Image_SDK_Create_Menu(struct user_set_node_atrr attr,
        window_node_menu_t _menu){

    if(sdk_handle == NULL)
        return -2;

    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){

        return -3;
    }

    window_node_menu_t * menu = NULL;
    menu = (window_node_menu_t *)object_pool_get(sdk_handle->object_pool);
    if(menu == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }

    memcpy(lq->node_id ,attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,lq);
        object_pool_free(sdk_handle->object_pool,menu);
        return -6;
    }

    *menu =  _menu;
    menu->this_node = lq; 
    if(_menu.video_set.data == NULL)
        menu->video_set.data  = (void *)menu;

    if(menu->x > sdk_handle->scree_w){
        menu->x = sdk_handle->scree_w - menu->w;
        xw_logsrv_err("you set x > srcee_w\n");
    }
    if(menu->y > sdk_handle->scree_h){
        menu->y = sdk_handle->scree_h -menu->h;
        xw_logsrv_err("you set y > srcee_h\n");
    }
    if(menu->w > sdk_handle->scree_w){
        menu->w = 60;
        xw_logsrv_err("you set w > srcee_w,now set 60 pixl\n");

    }
    if(menu->h > sdk_handle->scree_h){
        menu->h = 30;
        xw_logsrv_err("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = attr.en_node;
    lq->freshen_arrt = attr.en_freshen;
    lq->win_type = OBJECT_MENU;
    lq->window = (void *)menu;
    lq->mouse_garp_reflect = attr.mouse_garp_reflect;

    return 0;

}
int    Image_SDK_Create_Line(struct user_set_node_atrr attr,
        window_node_line_t _line)
{

    if(sdk_handle == NULL)
        return -2;

    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){

        return -3;
    }
    // xw_logsrv_err("linenode:%p \n",lq);

    window_node_line_t * line = NULL;
    line = (window_node_line_t *)object_pool_get(sdk_handle->object_pool);
    if(line == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }

    memcpy(lq->node_id ,attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,lq);
        object_pool_free(sdk_handle->object_pool,line);
        return -6;
    }

    *line =  _line;
    line->this_node = lq; 
    if(_line.video_set.data == NULL)
        line->video_set.data  = (void *)line;

    if(line->start_x > sdk_handle->scree_w){
        line->start_x = sdk_handle->scree_w -line->size;
        xw_logsrv_err("you set x > srcee_w\n");
    }
    if(line->start_y > sdk_handle->scree_h){
        line->start_y = sdk_handle->scree_h -line->size;
        xw_logsrv_err("you set y > srcee_h\n");
    }
    if(line->end_x > sdk_handle->scree_w){
        line->end_x = sdk_handle->scree_w - line->size;
        xw_logsrv_err("you set w > srcee_w,now set 60 pixl\n");

    }
    if(line->end_y > sdk_handle->scree_h){
        line->end_y = sdk_handle->scree_h - line->size;
        xw_logsrv_err("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = attr.en_node;
    lq->freshen_arrt = attr.en_freshen;
    lq->move_arrt = attr.move_arrt;
    lq->win_type = OBJECT_LINE;
    lq->window = (void *)line;
    lq->mouse_garp_reflect = attr.mouse_garp_reflect;

    return 0;


}
int    Image_SDK_Create_Text(struct user_set_node_atrr attr,
        window_node_text_t _text)
{

    if(sdk_handle == NULL)
        return -2;

    if(image_text_get_font_width() <= 0){

        xw_logsrv_err("text lib not load \n");
        return -9;
    }
    uint8_t asc_width = image_text_get_font_width();
    uint8_t font_size = image_text_get_font_size();



    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){

        return -3;
    }

    window_node_text_t * text = NULL;
    text = (window_node_text_t *)object_pool_get(sdk_handle->object_pool);
    if(text == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }

    memcpy(lq->node_id ,attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,lq);
        object_pool_free(sdk_handle->object_pool,text);
        return -6;
    }

    *text =  _text;
    text->this_node = lq;
    text->asc_width = asc_width;
    text->font_size = font_size;

    //need change mem pool;

    //text->text_cache = (char *)malloc(text->lens +1); //save text 
    text->text_cache   = (char *)buddymem_alloc(sdk_handle->mems, text->lens+1);


    memset(text->text_cache,0x0,text->lens + 1);

    if(_text.video_set.data == NULL)
        text->video_set.data  = (void *)text;

    if(text->x > sdk_handle->scree_w){
        text->x = sdk_handle->scree_w - text->lens *text->asc_width;
        xw_logsrv_err("you set x > srcee_w\n");
    }
    if(text->y > sdk_handle->scree_h){
        text->y = sdk_handle->scree_h - text->font_size;
        xw_logsrv_err("you set y > srcee_h\n");
    }

    lq->en_node = attr.en_node;
    lq->freshen_arrt = attr.en_freshen;
    lq->win_type = OBJECT_TEXT_WIN;
    lq->window = (void *)text;
    lq->mouse_garp_reflect = attr.mouse_garp_reflect;

    return 0;


}

int    Image_SDK_Create_Bar(struct user_set_node_atrr attr,
        window_node_bar_t _bar)
{

    xw_logsrv_debug("create bar w:%d max_value:%d now_value:%d \n",_bar.w,
            _bar.max_value,_bar.now_value);
    if(sdk_handle == NULL)
        return -2;

    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){

        return -3;
    }

    window_node_bar_t *     bar = NULL;
    bar = (window_node_bar_t *)object_pool_get(sdk_handle->object_pool);
    if(bar == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }

    memcpy(lq->node_id ,attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,lq);
        object_pool_free(sdk_handle->object_pool,bar);
        return -6;
    }

    *bar =  _bar;
    bar->this_node = lq;

    if(_bar.video_set.data == NULL)
        bar->video_set.data  = (void *)bar;

    if(bar->x > sdk_handle->scree_w){
        bar->x = sdk_handle->scree_w - bar->w;
        xw_logsrv_err("you set x > srcee_w\n");
    }
    if(bar->y > sdk_handle->scree_h){
        bar->y = sdk_handle->scree_h -bar->h;
        xw_logsrv_err("you set y > srcee_h\n");
    }

    lq->en_node = attr.en_node;
    lq->freshen_arrt = attr.en_freshen;
    lq->win_type = OBJECT_BAR;
    lq->window = (void *)bar;
    lq->mouse_garp_reflect = attr.mouse_garp_reflect;
    xw_logsrv_debug("bar->now_value:%d _bar.now_value:%d\n",bar->now_value,_bar.now_value);
    return 0;


}



//ok
static window_node_t  *find_all_key_node(char *node_id,int level){

    if(level == 1)
        return sdk_handle->root;

    window_node_t *temp  = sdk_handle->root;

    int k ;
    for (k = 1 ; k < level ; k ++ ){
        temp = key_find_node(temp,node_id[k],k);
    }
    return temp;
}



static  inline  window_node_t    *find_frist_free_node(window_node_t *head)
{   
    window_node_t *temp = head;

    for(;temp != NULL; temp =  temp->next){
        if(temp->order_attr == FREE_ORDER)
            break;
    }

    return temp;

}

int     Image_SDK_Destory_node(char *node_id)
{
    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        xw_logsrv_err("func:%s line:%d not find the node:%s\n",__func__,__LINE__,node_id);
        return -2;
    }

    if(temp->window)
        object_pool_free(sdk_handle->object_pool,temp->window);
    object_pool_free(sdk_handle->object_pool,(void *)temp);
    
    return 0;
    
}


//ok
int     Image_SDK_Set_Node_Move_Atrr(char *node_id,NODE_MOVE_ARRT _arrt)
{

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
        return -2;
    temp->move_arrt = _arrt;

    return 0;

}
//ok
int     Image_SDK_Set_Node_En(char *node_id,uint8_t en)
{

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
    {
        xw_logsrv_err("not find this window :%s level:%d\n",node_id,level);

        return -2;
    }
    window_node_t   *save = NULL;
  
    //move the free node head
    do{
        if(en == 0)
            break;
        xw_logsrv_err("maybe move this window :%s to free head\n",node_id);


#if 0
#ifdef  XW_GOKE_OPTTIMIZE
        if(level > 2 && node_id[1] == 'b')
            break;
#endif
#endif

        if(temp->f_node != sdk_handle->root && temp->f_node->order_attr == FIXD_ORDER)
            break;



        if(temp->order_attr == FIXD_ORDER)
            break;
        if(temp->f_node->s_head == temp)
            break;
       // if(temp->prev->order_attr == FIXD_ORDER)
         //   break;
        save  = find_frist_free_node(temp->f_node->s_head);
        if(save == NULL || temp == save)
            break;
        temp->prev->next = temp->next;
        if(temp->next)
            temp->next->prev = temp->prev;
        else{
            temp->f_node->s_end = temp->prev;   
        }
        temp->prev = save->prev;
        if(temp->prev == NULL){
            temp->f_node->s_head = temp;
        }else{
            temp->prev->next = temp;
        }
        temp->next = save;
        save->prev = temp;
         xw_logsrv_debug("#########move############ node id:%c%c%c level:%d in node_id:%s\n",temp->node_id[0],temp->node_id[1],temp->node_id[2],level,node_id);

        break;
    }while(1);
    
      temp->en_node = en;

    return 0;

}

//ok
int     Image_SDK_Set_Node_En_Freshen(char *node_id, NODE_FRESHEN_ARRT en_freshen)
{

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        xw_logsrv_err("not find this window :%s \n",node_id);
        return -2;
    }
    temp->freshen_arrt = en_freshen;
    xw_logsrv_debug("temp set freansh_arrt:%d,node_id:%c%c%c\n",temp->freshen_arrt,temp->node_id[0],
            temp->node_id[1],temp->node_id[2]); 
    return 0;

}

int     Image_SDK_Set_Node_Order(char *node_id,NODE_ORDER_ATTR _attr){

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
    {
        xw_logsrv_err("not find this window :%s \n",node_id);

        return -2;
    }
    temp->order_attr = _attr;

    return 0;


}
int     Image_SDK_Set_Node_Submenu(char *node_id,uint8_t en){

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
    {
        xw_logsrv_err("not find this window :%s \n",node_id);

        return -2;
    }
    temp->en_submenu = en;

     xw_logsrv_debug("set en_submenu:%dnode id:%c%c%c level:%d in node_id:%s\n",en,
           temp->node_id[0],temp->node_id[1],temp->node_id[2],level,node_id);

    return 0 ;


}



int     Image_SDK_Set_Text_Node_Text(char *node_id, char *text,int size)
{


    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        //       xw_logsrv_err("not find node_id\n");
        return -2;
    }
    if(temp->win_type != OBJECT_TEXT_WIN)
        return -3;

    window_node_text_t *tt = (window_node_text_t *)temp->window;

    if(tt->lens < size)
        return -5;

    memcpy(tt->text_cache,text,size);

    tt->text_cache[size] = '\0';
    //xw_logsrv_debug("set char tt->text:%s\n",tt->text_cache);
    temp->freshen_arrt = NEED_FRESHEN;

    return 0;

}

int     Image_SDK_Set_Text_Node_Xy(char *node_id, uint16_t x,uint16_t y)
{


    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        //       xw_logsrv_err("not find node_id\n");
        return -2;
    }
    if(temp->win_type != OBJECT_TEXT_WIN)
        return -3;

    window_node_text_t *tt = (window_node_text_t *)temp->window;

    tt->x = x;
    tt->y = y;


    temp->freshen_arrt = NEED_FRESHEN;

    return 0;

}




int     Image_SDK_Set_Line_Node_Param(char *node_id, window_node_line_t  *lt)
{

    if(node_id == NULL || lt == NULL)
        return -1;
    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        xw_logsrv_err("not find node_id\n");
        return -2;
    }
    if(temp->win_type != OBJECT_LINE)
        return -3;

    window_node_line_t *tt = (window_node_line_t *)temp->window;
    tt->start_x  = lt->start_x;
    tt->end_x    = lt->end_x;
    tt->start_y  = lt->start_y;
    tt->end_y    = lt->end_y;
    tt->size     = lt->size;
    tt->color    = lt->color;

    // temp->f_node->freshen_arrt = NEED_FRESHEN;

    return 0;

}




int     Image_SDK_Set_Node_Disp(char *node_id,NODE_VIDEO_ATTR _attr)
{
    if(node_id == NULL)
        return -1;
    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        return -2;
    }


    temp->video_attr = _attr;

    return 0;

}

int     Image_SDK_Set_Button_Color(char *node_id ,uint16_t color){

    if(node_id == NULL)
        return -1;
    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL){
        return -2;
    }

    window_node_button_t *bt =  (window_node_button_t *)temp->window;
    bt->color = color;
    temp->freshen_arrt = NEED_FRESHEN;
    return 0;
}


static inline int image_buttont_xy_analysis(void *data,window_func_t *set,
        GK_MOUSE_DATA mdata)
{

    window_node_button_t *bt = (window_node_button_t *)data;
    //GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    //mdata.x = x + 

    if( (mdata.x  >= bt->x || mdata.x+ MOUSE_SIZE >= bt->x)  && mdata.x <= bt->x+bt->w 
            && (mdata.y >= bt->y || mdata.y + MOUSE_SIZE >= bt->y)&& mdata.y <= bt->y+bt->h){

        bt->this_node->mouse_data = sdk_handle->mouse_new_data; 
        //xw_logsrv_debug("mouse check mouse.x:%d mouse.y:%d button.x:%d button.y:%d\n",mdata.x,mdata.y,
        //      bt->x,bt->y);
        return 1;
    }
    else 
        return 0;

}

static inline int image_menu_xy_analysis(void *data,
        window_func_t *set,GK_MOUSE_DATA mdata){

    window_node_menu_t *bt = (window_node_menu_t *)data;
    //GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;

    if( mdata.x  >= bt->x  && mdata.x <= bt->x+bt->w 
            && mdata.y >= bt->y && mdata.y <= bt->y+bt->h)
    {

        bt->this_node->mouse_data = sdk_handle->mouse_new_data; 

        return 1;
    }
    else 
        return 0;

}

static inline int image_line_xy_analysis(void *data,
        window_func_t *set,GK_MOUSE_DATA mdata){


    window_node_line_t *bt = (window_node_line_t *)data;

    int h,w;
    if(bt->start_x  == bt->end_x){
        h = bt->end_y - bt->start_y;
        w = bt->size;
    }else{

        w = bt->end_x - bt->start_x;
        h = bt->size;
    }
    int  size = 30;
    //   GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    //xw_logsrv_debug("chcek  line set->data:%p line:%p\n" ,set->data,bt);
    if( mdata.x  >= (bt->start_x -size) && mdata.x <= (bt->start_x + w + size) 
            && mdata.y >= (bt->start_y - size) && mdata.y <= (bt->start_y + h + size)){

        bt->this_node->mouse_data = sdk_handle->mouse_new_data; 
        //xw_logsrv_debug("line x:%d y:%d ",bt->start_x,bt->start_y);
        //debug_node_id(bt->this_node);
        return 1;
    }
    else 
        return 0;


    return 0;

}


static inline int image_text_xy_analysis(void *data,
        window_func_t *set,GK_MOUSE_DATA mdata){


    window_node_text_t *bt = (window_node_text_t *)data;

    //GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    //xw_logsrv_debug("chcek  line set->data:%p line:%p\n" ,set->data,bt);
    if( mdata.x  >= bt->x  && mdata.x <= bt->x + bt->asc_width * bt->lens 
            && mdata.y >= bt->y  && mdata.y <= bt->y+bt->font_size){

        bt->this_node->mouse_data = sdk_handle->mouse_new_data; 

        return 1;
    }
    else 
        return 0;


    return 0;

}

static inline int image_bar_xy_analysis(void *data,
        window_func_t *set,GK_MOUSE_DATA mdata){


    window_node_bar_t *bt = (window_node_bar_t *)data;

    //GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    //xw_logsrv_debug("chcek  line set->data:%p line:%p\n" ,set->data,bt);
    if( (mdata.x + MOUSE_SIZE /2)  >= (bt->x - 20)  && (mdata.x + MOUSE_SIZE/2) <= (bt->x + bt->w + 20)  
            && (mdata.y + MOUSE_SIZE /2) >= bt->y - 2  && (mdata.y+ MOUSE_SIZE/2) <= (bt->y+bt->h+2)){
        //add mouse data

        bt->this_node->mouse_data = sdk_handle->mouse_new_data; 
        //xw_logsrv_debug("check bar \n"); 
        return 1;
    }
    else 
        return 0;


    return 0;

}





static void   _image_analysis_mdata(GK_MOUSE_DATA mdata)

{

    if(sdk_handle->root == NULL)
        return ;

    int ks;
    
    if(sdk_handle->check_level_cnt != 0){ 
        if(sdk_handle->mouse_new_data.event == sdk_handle->last_check_node[sdk_handle->check_level_cnt-1]->last_event
                && sdk_handle->mouse_new_data.event != GK_MOUSE_OFFSET)
        {
           // xw_logsrv_debug("######### only one event ###### mouse event:%d\n",sdk_handle->mouse_new_data.event);
            sdk_handle->last_check_node[sdk_handle->check_level_cnt-1]->mouse_data = sdk_handle->mouse_new_data;
            return ;
        }
    }
    
    //clear sdk hanlde save mouse check node buf
    for(ks = 0 ; ks < MENU_LEVEL ; ks++)
        sdk_handle->check_node[ks] = NULL;

    //set sdk handle last node is mouse not check mode
    for(ks = 0; ks < MENU_LEVEL ; ks ++){
        if(sdk_handle->last_check_node[ks] == NULL)
            break;
        sdk_handle->last_check_node[ks]->check_node = 0;
    }
    //clear last check node cnt
    sdk_handle->check_level_cnt = 0;



    window_node_t  *node = NULL,*save_node[MENU_LEVEL+1],*ft_stack[MENU_LEVEL] ;
    memset(save_node,0x0,sizeof(void *) * MENU_LEVEL+1);
    int check_cnt = 0,ret = 0,stack_cnt = 1;
    
   


    ft_stack[0] = sdk_handle->root;

    node = sdk_handle->root->s_head;

    while ( node  ){

        ret = 0; 
        if(node->en_node){

            switch(node->win_type)
            {
                case OBJECT_BUTTION:

                    ret = image_buttont_xy_analysis(node->window,NULL,mdata);
                    break;
                case OBJECT_MENU:
                    ret = image_menu_xy_analysis(node->window,NULL,mdata);
                    break;
                case OBJECT_LINE:
                    ret = image_line_xy_analysis(node->window,NULL,mdata);
                    break;
                case OBJECT_TEXT_WIN:
                    ret  = image_text_xy_analysis(node->window,NULL,mdata);
                    break;
                case OBJECT_BAR:
                    ret  = image_bar_xy_analysis(node->window,NULL,mdata);
                    break;
                default:
                    break;
            }
        }

        // 
        if(ret > 0)
        {
            if( check_cnt == 0 || (check_cnt > 0 && node->f_node == save_node[check_cnt - 1])){
                node->check_node = 1;
                save_node[check_cnt] = node;
                check_cnt++;

              //  xw_logsrv_debug("check node_id:%c%c%c node:%p check_cnt:%d mouserx:%d mousry;%d \n\n",node->node_id[0],node->node_id[1], node->node_id[2],node,check_cnt,mdata.x,mdata.y);
            }
        }

        if(ret > 0  && node->s_head == NULL ){
            break;

        }

        // if check node have childe node ,than tarlve the son node 
        // else tarvle bother node
        if( ret > 0  && node->s_head != NULL && node->en_submenu != 0 ){

            ft_stack[stack_cnt] = node;
            stack_cnt++;
            node = node->s_head;

        }else{
            node = node->next;
        }
        if(node == NULL && stack_cnt > 0 ){
            stack_cnt--;
            node = ft_stack[stack_cnt]->next;

            //if sub windows not check ,than those tarlve mouse envet eof       
            if(check_cnt != 0 && node){
               // xw_logsrv_debug("quit tarlve mouse decitc\n");
                //debug_node_id(node);
                break;
            }

        }

    }

    // there have a bug ,now the mouse data get a pthread ,poces s another thread 
    if( check_cnt != 0 )
    {
        if(save_node[check_cnt -1]->last_event == sdk_handle->mouse_new_data.event
                &&save_node[check_cnt - 1]->move_arrt == NOT_MOVE )
        {
            check_cnt = 0;
        }
    }

    //move top
    window_node_t *temp = NULL;
    int  k;

    for (k  =  0 ; k < check_cnt ; k++)
    {
        // ****** #### *****
        // if the mouse is offset event ,is not need move node,but maybe ....
        // some scene have bugs.....
        if(mdata.event == GK_MOUSE_OFFSET)
            break;



        node = save_node[k];


        // the node is head or FIXD node ,not need move 
        if(node == node->f_node->s_head || node->order_attr == FIXD_ORDER )
            continue;
        //if it is perv node is fixd node ,not need move
        if(node->prev->order_attr == FIXD_ORDER)
            continue;

        node->prev->next = node->next;
        if(node->next){
            node->next->prev = node->prev;
        }else{ //is end
            node->f_node->s_end = node->prev;
        }
        //need find free fisrt node 
        temp = find_frist_free_node(node->f_node->s_head);
        if(temp == NULL)
            continue;
        xw_logsrv_debug("move window order\n"); 
        debug_node_id(node);

        node->prev = temp->prev;
        node->next = temp;
        temp->prev = node;
        if(node->prev)
            node->prev->next = node;
        else{ 
            node->f_node->s_head = node;
        }

        //if the video state is put ,and the freshen not is clear than need update
        if(node->video_state == VIDEO_STATE && node->freshen_arrt != NEED_CLEAR
                && node->en_intersection != 0 ){
            node->freshen_arrt = NEED_FRESHEN;

            xw_logsrv_debug("##2######mdata.event:%d en_intersectio:%d node_id:%c%c%c \n\n",mdata.event,
                    node->en_intersection,node->node_id[0],node->node_id[1],
                    node->node_id[2]);
        }
    }

    sdk_handle->check_level_cnt = check_cnt;
    //save now mouse check node
    for (k  =  0 ; k < check_cnt ; k++){
        sdk_handle->check_node[k] = save_node[k];
        sdk_handle->check_node[k]->last_event = sdk_handle->mouse_new_data.event;
    }

    return  ;

}

static window_func_t inline *get_window_func(window_node_t *node)
{


    if(node == NULL)
        return NULL;

    window_func_t *func = NULL;

    switch(node->win_type){

        case OBJECT_BUTTION:
            func = &(((window_node_button_t *)node->window)->video_set);
            break;
        case OBJECT_MENU:
            func = &(((window_node_menu_t *)node->window)->video_set);
            break;
        case OBJECT_LINE:
            func = &(((window_node_line_t *)node->window)->video_set);
            break;
        case OBJECT_TEXT_WIN:
            func = &(((window_node_text_t *)node->window)->video_set);
            break;
        case OBJECT_BAR:
            func = &(((window_node_bar_t *)node->window)->video_set);
            break;
        default:
            func = NULL;
            break;
    }


    return func;;
}

static void  _image_window_func_run(void *data)
{

    if(sdk_handle->root->s_head == NULL)
        return;
    window_node_t  *node = NULL ;
    int k = 0;
    //build leave event and run
    window_func_t *funcs = NULL;
    for(k = MENU_LEVEL -1  ; k >= 0; k--){

        if(sdk_handle->last_check_node[k] == NULL)
            continue;

        node = sdk_handle->last_check_node[k];
        // xw_logsrv_debug("node->chcek_node:%d\n",node->check_node);

        if(node->check_node)
            continue;

        funcs = get_window_func(node);
        if(funcs->mouse_leave){
            //xw_logsrv_debug("func:%s line:%d run leave func %c%c%c \n",
            //     __func__,__LINE__,node->node_id[0],node->node_id[1],
            //    node->node_id[2]); 
            funcs->mouse_leave(funcs->data);
        }
        node->last_event = 0;
        sdk_handle->last_check_node[k] = NULL; 
    }

    if(sdk_handle->check_level_cnt == 0)
        return;
    //clear save buf
    for(k = 0; k < MENU_LEVEL ; k++){
        sdk_handle->last_check_node[k] = NULL;
    }
    //save node
    for(k = 0 ;k < sdk_handle->check_level_cnt; k++){
        sdk_handle->last_check_node[k] = sdk_handle->check_node[k];
    }


    //get top event,and only run top window event 
    node = sdk_handle->check_node[sdk_handle->check_level_cnt -1];

    //clear levevl save 
   //sdk_handle->check_level_cnt = 0; 

    if(!node->en_node)
        return;

    funcs = get_window_func(node);
    if(funcs == NULL)
        return;
    switch(sdk_handle->mouse_new_data.event)

    {

        case GK_MOUSE_OFFSET:
            if(funcs->mouse_offset)    
                funcs->mouse_offset(funcs->data);
            break;
        case GK_MOUSE_LEFT_DOWN:
            if(funcs->mouse_left_down)    
                funcs->mouse_left_down(funcs->data);
            break;
        case GK_MOUSE_LEFT_UP:
            if(funcs->mouse_left_up)
                funcs->mouse_left_up(funcs->data);
            break;
        case GK_MOUSE_RIGHT_DOWN:
            if(funcs->mouse_right_down)
                funcs->mouse_right_down(funcs->data);
            break;
        case GK_MOUSE_RIGHT_UP:
            if(funcs->mouse_right_up)    
                funcs->mouse_right_up(funcs->data);
            break;
        case GK_MOUSE_LEAVE:
            if(funcs->mouse_leave)
                funcs->mouse_leave(funcs->data);
            break;

        case GK_MOUSE_NOTHOR:
            break;

        case GK_MOUSE_DEL:
            break;

        default:
            break;
    }
    
    return ;

}



static void freshen_image_buttion(void *data){

    window_node_button_t *bt =  (window_node_button_t *)data;

    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 

    if(bt->user_video_freshen){
        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);
        bt->this_node->freshen_arrt = NORTHING;
        bt->this_node->video_state = VIDEO_STATE;
        return;
    }


    int k,i,s;
    //
    if(bt->this_node->freshen_arrt  == NEED_CLEAR){
        xw_logsrv_debug("button clear\n");
        for(k = bt->last_y ; k < (bt->h + bt->last_y) ;k++){
            for(i = bt->last_x ;  i < (bt->w + bt->last_x) ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
        }
        bt->this_node->video_state = CLEAR_STATE;
    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){

        //xw_logsrv_debug("freshen button\n");

        //if open move attr ,need clear before x,y
        if(bt->this_node->move_arrt != NOT_MOVE){
            for(k = bt->last_y ; k < (bt->h + bt->last_y) ;k++){
                for(i = bt->last_x ;  i < (bt->w + bt->last_x) ; i++ )
                    *(buf+ sdk_handle->scree_w*k + i) = 0;    
            }
        }
        for(s = 0 ,k = bt->y ; k < (bt->h + bt->y) ;k++){
            for(i = bt->x ;  i < (bt->w + bt->x) ; i++,s++ )
                if(bt->image_cache){
                    *(buf+ sdk_handle->scree_w*k + i) = *(((int16_t *)bt->image_cache) +s);    
                }else{
                    *(buf+ sdk_handle->scree_w*k + i) = bt->color;
                }
        }
        bt->last_x = bt->x;
        bt->last_y = bt->y;
        bt->this_node->video_state = VIDEO_STATE;
    }

    bt->this_node->freshen_arrt = NORTHING;

}

#ifdef  XW_GOKE_OPTTIMIZE

static uint8_t  top_menu_video_state = 0 ;
static uint8_t  main_menu_video_state = 0;



#endif



static void freshen_image_menu(void *data){

    window_node_menu_t *bt =  (window_node_menu_t *)data;

    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 

    if(bt->user_video_freshen){

        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);
        bt->this_node->freshen_arrt = NORTHING;

        return;
    }

    int k,i ,s;
    //
    if(bt->this_node->freshen_arrt  == NEED_CLEAR){
        xw_logsrv_debug("clear menu \n");

        for(k = bt->last_y ; k < (bt->last_h + bt->last_y) ;k++){
            for(i = bt->last_x ;  i < (bt->last_w + bt->last_x) ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
        }
        bt->this_node->video_state = CLEAR_STATE;

    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){
        //if open move attr ,need clear before x,y
        if(bt->this_node->move_arrt != NOT_MOVE){
            for(k = bt->last_y ; k < (bt->h + bt->last_y) ;k++){
                for(i = bt->last_x ;  i < (bt->w + bt->last_x) ; i++ )
                    *(buf+ sdk_handle->scree_w*k + i) = 0;    
            }
        }

        for(s = 0 ,k = bt->y ; k < (bt->h + bt->y) ;k++) {
            for(i = bt->x ;  i < (bt->w + bt->x) ; i++,s++ )
            {

                if(bt->image_cache){
                    *(buf+ sdk_handle->scree_w*k + i) = *(((int16_t *)bt->image_cache) +s);    
                }else{
                    *(buf+ sdk_handle->scree_w*k + i) = bt->color;
                }

            }    
        }
        bt->last_h = bt->h;
        bt->last_w = bt->w;
        bt->last_x = bt->x;
        bt->last_y = bt->y;

        bt->this_node->video_state = VIDEO_STATE;

    }

    bt->this_node->freshen_arrt = NORTHING;

    return ;

}
#ifdef  XW_GOKE_OPTTIMIZE

static   window_node_t  *main_menu= NULL,*top_menu= NULL;

static uint16_t main_x = 0, main_y = 0,main_xd = 0,main_yd;
static uint16_t top_x = 0,top_y = 0,top_xd = 0,top_yd = 0 ;


static void freshen_image_line(void *data){

    int level = 0;
    if(main_menu == NULL){
        level       = node_id_level_re("Aa");
        main_menu   =  find_all_key_node("Aa",level);
    }
    if( top_menu == NULL){
        level       =  node_id_level_re("Ac");
        top_menu    =  find_all_key_node("Ac",level);
    }


    uint8_t video_state_main = 0,video_state_top = 0;
    window_node_menu_t *mt = NULL;

    if(main_menu == NULL){
        video_state_main = 0;
    }else {
        if(main_menu->freshen_arrt == NEED_FRESHEN ||
                main_menu->video_state == VIDEO_STATE)
            video_state_main = 1;
        if(main_yd == 0){
            mt = (window_node_menu_t *)main_menu->window;
            main_x = mt->x;
            main_y = mt->y;
            main_xd = mt->x + mt->w;
            main_yd = mt->h + mt->y;
            xw_logsrv_debug("main_x:%d y:%d main_xd:%d main_yd:%d\n",main_x,
                    main_y,main_xd,main_yd);
        }
    }
    if(top_menu == NULL){
        video_state_top = 0;
    }else {

        if(top_menu->freshen_arrt == NEED_FRESHEN ||
                top_menu->video_state == VIDEO_STATE)
            video_state_top = 1;

        if(top_yd == 0){
            mt = (window_node_menu_t *)top_menu->window;
            top_x = mt->x;
            top_y = mt->y;
            top_xd = mt->w + mt->x;
            top_yd = mt->h + mt->y;
            xw_logsrv_debug("main_x:%d y:%d main_xd:%d main_yd:%d top state:%d\n",top_x,
                    top_y,top_xd,top_yd, video_state_top);

        }

    }


    window_node_line_t *bt =  (window_node_line_t *)data;
    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 
#if 0
    if(bt->user_video_freshen){
        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);
        bt->this_node->freshen_arrt = NORTHING;
        bt->this_node->video_state = VIDEO_STATE;
        return ;
    }

#endif

    int k,i,h,w,ho,wo;
    if(bt->start_x  == bt->end_x){
        h = bt->end_y - bt->start_y;
        w = bt->size;
        ho = h;
        wo = bt->last_size;
    }else{

        w = bt->end_x - bt->start_x;
        h = bt->size;
        ho = bt->last_size;
        wo = w;
    }

    //int step_x = 0;
    if(bt->this_node->freshen_arrt  == NEED_CLEAR){

        for(k = bt->last_y ; k < bt->last_y + ho ;k++){
            for(i = bt->last_x  ;  i < bt->last_x + wo ; i++ )
            {
                if( k >= top_y && k <= top_yd && i >= top_x && i<= top_xd && video_state_top == 1){
                    i  = top_xd ;
                    continue;
                }
                if( k >= main_y && k <= main_yd && i >= main_x && i <= main_xd && video_state_main == 1){
                    i  = main_xd ;
                    continue;
                }
                //if(k < top_y && k > top_yd && i < top_x && i > top_xd 
                //      && k < main_x && k > main_xd && i < main_y && i > main_yd )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
            }
        }

        bt->this_node->video_state = CLEAR_STATE;

    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){

        //if open move attr ,need clear before x,y
        if(bt->this_node->move_arrt != NOT_MOVE){
            for(k = bt->last_y ; k < (ho + bt->last_y) ;k++){
                for(i = bt->last_x ;  i < (bt->last_x + wo)  ; i++ )
                {
                    if( k >= top_y && k <= top_yd && i >= top_x && i < top_xd && video_state_top == 1){
                        i  = top_xd ;
                        continue;
                    }else if( k >= main_y && k <= main_yd && i >= main_x &&  i <= main_xd &&video_state_main == 1){
                        i  = main_xd ;
                        continue;
                    }
                    //if(k < top_y && k > top_yd && i < top_x && i > top_xd 
                    //      && k < main_x && k > main_xd && i < main_y && i > main_yd )


                    *(buf+ sdk_handle->scree_w*k + i) = 0x0;    
                }
            }
        }

        for(k = bt->start_y ; k < (h + bt->start_y) ;k++){
            for(i = bt->start_x ;  i < (bt->start_x + w)  ; i++ )
            {
                if( k >= top_y && k <= top_yd && i >= top_x && i <= top_xd && video_state_top == 1){
                    i  = top_xd ;
                    continue;
                }else if( k >= main_y && k <= main_yd && i >= main_x && i<= main_xd && video_state_main == 1){
                    i  = main_xd ;
                    continue;
                }
                //if(k < top_y && k > top_yd && i < top_x && i > top_xd 
                //      && k < main_x && k > main_xd && i < main_y && i > main_yd )

                *(buf+ sdk_handle->scree_w*k + i) = bt->color;
            }    
        }

        bt->last_x = bt->start_x;
        bt->last_y = bt->start_y;
        bt->last_size = bt->size;

        bt->this_node->video_state = VIDEO_STATE;

    }
    bt->this_node->limit = NULL;
    bt->this_node->freshen_arrt = NORTHING;
    return ;
}

#else
static void freshen_image_line(void *data){

    window_node_line_t *bt =  (window_node_line_t *)data;

    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 

    if(bt->user_video_freshen){

        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);

        bt->this_node->freshen_arrt = NORTHING;
        bt->this_node->video_state = VIDEO_STATE;
        return ;
    }

    int k,i,h,w,ho,wo,ret = 1;
    if(bt->start_x  == bt->end_x){
        h = bt->end_y - bt->start_y;
        w = bt->size;
        ho = h;
        wo = bt->last_size;
    }else{

        w = bt->end_x - bt->start_x;
        h = bt->size;
        ho = bt->last_size;
        wo = w;
    }

    if(bt->this_node->freshen_arrt  == NEED_CLEAR){

        for(k = bt->last_y ; k < bt->last_y + ho ;k++){
            for(i = bt->last_x ;  i < bt->last_x + wo ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
        }

        bt->this_node->video_state = CLEAR_STATE;

    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){

        //if open move attr ,need clear before x,y
        if(bt->this_node->move_arrt != NOT_MOVE){
            for(k = bt->last_y ; k < (ho + bt->last_y) ;k++){
                for(i = bt->last_x ;  i < (bt->last_x + wo)  ; i++ )
                {
                    //limit rect conut
                    //  if(!compler_limit_rect_ture(i,k,bt->this_node->limit))
                    *(buf+ sdk_handle->scree_w*k + i) = 0x0;    

                }
            }
        }

        for(k = bt->start_y ; k < (h + bt->start_y) ;k++){
            for(i = bt->start_x ;  i < (bt->start_x + w)  ; i++ ){
                // if(!compler_limit_rect_ture(i,k,bt->this_node->limit))
                *(buf+ sdk_handle->scree_w*k + i) = bt->color;
            }    
        }

        bt->last_x = bt->start_x;
        bt->last_y = bt->start_y;
        bt->last_size = bt->size;

        bt->this_node->video_state = VIDEO_STATE;
        //debug_node_id(bt->this_node);

    }
    //limit_rect_free(bt->this_node->limit);
    bt->this_node->limit = NULL;
    bt->this_node->freshen_arrt = NORTHING;
    return ;
}

#endif


static void freshen_image_bar(void *data){

    window_node_bar_t *bt =  (window_node_bar_t *)data;

    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 

    if(bt->user_video_freshen){

        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);

        return;
    }
    int k,i;
    //
    if(bt->this_node->freshen_arrt  == NEED_CLEAR){

        for(k = bt->last_y ; k < (bt->h + bt->last_y) ;k++){
            for(i = bt->last_x ;  i < (bt->last_x +bt->w); i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
        }
    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){

        for(k = bt->y ; k < (bt->h + bt->y) ;k++){
            for(i = bt->x ;  i < (bt->x +bt->w) ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0xFd88;    
        }
        int vl = bt->w * bt->now_value/bt->max_value;
        //xw_logsrv_debug("xxxx bar bar now need push bar lens:%d  bt->w:%d now_value:%d color:%d\n",vl,bt->w,bt->now_value,
          //      bt->bar_color);
        for(k = bt->y ; k < (bt->h + bt->y) ;k++){
            for(i = bt->x ;  i < ( bt->x + vl )  ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = bt->bar_color;    
        }
        bt->last_x = bt->x;
        bt->last_y = bt->y;

        bt->this_node->freshen_arrt = NORTHING;

    }

    return ;
}
//need wirte text load lib

static void freshen_image_text(void *data){

    window_node_text_t *bt =  (window_node_text_t *)data;

    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 

    if(bt->user_video_freshen){

        bt->user_video_freshen(data,buf,VO_SCREE_W,VO_SCREE_H);
        bt->this_node->video_state = VIDEO_STATE;
        bt->this_node->freshen_arrt = NORTHING;

        return;
    }
    uint8_t  *text_bit = NULL; 

    int k,i;
    //
    if(bt->this_node->freshen_arrt  == NEED_CLEAR){

        for(k = bt->last_y ; k < (bt->font_size + bt->last_y) ;k++){
            for(i = bt->last_x ;  i < (bt->asc_width *bt->lens + bt->last_x) ; i++ )
            {
                if(bt->this_node->move_arrt == NOT_MOVE)
                    *(buf+ sdk_handle->scree_w*k + i) = bt->win_color;  
                else
                    *(buf+ sdk_handle->scree_w*k + i) = 0x0;  

            }  
        }
        bt->this_node->video_state = CLEAR_STATE;
    }else if(bt->this_node->freshen_arrt == NEED_FRESHEN){

        if(bt->last_x != 0 && bt->last_y != 0){
            for(k = bt->last_y ; k < (bt->font_size + bt->last_y) ;k++){

                for(i = bt->last_x ;  i < (bt->asc_width *bt->lens + bt->last_x) ; i++ ){

                    if(bt->this_node->move_arrt == NOT_MOVE){
                        *(buf+ sdk_handle->scree_w*k + i) = bt->win_color;

                    }else{
                        *(buf+ sdk_handle->scree_w*k + i) = 0x0;  
                    }  
                }
            }
        }
        //usleep(200000);
        char  s;
        int   nums = 0 ;

        for(nums = 0; nums < bt->lens; nums++){
            s = bt->text_cache[nums];
            if(s == '\0')
                break;
            text_bit = image_text_lib_put_pixl(&s);
            

            if(text_bit == NULL)
                break;
            int j = 0; 
            for(k = bt->y ; k < (bt->font_size + bt->y) ;k++){
                for(i = bt->x + nums*bt->asc_width ;  i < (bt->x + nums*bt->asc_width+bt->asc_width) ; i++,j++ )
                {
                    if(text_bit[j] == 0)
                        *(buf+ sdk_handle->scree_w*k + i) = bt->win_color;    
                    else
                        *(buf+ sdk_handle->scree_w*k + i) = bt->text_color;  
                }
            }
        }

        bt->this_node->video_state = VIDEO_STATE;
        bt->last_x = bt->x;
        bt->last_y = bt->y;
    }
    bt->this_node->freshen_arrt = NORTHING;

    return ;


}


static void freshen_image_mouse(void)
{



    if(!sdk_handle->mouse_data_updated)
        return  ;


    sdk_handle->mouse->x = sdk_handle->mouse_new_data.x;
    sdk_handle->mouse->y = sdk_handle->mouse_new_data.y;
    window_node_mouse_t *bt = sdk_handle->mouse;
    uint16_t *buf = (uint16_t *)sdk_handle->mmap_p; 
    int k,i;

#define  MOUSE_LINE_SIZE    2
    //reset data
    //clear
#if 1
    //1.reset data  2.save data 3.push data

    uint16_t *savebuf ;
    savebuf =  (uint16_t *)bt->save_cache;

    //reset data 
    for(k = bt->y_last + bt->size/2; k < (MOUSE_LINE_SIZE + bt->y_last+bt->size/2) ; k++)
    {
        for(i = bt->x_last ;  i < (bt->size + bt->x_last) ; i++){
            if( *(buf+ sdk_handle->scree_w*k + i) == bt->color )  //if mouse use image ,this need change
                *(buf+ sdk_handle->scree_w*k + i) = *savebuf++;
            else
            {
                savebuf++; 
                //xw_logsrv_debug(" W :srcecc buf x,y peixl:%u,mouse color:%u\n",*(buf + sdk_handle->scree_w*k +i),bt->color);
            }
        }
    }

    for(k = bt->x_last + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->x_last+bt->size/2) ; k++)
    {
        for(i = bt->y_last ;  i < (bt->size + bt->y_last) ; i++){
            if(*(buf+ sdk_handle->scree_w*i + k) == bt->color) //if mouse use image ,this need change        
                *(buf+ sdk_handle->scree_w*i + k) = *savebuf++;
            else{
                //  xw_logsrv_debug("H:srecc buf,x,y peixl:%u,mouse color:%u\n",
                //               *(buf + sdk_handle->scree_w*i + k),bt->color);
                savebuf++;
            }
        }
    }
    //save data;
    savebuf =  (uint16_t *)bt->save_cache;

    for(k = bt->y + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->y+bt->size/2) ; k++)
    {
        for(i = bt->x ;  i < (bt->size + bt->x) ; i++ )
            *savebuf++  =  *(buf+ sdk_handle->scree_w*k + i);
    }
    for(k = bt->x + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->x + bt->size/2) ; k++)
    {
        for(i = bt->y ;  i < (bt->size + bt->y) ; i++ )
            *savebuf++  = *(buf+ sdk_handle->scree_w*i + k);
    } 

    //pushdata
    for(k = bt->y + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->y+bt->size/2) ; k++)
    {
        for(i = bt->x ;  i < (bt->size + bt->x) ; i++ ){
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;
            // xw_logsrv_debug("mem :%d \n",*(buf + sdk_handle->scree_w*k +i));
        }
    }
    for(k = bt->x + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->x + bt->size/2) ; k++)
    {
        for(i = bt->y ;  i < (bt->size + bt->y) ; i++ )
            *(buf+ sdk_handle->scree_w*i + k) = bt->color;
    } 
#endif

#if 0
    for(k = bt->y_last + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->y_last+bt->size/2) ; k++)
    {
        for(i = bt->x_last ;  i < (bt->size + bt->x_last) ; i++ ,j++)
            *(buf+ sdk_handle->scree_w*k + i) = 0;
    }
    for(k = bt->x_last + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->x_last+bt->size/2) ; k++)
    {
        for(i = bt->y_last ;  i < (bt->size + bt->y_last) ; i++ )
            *(buf+ sdk_handle->scree_w*i + k) = 0;
    }

    //push
    for(k = bt->y + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->y+bt->size/2) ; k++)
    {
        for(i = bt->x ;  i < (bt->size + bt->x) ; i++ ,j++)
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;;
    }
    for(k = bt->x + bt->size/2 ; k < (MOUSE_LINE_SIZE + bt->x + bt->size/2) ; k++)
    {
        for(i = bt->y ;  i < (bt->size + bt->y) ; i++ )
            *(buf+ sdk_handle->scree_w*i + k) = bt->color;
    } 
#endif


#if 0
    //push 
    for(k = bt->y,j= 0 ; k < (bt->size + bt->y) ;k++){
        for(i = bt->x ;  i < (bt->size + bt->x) ; i++,j++)

            *(buf+ sdk_handle->scree_w*k + i) = *(((int16_t *)bt->image_p)+j);    
    }

#endif
    bt->x_last = bt->x;
    bt->y_last = bt->y;
    
    //xw_logsrv_debug("%dupdate mmap\n",__LINE__);
    sdk_handle->mmap_updated = 1;
    return ;
}


static inline void _image_freshen(window_node_t *node)
{

    if(node->video_attr != OPEN_DISP)
    {
        if(node->freshen_arrt == NEED_CLEAR){
            node->video_state = CLEAR_STATE;
        }else{
            node->video_state = VIDEO_STATE;
        }
        node->freshen_arrt = NORTHING;
        //debug_node_id(node);

        return;
    }
    
    sdk_handle->mmap_updated = 1;
    

    switch(node->win_type){

        case OBJECT_BUTTION:
            freshen_image_buttion(node->window);
            break;
        case OBJECT_MENU:
            freshen_image_menu(node->window);

            break;

        case OBJECT_LINE:
            freshen_image_line(node->window);

            break;
        case OBJECT_TEXT_WIN:
            freshen_image_text(node->window);
            break;
        case OBJECT_BAR:
            freshen_image_bar(node->window);
            break;

        default:
            break;
    }

}




static void image_freshen_set_sub_freshen(window_node_t *node)
{
    window_node_t  *temp = node->s_head;
    for(;temp != NULL;temp = temp->next){
        if((temp->video_state != CLEAR_STATE && temp->freshen_arrt ==  NORTHING)
                || (temp->f_node->video_attr == CLOSE_DISP)){
            temp->freshen_arrt = NEED_FRESHEN; 

        }
    }
    //xw_logsrv_debug("&&&&&move############ node id:%c%c%c \n",node->node_id[0],node->node_id[1],node->node_id[2]);



    return;

} 

static void image_freshen_set_sub_clear(window_node_t *node)
{
    window_node_t  *temp = node->s_head;
    for(;temp != NULL;temp = temp->next){
        // if(temp->video_state != CLEAR_STATE && temp->freshen_arrt ==  NORTHING)
        temp->freshen_arrt = NEED_CLEAR;

    }
    return;

} 

static inline void image_set_bother_clear_freshen(window_node_t *head)
{

    window_node_t *f_node = head->f_node;
    // if father menu is need disp 
    if(f_node->video_attr == OPEN_DISP && f_node != sdk_handle->root)
    {

        f_node->freshen_arrt = NEED_FRESHEN;
        for(;head != NULL;head = head->next)
        {

            if( head->freshen_arrt == NEED_CLEAR || (head->video_state == CLEAR_STATE
                        && head->freshen_arrt != NEED_FRESHEN))
            {
                head->freshen_arrt =  NORTHING;
            }else{  
                head->freshen_arrt = NEED_FRESHEN;
            }
          
        }
        return;
    }
    // father menu is not disp


    for(;head != NULL;head = head->next)
    {
        if(head->en_intersection){
            if(head->video_state == VIDEO_STATE && head->freshen_arrt != NEED_CLEAR)
            {
                debug_node_id(head);
                head->freshen_arrt = NEED_FRESHEN;
            }
        }

    }
    return;

}

static void image_clear_video(void)
{

    //clear biggan the head  
    if(sdk_handle->root->s_head == NULL)
        return ;


    window_node_t *node = NULL,*ft_stack[MENU_LEVEL],*clear_stack[100];
    int stack_cnt = 1,clear_cnt = 0;
    ft_stack[0] = sdk_handle->root;
    node = sdk_handle->root->s_head;

    //set attr
    while ( node  )
    {

        if( node->freshen_arrt == NEED_CLEAR)
        {

            if(node->en_intersection){
#ifdef XW_GOKE_OPTTIMIZE
                if(node->node_id[1] != 'b')
#endif
                    image_set_bother_clear_freshen(node->f_node->s_head); 

            }

            clear_stack[clear_cnt] = node;
            clear_cnt++;

            if(node->s_head != NULL && node->video_attr == CLOSE_DISP)
            {
                image_freshen_set_sub_clear(node);
            }
        }
        //clear not need en_submenu
        if(/*node->en_submenu &&*/ node->s_head != NULL){
            ft_stack[stack_cnt] = node;
            node = node->s_head;
            stack_cnt++;
        }else{
            node = node->next;
        }
        if(node == NULL){
            stack_cnt--;
            node = ft_stack[stack_cnt]->next;
        }         

    }
    //clear not user order
    int i = 0 ;

    for(i = 0 ; i < clear_cnt ; i++)
    {
        if(clear_stack[i]->freshen_arrt == NEED_CLEAR)
            _image_freshen(clear_stack[i]);
    }


    return;

}

#ifdef XW_GOKE_OPTTIMEZE

#define     TOP_ID      "Ac"
#define     MAIN_ID     "Aa"
#define     ARRY_LINE   "Ab"
#endif


static inline void image_set_bother_put_freshen(window_node_t *node)
{


#ifdef XW_GOKE_OPTTIMIZE

    window_node_t   *temp = node;
#if 0
    if(node->node_id[1] == 'b')
        return ;
#endif

#endif



    node = node->prev; 
    for( ; node != NULL; node = node->prev)
    {

        if(node->en_intersection){

            if(node->video_state == VIDEO_STATE && node->freshen_arrt != NEED_CLEAR){
            
#ifdef XW_GOKE_OPTTIMIZE
            if(node->f_node == sdk_handle->root && (node->node_id[1]== 'c'||
                        node->node_id[1] == 'a') && temp->node_id[1] == 'b')
                    continue;
#endif
            node->freshen_arrt = NEED_FRESHEN;
            }
        }    
    }
    return;
}

static inline int  find_set_freshen_fnode(window_node_t **savebuf,window_node_t *fnode,int cnt)
{

    int i = 0;
    for(i  = 0 ; i < cnt ;i++)
        if(savebuf[i] == fnode)
            return 1;

    return 0;
}


static void  image_put_video(void)
{
    //tarlve list set all  ,very level only set one times
    if(sdk_handle->root->s_end == NULL)
        return ;


    window_node_t *node = NULL,*ft_stack[MENU_LEVEL],*save_set[100];
    memset(save_set,0x0,sizeof(void *)* 100);

    int stack_cnt = 1,save_cnt = 0;
    ft_stack[0] = sdk_handle->root;

    node = sdk_handle->root->s_end;
    //set freshen attr
    while ( node  ){

        if(node->en_node && node->freshen_arrt == NEED_FRESHEN )
        {

            //use dereferntt func
#if 0
            if(node->f_node == sdk_handle->root ||
                    (node->f_node->en_node == 1 && node->f_node != sdk_handle->root))
            {
                get_limit_rect_for_node_freshen(node);
            }


#else

            //the every level only set one times the fshen attr ,becares the image put run before,compler the all
            //window intersection attr,and set flags
            if(node->en_intersection  
                    && !find_set_freshen_fnode(save_set,node->f_node,save_cnt))
            {
                // xw_logsrv_debug("set bother freshen attr fathernode:%p\n",node->f_node);
                //debug_node_id(node);
                image_set_bother_put_freshen(node);
                save_set[save_cnt] = node->f_node;
                save_cnt++;
            }
#endif

            //freshen now node
            if(node->f_node == sdk_handle->root){
                _image_freshen(node);
            }else if(node->f_node->en_node == 1 && node->f_node != sdk_handle->root){
                _image_freshen(node);
            }
            // if have sub window set sub window freshen
            if(node->en_submenu && node->s_head != NULL)
                image_freshen_set_sub_freshen(node);
        }
        // the freshen attr is only about of the need run
        if(/*node->en_submenu &&*/node->s_end != NULL){
            ft_stack[stack_cnt] = node;
            node = node->s_end;
            stack_cnt++;
        }else{
            node = node->prev;
        }
        if(node == NULL){
            stack_cnt--;
            node = ft_stack[stack_cnt]->prev;
        }         

    }



    return ;

}
#if 1
extern  uint16_t *lcd_mem;


static void test_zoom_lcd(void)
{
    
    image_zoom_t    data;
    data.outbuf     = lcd_mem;//(uint16_t *)malloc(1024*600*2);
    data.outwidth   = 1024;
    data.outheight  = 600;
    data.inwidth    = 1920;
    data.inheight   = 1080;
    data.inbuf      = (uint16_t*)sdk_handle->mmap_p;//xw_get_window_png("testprivew-1ID");
    
    iamge_zoom_func(&data);
    //Image_SDK_Set_Node_En("Ax",1);
   // Image_SDK_Set_Node_En_Freshen("Ax",NEED_FRESHEN);

    //usleep(100000);

    return ;



}

#endif

static void  _image_freshen_video(void)
{

    image_clear_video();
    image_put_video();
    freshen_image_mouse();
    sdk_handle->mmap_updated = 0;
    
    return ;

}


//debug xy test


static  void*   _image_mouse_event_read_thread(void *data)
{

    GK_MOUSE_DATA       mdata;
    memset(&mdata,0x0,sizeof(GK_MOUSE_DATA));

    fd_set              readfds;
    struct  timeval     tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0 ;
    int                 retval = 0;
    sdk_handle->mouse_thread_state = 1;
    while(1)
    {
        //quit
        if(sdk_handle->en == SDK_WORK_STATE_QUIT)
            break;
        if(sdk_handle->en == SDK_WORK_STATE_STOP){
            usleep(100000);
            sdk_handle->mouse_thread_state = SDK_WORK_STATE_STOP;
            continue;
        }
        sdk_handle->mouse_thread_state = SDK_WORK_STATE_RUN;


        //host instret mouse
        if (sdk_handle->need_restart_mouse)
        {

            sdk_handle->mouse_fd =  
                gk_mouse_open(GK_DEVICE_MOUSE,VO_SCREE_W,VO_SCREE_H,GK_MOUSE_SPEED,MOUSE_SIZE);
            sdk_handle->need_restart_mouse = 0;
        }
        //read mouse data
        if(sdk_handle->mouse_fd > 0)
        {

            FD_ZERO(&readfds);
            FD_SET(sdk_handle->mouse_fd, &readfds);
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            retval = select(sdk_handle->mouse_fd+1,&readfds,NULL,NULL,&tv);
            if(retval <= 0)
                continue;
            if(FD_ISSET(sdk_handle->mouse_fd,&readfds))
            {

                if(gk_mouse_read_data(sdk_handle->mouse_fd,&mdata) < 0 )
                {
                    continue;
                }
                sdk_handle->mouse_new_data = mdata;
                sdk_handle->mouse_data_updated = 1;

            }
        }else{
            sleep(1);
        }
    }
    
    sdk_handle->mouse_thread_state = 0;

    return NULL;
}

static void* _image_sdk_handle_data_process_thread(void *data)
{


    int wait_times = 1000*1000 / sdk_handle->disp_fps;

    GK_MOUSE_DATA _ldata;

    sdk_handle->event_thread_state = 1;

    while(1)
    {
        //quit
        if(sdk_handle->en == SDK_WORK_STATE_QUIT)
            break;
        if(sdk_handle->en == SDK_WORK_STATE_STOP){
            sdk_handle->event_thread_state = SDK_WORK_STATE_STOP;
            usleep(100000);
            continue;
        }
        sdk_handle->event_thread_state = SDK_WORK_STATE_RUN;

        if(sdk_handle->mouse_data_updated){

            _ldata = sdk_handle->mouse_new_data;

            _image_analysis_mdata(sdk_handle->mouse_new_data);
            _image_window_func_run(NULL);

            if(_ldata.x == sdk_handle->mouse_new_data.x &&
                    _ldata.y == sdk_handle->mouse_new_data.y &&
                    _ldata.event == sdk_handle->mouse_new_data.event)
                sdk_handle->mouse_data_updated = 0;

        }else{

            usleep(wait_times);
        } 

        _image_freshen_video();
    }
    sdk_handle->event_thread_state = 0;

    return NULL;

}

struct  update_param_save{

    uint16_t    x;
    uint16_t    y;
    uint16_t    end_x;
    uint16_t    end_y;
    WIN_TYPE_S  win_type; 
};

static  int update_param_get(window_node_t *node,struct update_param_save *data)
{
    data->win_type = node->win_type;
    switch(node->win_type){

        case OBJECT_BUTTION:
            data->x         = ((window_node_button_t *)(node->window))->x;
            data->y         = ((window_node_button_t *)(node->window))->y;
            data->end_x     = ((window_node_button_t *)(node->window))->w + data->x;
            data->end_y     = ((window_node_button_t *)(node->window))->h + data->y;

            break;
        case OBJECT_MENU:
            data->x         = ((window_node_menu_t *)(node->window))->x;
            data->y         = ((window_node_menu_t *)(node->window))->y;
            data->end_x     = ((window_node_menu_t *)(node->window))->w + data->x;
            data->end_y     = ((window_node_menu_t *)(node->window))->h + data->y;
            break;

        case OBJECT_LINE:

            data->x         = ((window_node_line_t *)(node->window))->start_x;
            data->y         = ((window_node_line_t *)(node->window))->start_y;
            data->end_x     = ((window_node_line_t *)(node->window))->end_x;
            data->end_y     = ((window_node_line_t *)(node->window))->end_y;
            if(data->x == data->end_x){ // line
                data->end_x += ((window_node_line_t *)(node->window))->size;

            }else{  // row line
                data->end_y += ((window_node_line_t *)(node->window))->size;

            }
            break;
        case OBJECT_TEXT_WIN:
            data->x = ((window_node_text_t *)(node->window))->x;
            data->y = ((window_node_text_t *)(node->window))->y;
            data->end_x = ((window_node_text_t *)(node->window))->lens;
            data->end_x = data->end_x * ((window_node_text_t *)(node->window))->asc_width;
            data->end_y = ((window_node_text_t *)(node->window))->font_size;

            break;
        case OBJECT_BAR:
            data->x = ((window_node_bar_t *)(node->window))->x;
            data->y = ((window_node_bar_t *)(node->window))->y;
            data->end_x = ((window_node_bar_t *)(node->window))->w;
            data->end_y = ((window_node_bar_t *)(node->window))->h;
            break;
        default:
            break;
    }

    return 0;
}

static inline int ab_abs(int a,int b)
{

    return a > b ? a - b : b - a;

}

static int compler_image(window_node_t *node,window_node_t *temp)
{

#if 1
    if(node->move_arrt != NOT_MOVE && node->win_type == OBJECT_LINE)
    {

        node->en_intersection = 1;
        temp->en_intersection = 1;
        return 1;
    }
#endif

    struct  update_param_save   data_s,data_n;
    update_param_get(temp,&data_n);
    update_param_get(node,&data_s);
    int x1,y1,x2,y2;
    x1 = (data_s.end_x - data_s.x)/2 + data_s.x;
    x2 = (data_n.end_x - data_n.x)/2 + data_n.x;

    y1 = (data_s.end_y - data_s.y)/2 + data_s.y;
    y2 = (data_n.end_y - data_n.y)/2 + data_n.y;

    //xw_logsrv_debug("x1:%d y1:%d x2:%d y2:%d \n",x1,y1,x2,y2); 

    if((ab_abs(x1 , x2) < (data_s.end_x - data_s.x + data_n.end_x - data_n.x)/2) &&
            (ab_abs(y1 , y2) < (data_s.end_y - data_s.y + data_n.end_y - data_n.y)/2)){

        node->en_intersection = 1;
        temp->en_intersection = 1;
        //xw_logsrv_debug("XXXX%s:line:%d node_id:%c%c%c  temp_id:%c%c%c\n",__func__,__LINE__,
        //      node->node_id[0],node->node_id[1],node->node_id[2],
        //    temp->node_id[0],temp->node_id[1],temp->node_id[2] );
        return 1;
    }
    return 0;
}


static void  compler_intersection_attr(window_node_t *node)
{
    window_node_t *temp ,*savenode = node;

    for(; node != NULL; node = node->next )
    {
        if(node->en_intersection == 0)
        {
            for(temp =  savenode ; temp  != NULL ; temp =  temp->next)
            {
                if(temp == node)
                    continue;
                compler_image(node,temp); 
            }    
        }    

    }

    return ;

}



static void  _image_all_window_comper_intersection_attr(void)
{



    if(sdk_handle->root->s_head == NULL)
        return ;


    window_node_t *node = NULL,*ft_stack[MENU_LEVEL];
    int stack_cnt = 1;
    ft_stack[0] = sdk_handle->root;

    node = sdk_handle->root->s_head;
    //set attr
    while ( node  )
    {
        if(node  == node->f_node->s_head){

            compler_intersection_attr(node); 

        }
        // intersection compler all
        if(/*node->en_submenu &&*/ node->s_head != NULL){
            ft_stack[stack_cnt] = node;
            node = node->s_head;
            stack_cnt++;
        }else{
            node = node->next;
        }
        if(node == NULL){
            stack_cnt--;
            node = ft_stack[stack_cnt]->next;
        }         

    }

    return;


}


//the func get limit rect for now freshen node,only set now window is prev node
static int  get_limit_rect_for_node_freshen(window_node_t *node)
{   
    if(node == NULL)
        return -1;


    if( node->video_attr == CLOSE_DISP)
        return -2;
    window_node_t *temp = NULL;
    //*temp2 = NULL;

    if(node->f_node == sdk_handle->root || node->f_node->video_attr != CLOSE_DISP)
    {
        temp = node->prev;
    }else if(node->f_node->video_attr == CLOSE_DISP){
        temp  = node->f_node->prev;
        //temp2 = node->prev;
    }else{
        ;
    }

    struct update_param_save    save;
    limit_rect_t                *limit = NULL ,*templimit = NULL;

    for( ; temp != NULL ; temp = temp->prev)
    {

        if(temp->en_node == 0 || temp->video_state == CLEAR_STATE ||
                temp->video_attr == CLOSE_DISP)
            continue;

        //count insterince
        if(compler_image(node,temp))
        {
            if( update_param_get(temp,&save) != 0)
                continue;
            //debug_node_id(node);
            //debug_node_id(temp);

            limit  = (limit_rect_t *)object_pool_get(sdk_handle->limit_rect_pool);
            if(limit == NULL){
                //debug_node_id(node);
                //debug_node_id(temp);
                break;

            }
            limit->x = save.x;
            limit->y = save.y;
            limit->end_y = save.end_y ;
            limit->end_x = save.end_x ;
            if(node->limit == NULL)
            {
                node->limit = limit;
            }else{
                for(templimit = node->limit; templimit->next != NULL; templimit = templimit->next)
                    ;
                templimit->next = limit;
                limit->prev     = templimit;
            }
        }
    }

    return 0;

}

static int  compler_limit_rect_ture(uint16_t x,uint16_t y,limit_rect_t *limit)
{
    if(limit == NULL)
        return 0;

    limit_rect_t *temp = limit;
    for(;temp != NULL; temp =  temp->next){
        if( x >= temp->x && x <= temp->end_x && y >= temp->y && y <= temp->end_y)
            return 1; 
    }

    return 0;
}

static void inline  limit_rect_free(limit_rect_t *limit)
{

    for(;limit != NULL ; limit = limit->next){
        object_pool_free(sdk_handle->limit_rect_pool,(void * )limit);
        limit->next = NULL;
        limit->prev = NULL;

    }
    return ;

}




void    Image_SDK_Run(void)
{


    sdk_handle->en = 1;

    pthread_create(&mouse_thread_id,NULL,
            _image_mouse_event_read_thread,NULL);

    //set all window intersction attr

    _image_all_window_comper_intersection_attr();

    pthread_create(&process_thread_id,NULL,
            _image_sdk_handle_data_process_thread,NULL);

#if 0  //block use test
    for(;;)
        sleep(1);
    pthread_join(mouse_thread_id,NULL);
    pthread_join(process_thread_id,NULL);

#endif

    return ;

}



static struct fb_fix_screeninfo fixInfo;
static struct fb_var_screeninfo varInfo;


static void*  _image_fb_init(char *dev_path,int *fd)
{

    int     fbHandle = -1;
    void    *fbBuffer = NULL;
    
    

    fbHandle = open (dev_path, O_RDWR);
    if (fbHandle < 0)
    {
        *fd = -1;
        xw_logsrv_err("open /dev/fb0 error:\n");
        return NULL;
    }
    sdk_handle->video_fd = fbHandle;
        if(ioctl(fbHandle, FBIOGET_FSCREENINFO, &fixInfo) < 0)
    {
        xw_logsrv_err("Cannot get fixed screen info\n.");
        close (fbHandle);
        *fd = -1;

        return NULL;
    }

    varInfo.xres =  VO_SCREE_W;
    varInfo.yres =  VO_SCREE_H;
    varInfo.xres_virtual = VO_SCREE_W;
    varInfo.yres_virtual = VO_SCREE_H;
    varInfo.bits_per_pixel = VO_COLOR_FMT;
    varInfo.width = VO_SCREE_W;
    varInfo.height = VO_SCREE_H;

    if(ioctl(fbHandle, FBIOGET_VSCREENINFO, &varInfo) < 0)
    {
        xw_logsrv_err("Cannot put var screen info\n.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }
    varInfo.xres =  VO_SCREE_W;
    varInfo.yres =  VO_SCREE_H;
    varInfo.xres_virtual = VO_SCREE_W;
    varInfo.yres_virtual =  VO_SCREE_H;
    varInfo.bits_per_pixel = VO_COLOR_FMT;
    varInfo.width = 1024; //VO_SCREE_W;
    varInfo.height = 600;//VO_SCREE_H;

    //varInfo.xres_virtual = 1920;
    //varInfo.yres_virtual = 1080;
   // varInfo.xres = 1024;
   // varInfo.yres = 600;
   // varInfo.width = 1920;
   // varInfo.height = 1080;
#if 1
    if(ioctl(fbHandle, FBIOPUT_VSCREENINFO, &varInfo) < 0)
    {
        xw_logsrv_err("Cannot get var screen info\n.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }

#endif
#if 1
    if(ioctl(fbHandle, FBIOGET_VSCREENINFO, &varInfo) < 0)
    {
        xw_logsrv_err("Cannot put var screen info\n.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }
#endif


    xw_logsrv_debug("framebuffer: %d x %d @ %d -- %dbpp\n", varInfo.xres, varInfo.yres,
            fixInfo.line_length, varInfo.bits_per_pixel);

    fbBuffer = (unsigned short *)mmap(NULL, fixInfo.smem_len,
            PROT_WRITE,  MAP_SHARED, fbHandle, 0);
    xw_logsrv_debug("framebuffer addr:0x%08x, len=%d\n", (int)fbBuffer, fixInfo.smem_len);
    if (fbBuffer == MAP_FAILED)
    {
        xw_logsrv_err("Cannot mmap framebuffer memory.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }
    memset(fbBuffer,0x0,fixInfo.smem_len);
   

   #if 0//add test 
    uint16_t y ,x ;
    uint16_t *baseAddr = (uint16_t *)fbBuffer;
    for(y = 0 ;y < varInfo.yres/2;y++){
        for(x = 0 ;x < varInfo.xres/2; x++){
            *(baseAddr + x +y*(fixInfo.line_length/2)) = 0xf00f; 
        }
        for(; x < varInfo.xres; x++){
            
             *(baseAddr + x +y*(fixInfo.line_length/2)) = 0xf0f0; 
   
        }
    
    }
    for(; y < varInfo.yres;y++){
        for(x = 0; x < varInfo.xres/2;x++){
            *(baseAddr + x +y*(fixInfo.line_length/2)) = 0xff00; 
        }
        for(; x < varInfo.xres;x++){
            *(baseAddr + x + y*(fixInfo.line_length/2)) = 0xffff;
        }
    
    }

#endif
 
    *fd = fbHandle;

    return fbBuffer;
}

static void*  _image_fb_deinit(void *mmap_p,int fd)

{
    if(mmap_p == NULL || fd < 0)
        return NULL;


    munmap(mmap_p, VO_SCREE_H * VO_SCREE_W *(VO_COLOR_FMT/8));
    close(fd);
    return NULL;
}
void Image_Fb_Push(int xoffset,int yoffset){


    varInfo.yoffset      = yoffset;
    varInfo.xoffset      = xoffset;
    if(ioctl(sdk_handle->video_fd, FBIOPAN_DISPLAY, &varInfo) < 0)
    {
        xw_logsrv_err("Cannot display Pan\n.");
        return ;
    }
    return ;
}


int     Image_SDK_Reset(void)
{
    
    sdk_handle->en = SDK_WORK_STATE_STOP;
    while(sdk_handle->mouse_thread_state != SDK_WORK_STATE_STOP)
        usleep(100000);
    while(sdk_handle->event_thread_state != SDK_WORK_STATE_STOP)
        usleep(100000);
    
    sdk_handle->root = NULL;
    
    object_pool_reset(sdk_handle->object_pool);
    object_pool_reset(sdk_handle->window_node_pool);
    //init root node
    window_node_t *root = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    memset(root, 0x0 ,sizeof(window_node_t));
    memcpy(root->node_id,"A",1);
    root->en_node = 1;
    sdk_handle->root = root;
    sdk_handle->en = SDK_WORK_STATE_RUN;

    return 0;
}



