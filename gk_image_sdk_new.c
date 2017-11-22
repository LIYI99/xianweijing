
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include"gk_image_sdk_new.h"

//device info set 
#define     VO_SCREE_W          1920
#define     VO_SCREE_H          1080
#define     VO_DISP_FPS         25
#define     VO_COLOR_FMT        16
#define     GK_DEVICE_MOUSE     "/dev/input/mouse0"
#define     GK_DEVICE_FB        "/dev/fb0"
#define     GK_MOUSE_SPEED      1

//image disp info set
#define     DISP_OBJ_MAX        100

#define     MOUSE_SIZE          30

#define     MOUSE_IMAGE_PATH_1  NULL
#define     MOUSE_IMAGE_PATH_2  NULL
#define     MOUSE_IMAGE_PATH_3  NULL


static      image_sdk_t         *sdk_handle = NULL;

//video fb
static void*  _gk_fb_init(char *dev_path,int *fd);
static void*  _gk_fb_deinit(void *mmap_p,int fd);
static void   _gk_fb_push(int xoffset,int yoffset);

// mouse image load
static void   _gk_mouse_image_load(window_node_mouse_t *mouse);

//image 

#if 0
    static void   _analysis_mdata(GK_MOUSE_DATA mdata,struct obj_func_set *set);
    static void   _obj_func_run(struct obj_func_set *set);
    static void   _freshen_image(void);
#endif



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
    sdk_handle->mmap_p = _gk_fb_init(GK_DEVICE_FB,&sdk_handle->video_fd);
    if(sdk_handle->video_fd < 0)
        goto ERR1;
    
    sdk_handle->mouse_fd =  gk_mouse_open(GK_DEVICE_MOUSE,VO_SCREE_W,
            VO_SCREE_H,GK_MOUSE_SPEED,MOUSE_SIZE);
   
    if(sdk_handle->mouse_fd < 0){
        printf("not use mouse\n");
    }

    sdk_handle->window_node_pool =  
        object_pool_create(sizeof(window_node_t),DISP_OBJ_MAX+1,NULL);
    if(sdk_handle->window_node_pool == NULL)
        goto ERR2;
    
    unsigned int obj_size = 0;
    obj_size = sizeof(window_node_button_t) > sizeof(window_node_menu_t)? 
        sizeof(window_node_button_t) : sizeof(window_node_menu_t);
    obj_size = obj_size > sizeof(window_node_line_t)? obj_size : sizeof(window_node_line_t);
    
    obj_size = obj_size > sizeof(window_node_text_t)? obj_size : sizeof(window_node_text_t);
    
    sdk_handle->object_pool = object_pool_create(obj_size,DISP_OBJ_MAX+1,NULL);

    if(sdk_handle->object_pool == NULL)
        goto ERR3;
    
    //init root node
    window_node_t *root = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    memset(root, 0x0 ,sizeof(window_node_t));
    memcpy(root->node_id,'A',1);
    root->en_node = 1;

    //add mouse
    window_node_mouse_t *mouse = NULL;
    mouse = (window_node_mouse_t *)malloc(sizeof(window_node_mouse_t));
    if(mouse == NULL){
        printf("fisrt get obj fail???\n");
    }
    
    memset(mouse,0x0,sizeof(window_node_mouse_t));
    mouse->size = MOUSE_SIZE;
    mouse->save_cache = (char *)malloc(mouse->size *mouse->size);
    _gk_mouse_image_load(mouse);
    sdk_handle->mouse = mouse;
    sdk_handle->en = 1;
    
    return;

ERR3:
    object_pool_destory(sdk_handle->window_node_pool);

ERR2:
   if(sdk_handle->mouse_fd > 0)
        gk_mouse_close(sdk_handle->mouse_fd); 
    
   _gk_fb_deinit(sdk_handle->mmap_p,sdk_handle->video_fd);

ERR1:
    free(sdk_handle);
    sdk_handle = NULL;
    return ;
}

void    Image_SDK_deInit(void)
{
    if(sdk_handle == NULL)
        return;
     _gk_fb_deinit(sdk_handle->mmap_p,sdk_handle->video_fd);
    
     if(sdk_handle->mouse_fd > 0)
            gk_mouse_close(sdk_handle->mouse_fd); 

    object_pool_destory(sdk_handle->window_node_pool);
    object_pool_destory(sdk_handle->object_pool);
    free(sdk_handle->mouse->save_cache);
    free(sdk_handle->mouse);
    free(sdk_handle);
    sdk_handle = NULL;
    return;
}



static int inline  node_id_level_re(char *node_id){
    
    int i;
    for(i = 0 ;i  < MENU_LEVEL ; i++){
        if(node_id[i] < 'A'  || node_id[i] > 'z' )
            break;
    }
    return i;
}

static inline window_node_t  *key_find_node(window_node_t *father,char key,int level)
{
   window_node_t *temp = father->s_head;
    
   for (;temp != NULL; temp = temp->next)
   {
        if(temp->node_id[level] ==  key)
            break;
   }
    return temp;
}

static window_node_t  *find_father_node(char *node_id,int level){
    
    if(level == 2)
        return sdk_handle->root;
    
    window_node_t *temp  = sdk_handle->root;
    
    int k = 1;
    for (k = 1 ; k < level -1 ; k ++ ){
        temp = key_find_node(sdk_handle->root,node_id[k],k);
    }
    return temp;
}

static int  window_node_inster(window_node_t *node){

    int level ,k ;
    //get window level
    level = node_id_level_re(node->node_id);
    if(level < 2){
        printf(" node_id wirte erro \n");
        return -1;
    }

    //get window father window
    window_node_t *ftemp = NULL,*same = NULL;
    ftemp = find_father_node(node->node_id,level);
    if(ftemp == NULL){
        printf("not find father node \n");
        return -2;
    }
    //find same window
    same = key_find_node(ftemp,node->node_id[level -1],level -1);
    if(same != NULL){
        printf("a same node ,you set name error \n");
        return -3;
    }
    //inster node 
    node->f_node = ftemp;

    if(ftemp->s_head == NULL){
        ftemp->s_head = node;
        ftemp->s_end = node;
        return 0;
    }

    node->next = ftemp->s_head;
    node->prev = NULL;
    ftemp->s_head->prev = node;
    ftemp->s_head = node;
    
    return 0;
}

int    Image_SDK_Create_Button( struct user_set_node_atrr attr,
        window_node_button_t _button){

    if(sdk_handle == NULL)
        return -2;
    
    window_node_t  *lq = NULL;
    lq = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    if(lq == NULL){
        
        return -3;
    }
   
    window_node_button_t * button = NULL;
    button = (window_node_button_t *)object_pool_get(sdk_handle->object_pool);
    if(button == NULL){
        object_pool_free(sdk_handle->window_node_pool,lq);
        return -5;
    }
    
    memcpy(lq->node_id ,attr.node_id,MENU_LEVEL);
    int ret = 0;
    ret = window_node_inster(lq); 
    if(ret < 0 ){
        object_pool_free(sdk_handle->window_node_pool,lq);
        object_pool_free(sdk_handle->object_pool);
        return -6;
    }

    *button =  _button;
    button->this_node = lq;
    if(_button.user_data == NULL)
        button->user_data  = (void *)button;

    if(button->x > sdk_handle->scree_w){
        button->x = sdk_handle->scree_w - button->w;
        printf("you set x > srcee_w\n");
    }
    if(button->y > sdk_handle->scree_h){
        button->y = sdk_handle->scree_h -button->h;
        printf("you set y > srcee_h\n");
    }
    if(button->w > sdk_handle->scree_w){
        button->w = 60;
        printf("you set w > srcee_w,now set 60 pixl\n");

    }
    if(button->h > sdk_handle->scree_h){
        button->h = 30;
        printf("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = atrr.en_node;
    lq->en_freshen = attr.en_freshen;
    lq->win_type = OBJECT_BUTTION;
    lq->window = (void *)button;

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
        object_pool_free(sdk_handle->object_pool);
        return -6;
    }

    *menu =  _menu;
    menu->this_node = lq; 
    if(_menu.user_data == NULL)
        menu->user_data  = (void *)menu;

    if(menu->x > sdk_handle->scree_w){
        menu->x = sdk_handle->scree_w - menu->w;
        printf("you set x > srcee_w\n");
    }
    if(menu->y > sdk_handle->scree_h){
        menu->y = sdk_handle->scree_h -menu->h;
        printf("you set y > srcee_h\n");
    }
    if(menu->w > sdk_handle->scree_w){
        menu->w = 60;
        printf("you set w > srcee_w,now set 60 pixl\n");

    }
    if(menu->h > sdk_handle->scree_h){
        menu->h = 30;
        printf("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = atrr.en_node;
    lq->en_freshen = attr.en_freshen;
    lq->win_type = OBJECT_MENU;
    lq->window = (void *)menu;

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
        object_pool_free(sdk_handle->object_pool);
        return -6;
    }

    *line =  _line;
    line->this_node = lq; 
    if(_line.user_data == NULL)
        line->user_data  = (void *)line;

    if(line->start_x > sdk_handle->scree_w){
        line->x = sdk_handle->scree_w -line->size;
        printf("you set x > srcee_w\n");
    }
    if(line->y > sdk_handle->scree_h){
        line->y = sdk_handle->scree_h -line->size;
        printf("you set y > srcee_h\n");
    }
    if(line->end_x > sdk_handle->scree_w){
        line->w = sdk_handle->scree_w - line->size;
        printf("you set w > srcee_w,now set 60 pixl\n");

    }
    if(line->end_y > sdk_handle->scree_h){
        line->h = sdk_handle->scree_h - line->size;
        printf("you set h > srcee_h,now set 30pixl\n");

    }

    lq->en_node = atrr.en_node;
    lq->en_freshen = attr.en_freshen;
    lq->win_type = OBJECT_MENU;
        lq->window = (void *)line;
    
    return 0;


}
int    Image_SDK_Create_Text(struct user_set_node_atrr arrt,
        window_node_text_t _text)
{
    
    if(sdk_handle == NULL)
        return -2;
    
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
        object_pool_free(sdk_handle->object_pool);
        return -6;
    }

    *text =  _text;
     text->this_node = lq;

    if(_text.user_data == NULL)
        text->user_data  = (void *)text;

    if(text->x > sdk_handle->scree_w){
        text->x = sdk_handle->scree_w - text->lens *text->size;
        printf("you set x > srcee_w\n");
    }
    if(text->y > sdk_handle->scree_h){
        text->y = sdk_handle->scree_h -text->size;
        printf("you set y > srcee_h\n");
    }
   
    lq->en_node = atrr.en_node;
    lq->en_freshen = attr.en_freshen;
    lq->win_type = OBJECT_MENU;
    lq->window = (void *)text;

    return 0;


}


static window_node_t  *find_all_key_node(char *node_id,int level){
    
    if(level == 2)
        return sdk_handle->root;
    
    window_node_t *temp  = sdk_handle->root;
    
    int k ;
    for (k = 1 ; k < level ; k ++ ){
        temp = key_find_node(sdk_handle->root,node_id[k],k);
    }
    return temp;
}


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

int     Image_SDK_Set_Node_En(char *node_id,uint8_t en)
{

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
        return -2;
    temp->en_node = en;
    
    return 0;

}

int     Image_SDK_Set_Node_En_Freshen(char *node_id,uint8_t en_freshen){

    int level = 0;
    level = node_id_level_re(node_id);
    window_node_t *temp = NULL;
    temp =  find_all_key_node(node_id,level);
    if(temp == NULL)
        return -2;
    temp->en_freshen = en_freshen;
    
    return 0;

}



static inline int gk_buttont_xy_analysis(void *data,
        struct obj_func_set *set){
    
    image_sdk_button_t *bt = (image_sdk_button_t *)data;
    GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    
    

    if( (mdata.x  >= bt->x || mdata.x+ MOUSE_SIZE >= bt->x)  && mdata.x <= bt->x+bt->w 
            && (mdata.y >= bt->y || mdata.y + MOUSE_SIZE >= bt->y)&& mdata.y <= bt->y+bt->h){
              
        set->offset = bt->mouse_offset;
        set->ldown = bt->mouse_left_down;
        set->lup = bt->mouse_left_up;
        set->rdown = bt->mouse_right_down;
        set->rup = bt->mouse_right_up;
        set->leave = bt->mouse_leave;
        set->data = bt->data;
        
        return 1;
    }
    else 
        return 0;

}

static inline int gk_menu_xy_analysis(void *data,
        struct obj_func_set *set){
    
    image_sdk_menu_t *bt = (image_sdk_menu_t *)data;
    GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    
    if( mdata.x  >= bt->x  && mdata.x <= bt->x+bt->w 
            && mdata.y >= bt->y && mdata.y <= bt->y+bt->h){
               set->offset = bt->mouse_offset;
        set->ldown = bt->mouse_left_down;
        set->lup = bt->mouse_left_up;
        set->rdown = bt->mouse_right_down;
        set->rup = bt->mouse_right_up;
        set->data = bt->data;
        set->leave = bt->mouse_leave;

        return 1;
    }
    else 
        return 0;

}

static inline int gk_line_xy_analysis(void *data,
        struct obj_func_set *set){
    
    
    image_sdk_line_t *bt = (image_sdk_line_t *)data;
    
    GK_MOUSE_DATA  mdata = sdk_handle->mouse_new_data;
    //printf("chcek  line set->data:%p line:%p\n" ,set->data,bt);

    if( mdata.x  >= bt->start_x  && mdata.x <= bt->end_x 
            && mdata.y >= bt->start_y-5 && mdata.y <= bt->start_y+bt->size+5){
               set->offset = bt->mouse_offset;
        set->ldown = bt->mouse_left_down;
        set->lup = bt->mouse_left_up;
        set->rdown = bt->mouse_right_down;
        set->rup = bt->mouse_right_up;
        set->data = bt->data;
        set->leave = bt->mouse_leave;
      //  printf("chcek  line set->data:%p line:%p,stat_x:%d\n" ,set->data,bt,bt->start_x);
        return 1;
    }
    else 
        return 0;

    
    return 0;

}




static void _analysis_mdata(GK_MOUSE_DATA mdata,struct obj_func_set *set)

{
    
    if(sdk_handle->head == NULL)
        return;
    
    struct lqueue *node = NULL,*temp= NULL;
    
    node = sdk_handle->head;
    
    int ret = 0 ;

    while(node){
        
        if(node->object_s == OBJECT_BUTTION)
        {
            ret = gk_buttont_xy_analysis(node->data,set);
             
        }else if(node->object_s == OBJECT_MENU)
        {
            ret = gk_menu_xy_analysis(node->data,set);
        
        }else
        {
            ret = gk_line_xy_analysis(node->data,set);
        }
        //build leave event
        if(node == sdk_handle->last_run_node  && ret == 0)
        {
            
            *set  = sdk_handle->set;
            node->mouse_en = 1;
            node->need_push =  2;
            sdk_handle->last_run_node = NULL;
            sdk_handle->mouse_new_data.event = GK_MOUSE_LEAVE;
            break;        
        }

        if(ret > 0){
            
            node->mouse_en = 1;
            node->need_push = 1; 
            sdk_handle->last_run_node = node;
            sdk_handle->set = *set;

            if(sdk_handle->head == sdk_handle->end)
            
                break;
            
            if(node == sdk_handle->head)
                break;
                       
            if(node == sdk_handle->end)
            {
                
                node->prev->next = NULL;
                sdk_handle->end = node->prev;
                node->next = sdk_handle->head;
                node->prev = NULL;
                sdk_handle->head->prev =  node;
                sdk_handle->head = node;
            }else{

                temp = node;
                node->prev->next = temp->next;
                node->next->prev = temp->prev;
                node->next = sdk_handle->head;
                node->prev = NULL;
                sdk_handle->head->prev = node;
                sdk_handle->head = node;
            }
            
            break;

        }
        ret = 0;

        node = node->next;
    }

}

static void  _obj_func_run(struct obj_func_set *set)
{
    
    if(sdk_handle->head == NULL)
        return;
    struct lqueue *node = sdk_handle->head;
    
   // printf("line:%d set->offset_func:%p  node:%p  mouse chcek:%d mouse event:%d\n",__LINE__,set->offset,
     //       node,node->mouse_en,sdk_handle->mouse_new_data.event);    


    if( !node->mouse_en)
        return;
    
    if(sdk_handle->mouse_new_data.event == GK_MOUSE_OFFSET){
        if(set->offset)    
            set->offset(set->data);
    }else if(sdk_handle->mouse_new_data.event ==  GK_MOUSE_LEFT_DOWN){
        if(set->ldown)    
            set->ldown(set->data);
    }else if (sdk_handle->mouse_new_data.event ==  GK_MOUSE_LEFT_UP){
        if(set->lup)
            set->lup(set->data);
    }else if(sdk_handle->mouse_new_data.event ==  GK_MOUSE_RIGHT_DOWN){
        if(set->rdown)    
            set->rdown(set->data);
    }else if(sdk_handle->mouse_new_data.event == GK_MOUSE_RIGHT_UP){
        if(set->rup)
            set->rup(set->data);
    }else if(sdk_handle->mouse_new_data.event == GK_MOUSE_LEAVE){
        if(set->leave)
            set->leave(set->data);
    }
    node->mouse_en = 0;
    
    return ;

}



static void freshen_image_buttion(void *data){
    
    
    image_sdk_button_t *bt =  (image_sdk_button_t *)data;
    int16_t *buf = (int16_t *)sdk_handle->mmap_p; 
    
    
    int k,i;
    //clean before
    if(bt->en_follow){
        
        for(k = bt->y ; k < (bt->h + bt->y) ;k++){
            for(i = bt->x ;  i < (bt->w + bt->x) ; i++ )
                *(buf+ sdk_handle->scree_w*k + i) = 0;    
        
        }
        
        bt->y = sdk_handle->mouse_new_data.y;
        bt->x = sdk_handle->mouse_new_data.x;
    }
    for(k = bt->y ; k < (bt->h + bt->y) ;k++){
        for(i = bt->x ;  i < (bt->w + bt->x) ; i++ )
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;    
    }
    
    return ;
}

static void freshen_image_menu(void *data){
    
    image_sdk_menu_t *bt =  (image_sdk_menu_t *)data;
    int16_t *buf = (int16_t *)sdk_handle->mmap_p; 
    
    int k,i;
    //after modfiy memcpy 
    for(k = bt->y ; k < (bt->h + bt->y) ;k++){
        for(i = bt->x ;  i < (bt->w + bt->x) ; i++ )
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;    
    }
 //   printf("push good menu \n");
    return ;
       
}
static void freshen_image_line(void *data){
    
    image_sdk_line_t *bt =  (image_sdk_line_t *)data;
    int16_t *buf = (int16_t *)sdk_handle->mmap_p; 
    
    int k,i;
    
    if(bt->en_follow){
    
        //memset(buf,0x0,VO_SCREE_H * VO_SCREE_W*2);
    for(k = bt->start_y ; k < (bt->start_y + bt->size) ;k++){
        
        for(i = bt->start_x ;  i < bt->end_x ; i++ )
            *(buf+ sdk_handle->scree_w*k + i) = 0;    
    }
        bt->start_y = sdk_handle->mouse_new_data.y;
        bt->end_y = sdk_handle->mouse_new_data.y;
        
    }

    //after modfiy memcpy 
    for(k = bt->start_y ; k < (bt->start_y + bt->size) ;k++){
        for(i = bt->start_x ;  i < bt->end_x ; i++ )
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;    
    }
 //   printf("push good menu \n");


    return ;
}

static void freshen_image_mouse(void)
{
    
    
    
    sdk_handle->mouse->x = sdk_handle->mouse_new_data.x;
    sdk_handle->mouse->y = sdk_handle->mouse_new_data.y;
    image_sdk_mouse_t *bt = sdk_handle->mouse;
    int16_t *buf = (int16_t *)sdk_handle->mmap_p; 
    int k,i,j;
    
    //reset data
    //clear
#if 1 
    for(k = bt->y_o ,j = 0 ; k < (bt->h + bt->y_o) ; k++)
    {
        for(i = bt->x_o ;  i < (bt->w + bt->x_o) ; i++ ,j++)
            *(buf+ sdk_handle->scree_w*k + i) = 0;
    }
#endif
#if 0
     //reset data
    for(k = bt->y_o ,j = 0 ; k < (bt->h + bt->y_o) ; k++)
    {
        for(i = bt->x_o ;  i < (bt->w + bt->x_o) ; i++ ,j++)
            *(buf+ sdk_handle->scree_w*k + i) = *(uint16_t *)(((uint16_t *)bt->save_buf)+j);
    }
    
    //save 
    for(k = bt->y ,j = 0; k < (bt->h + bt->y) ;k++)
    {
        for(i = bt->x ;  i < (bt->w + bt->x) ; i++,j++ ){
           *(uint16_t *)(((uint16_t *)bt->save_buf)+j) =  *(buf+ sdk_handle->scree_w*k + i);
        }
    }
#endif

    //push 
    for(k = bt->y ; k < (bt->h + bt->y) ;k++){
        for(i = bt->x ;  i < (bt->w + bt->x) ; i++ )
            *(buf+ sdk_handle->scree_w*k + i) = bt->color;    
    }
    
    bt->x_o = bt->x;
    bt->y_o = bt->y;
//    printf("push good mouse \n");
    return ;
}



static void  _freshen_image(void)
{
    
    if(sdk_handle->end == NULL)
        return ;
    
  
    struct lqueue *temp = NULL,*node = NULL;
    
    temp = sdk_handle->end;
   

    

    for(;temp != NULL;temp = temp->prev){
        
        if(temp->need_push == 0)
            continue;
        if(temp->need_push == 2){
            node = temp;
            continue;
        }
        if(temp->object_s ==  OBJECT_BUTTION){
            freshen_image_buttion(temp->data);        
        }else if(temp->object_s == OBJECT_MENU){
            freshen_image_menu(temp->data);
        }else {
            freshen_image_line(temp->data);
        }
        temp->need_push =  0;

    }
   

    freshen_image_mouse();
    
    if(node != NULL){
        
        if(node->object_s ==  OBJECT_BUTTION){
            freshen_image_buttion(node->data);        
        }else if(node->object_s == OBJECT_MENU){
            freshen_image_menu(node->data);
        }else {
            freshen_image_line(node->data);
        }
        node->need_push =  0;
        //printf("push data node:%p\n",node);
    }

    return ;

}

void    Image_SDK_Run(void)
{
    
    if(sdk_handle == NULL)
        return;
    
    int retval = 0;
    fd_set      readfds;
    struct  timeval     tv, tv1,tv2;
    GK_MOUSE_DATA       mdata;
    struct obj_func_set set_s;
    memset(&set_s,0x0,sizeof(struct obj_func_set));
    memset(&mdata,0x0,sizeof(GK_MOUSE_DATA));


    //_gk_fb_push( 0 ,0);
    while(1)
    {
        
        
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        if(sdk_handle->mouse_fd > 0){
            FD_ZERO(&readfds);
            FD_SET(sdk_handle->mouse_fd, &readfds);
            retval = select(sdk_handle->mouse_fd+1,&readfds,NULL,NULL,&tv);
            //get data
            if(FD_ISSET(sdk_handle->mouse_fd,&readfds))
            {
                
                if(gk_mouse_read_data(sdk_handle->mouse_fd,&mdata) < 0 )
                {
                    continue;
                }
                sdk_handle->mouse_new_data = mdata;
                //NEED_FRE_F = 0;
            }
        }else{
            usleep(100000);
            retval = 0;
        }

#if 1
        if( retval > 0 )
        {
            _analysis_mdata(mdata,&set_s);
            _obj_func_run(&set_s);
        }
#endif

        if(retval >= 0 )
        {
            _freshen_image();

        }


    }
    return ;

}


int    Image_SDK_Create_Button( image_sdk_button_t _button)
{
    
    if(sdk_handle == NULL)
        return -2;
    image_sdk_button_t * button = NULL;
    button = (image_sdk_button_t *)object_pool_get(sdk_handle->imageobj_pool);
    if(button == NULL)
        return -3;
    
    struct lqueue  *lq = NULL;
    lq = (struct lqueue *)object_pool_get(sdk_handle->lqueue_pool);
    if(lq == NULL){
        object_pool_free(sdk_handle->imageobj_pool,button);
        return -5;
    }
    
    *button =  _button;
    
    if(_button.data == NULL)
        button->data  = (void *)button;


    if(button->x > sdk_handle->scree_w){
        button->x = sdk_handle->scree_w - button->w;
        printf("you set x > srcee_w\n");
    }
    if(button->y > sdk_handle->scree_h){
        button->y = sdk_handle->scree_h -button->h;
        printf("you set y > srcee_h\n");
    }
    if(button->w > sdk_handle->scree_w){
        button->w = 60;
        printf("you set w > srcee_w,now set 60 pixl\n");

    }
    if(button->h > sdk_handle->scree_h){
        button->h = 30;
        printf("you set h > srcee_h,now set 30pixl\n");

    }

    lq->mouse_en = 0;
    lq->need_push = 1;
    lq->object_s = OBJECT_BUTTION;
    lq->prev = NULL;
    lq->next = NULL;
    lq->data = (void *)button;

    if(sdk_handle->head == NULL){
        
        sdk_handle->head = sdk_handle->end = lq;
    }else{
        lq->next = sdk_handle->head;
        sdk_handle->head->prev = lq;
        sdk_handle->head = lq;


    }
    
  //  printf("+++=add botton: head:%p  node:%p node->prev:%p,node->next:%p\n\n"
    //        ,sdk_handle->head,lq,lq->prev,lq->next); 
    return 0;

}

int    Image_SDK_Create_Menu(image_sdk_menu_t _menu){

  if(sdk_handle == NULL)
        return -2;
    image_sdk_menu_t * menu = NULL;
    menu = (image_sdk_menu_t *)object_pool_get(sdk_handle->imageobj_pool);
    if(menu == NULL)
        return -3;
    
    struct lqueue  *lq = NULL;
    lq = (struct lqueue *)object_pool_get(sdk_handle->lqueue_pool);
    if(lq == NULL){
        object_pool_free(sdk_handle->imageobj_pool,menu);
        return -5;
    }
    
    *menu =  _menu;

    if(menu->x > sdk_handle->scree_w){
        menu->x = sdk_handle->scree_w -menu->w;
        printf("you set x > srcee_w\n");
    }
    if(menu->y > sdk_handle->scree_h){
        menu->y = sdk_handle->scree_h -menu->h;
        printf("you set y > srcee_h\n");
    }
    if(menu->w > sdk_handle->scree_w){
        menu->w = 60;
        printf("you set w > srcee_w,now set 60 pixl\n");

    }
    if(menu->h > sdk_handle->scree_h){
        menu->h = 30;
        printf("you set h > srcee_h,now set 30pixl\n");

    }

    lq->mouse_en = 0;
    lq->need_push = 1;

    lq->object_s = OBJECT_BUTTION;
    lq->prev = NULL;
    lq->next = NULL;
    lq->data = (void *)menu;

    if(sdk_handle->head == NULL){
        
        sdk_handle->head = sdk_handle->end = lq;
    }else{
        lq->next = sdk_handle->head;
        sdk_handle->head->prev = lq;
        sdk_handle->head = lq;

    }
    
    return 0;


}

int    Image_SDK_Create_Line(image_sdk_line_t _line){
    
   
    if(sdk_handle == NULL)
        return -2;
    image_sdk_line_t * line = NULL;
    line = (image_sdk_line_t *)object_pool_get(sdk_handle->imageobj_pool);
    if(line == NULL)
        return -3;
    
    struct lqueue  *lq = NULL;
    lq = (struct lqueue *)object_pool_get(sdk_handle->lqueue_pool);
    if(lq == NULL){
        object_pool_free(sdk_handle->imageobj_pool,line);
        return -5;
    }
    
    *line =  _line;
    
    if(_line.data == NULL)
        line->data  = (void *)line;




    if(line->start_x > sdk_handle->scree_w){
        line->start_x = sdk_handle->scree_w;
        printf("you set x > srcee_w\n");
    }
    if(line->start_y > sdk_handle->scree_h){
        line->start_y = sdk_handle->scree_h ;
        printf("you set y > srcee_h\n");
    }
   
    if(line->end_x > sdk_handle->scree_w){
        line->end_x = sdk_handle->scree_w;
        printf("you set x > srcee_w\n");
    }
    if(line->end_y > sdk_handle->scree_h){
        line->end_y = sdk_handle->scree_h ;
        printf("you set y > srcee_h\n");
    }


    if(line->size + line->start_y > sdk_handle->scree_h ){
        line->size = 1;

    }

    lq->mouse_en = 0;
    lq->need_push = 1;

    lq->object_s = OBJECT_LINE;
    lq->prev = NULL;
    lq->next = NULL;
    lq->data = (void *)line;
    printf("node:%p,lq->data:%p \n",lq,lq->data);
    if(sdk_handle->head == NULL){
        
        sdk_handle->head = sdk_handle->end = lq;
    }else{
        lq->next = sdk_handle->head;
        sdk_handle->head->prev = lq;
        sdk_handle->head = lq;

    }
    
    return 0;
  
    
   // printf("now not suport line \n");

}


static void*  _gk_fb_init(char *dev_path,int *fd)
{
    
    int     fbHandle = -1;
    void    *fbBuffer = NULL;

    fbHandle = open (dev_path, O_RDWR);
    if (fbHandle < 0)
    {
        *fd = -1;
        perror("open /dev/fb0 error:\n");
        return NULL;
    }

    struct fb_fix_screeninfo fixInfo;
    struct fb_var_screeninfo varInfo;
    if(ioctl(fbHandle, FBIOGET_FSCREENINFO, &fixInfo) < 0)
    {
        printf("Cannot get fixed screen info\n.");
        close (fbHandle);
        *fd = -1;

        return NULL;
    }

    varInfo.xres = VO_SCREE_W;
    varInfo.yres = VO_SCREE_H;
    varInfo.bits_per_pixel = VO_COLOR_FMT;
    
    if(ioctl(fbHandle, FBIOGET_VSCREENINFO, &varInfo) < 0)
    {
        printf("Cannot get var screen info\n.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }
    printf("framebuffer: %d x %d @ %d -- %dbpp\n", varInfo.xres, varInfo.yres,
                fixInfo.line_length, varInfo.bits_per_pixel);

    fbBuffer = (unsigned short *)mmap(NULL, fixInfo.smem_len,
                                     PROT_WRITE,  MAP_SHARED, fbHandle, 0);
    printf("framebuffer addr:0x%08x, len=%d\n", (int)fbBuffer, fixInfo.smem_len);
    if (fbBuffer == MAP_FAILED)
    {
        printf("Cannot mmap framebuffer memory.");
        close (fbHandle);
        *fd = -1;
        return NULL;
    }

    memset(fbBuffer,0x0,fixInfo.smem_len);
    *fd = fbHandle;
    
    return fbBuffer;
}

static void*  _gk_fb_deinit(void *mmap_p,int fd)

{
    if(mmap_p == NULL || fd < 0)
        return NULL;


    munmap(mmap_p, VO_SCREE_H * VO_SCREE_W *(VO_COLOR_FMT/8));
    close(fd);
    return NULL;
}
static void _gk_fb_push(int xoffset,int yoffset){

    
    struct fb_var_screeninfo varInfo;
    varInfo.xres_virtual = VO_SCREE_W;
    varInfo.yres_virtual = VO_SCREE_H;
    varInfo.yoffset      = yoffset;
    varInfo.xoffset      = xoffset;
    if(ioctl(sdk_handle->mmap_p, FBIOPAN_DISPLAY, &varInfo) < 0)
    {
        printf("Cannot display Pan\n.");
        return ;
    }
    return ;
}





