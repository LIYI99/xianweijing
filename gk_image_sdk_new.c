
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
static void*  _image_fb_init(char *dev_path,int *fd);
static void*  _image_fb_deinit(void *mmap_p,int fd);
static void   _image_fb_push(int xoffset,int yoffset);

// mouse image load
static void     _image_mouse_image_load(window_node_mouse_t *mouse);
static void*    _image_mouse_event_read_thread(void *data);

//handle data proess
static void*    _image_sdk_handle_data_process(void *data);










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
    //open mouse fd 
    sdk_handle->mouse_fd =  gk_mouse_open(GK_DEVICE_MOUSE,VO_SCREE_W,
            VO_SCREE_H,GK_MOUSE_SPEED,MOUSE_SIZE);
   
    if(sdk_handle->mouse_fd < 0){
        printf("not use mouse\n");
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
    
    //init root node
    window_node_t *root = (window_node_t *)object_pool_get(sdk_handle->window_node_pool);
    memset(root, 0x0 ,sizeof(window_node_t));
    memcpy(root->node_id,'A',1);
    root->en_node = 1;

    //add mouse window node
    window_node_mouse_t *mouse = NULL;
    mouse = (window_node_mouse_t *)malloc(sizeof(window_node_mouse_t));
    if(mouse == NULL){
        printf("fisrt get obj fail???\n");
    }
    
    memset(mouse,0x0,sizeof(window_node_mouse_t));
    mouse->size = MOUSE_SIZE;
    mouse->save_cache = (char *)malloc(mouse->size *mouse->size);
    _image_mouse_image_load(mouse);
    sdk_handle->mouse = mouse;
    
    //en sdk handle
    sdk_handle->en = 1;
    
    return;

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

void    Image_SDK_deInit(void)
{
    if(sdk_handle == NULL)
        return;
     _image_fb_deinit(sdk_handle->mmap_p,sdk_handle->video_fd);
    
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
    for (k = 1 ; k < level -1 && temp != NULL; k ++ ){
        temp = key_find_node(temp,node_id[k],k);
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
    if(_button.func_set.data == NULL)
        button->func_set.data  = (void *)button;

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
    if(_menu.func_set.data == NULL)
        menu->func_set.data  = (void *)menu;

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
    if(_line.func_set.data == NULL)
        line->func_set.data  = (void *)line;

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

    if(_text.func_set.data == NULL)
        text->func_set.data  = (void *)text;

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
        
        return 1;
    }
    else 
        return 0;

    
    return 0;

}




static void _image_analysis_mdata(GK_MOUSE_DATA mdata)

{
    
    if(sdk_handle->head == NULL)
        return;
   
    memset(sdk_handle->check_node,0x0,sizeof(void *)*MENU_LEVEL);

    window_node_t  *node = NULL,*save_node[MENU_LEVEL+1];
    
    memset(save_node,0x0,sizeof(void *) * MENU_LEVEL+1);
    int check_cnt = 0;

    node = sdk_handle->root->s_head;
    
    while ( node  ){
        
        if(node->win_type == OBJECT_BUTTION)
        {
            ret = gk_buttont_xy_analysis(node->data,set);
             
        }else if(node->win_type == OBJECT_MENU)
        {
            ret = gk_menu_xy_analysis(node->data,set);
        
        }else if (node->win_type == OBJECT_LINE){
            
            ret = gk_line_xy_analysis(node->data,set);
        
        }else if(node->win_type == OBJECT_TXT_WIN){
        
        }else if(node->win_type == OBJECT_BAR){
            
        }else{
            printf("erro ,the node not use\n");
        }
        
        if(ret > 0){
            
            node->check_node = 1;
            save_node[check_cnt] = node;
            check_cnt++;
        }
        // if check node dont have childe node ,so travle end
        if(ret > 0  && node->s_head == NULL ){
            break;
        }
        // if check node have childe node ,than tarlve the son node 
        // else tarvle bother node
        if( ret > 0  && node->s_head != NULL){
            node = node->s_head;
        }else{
            node = node->next;
        }
        // if now node == NULL ,goto father,s bother node
        if(node == NULL){
            node = node->f_node->next;
        }
    
    }
    //move check node
    window_node_t *temp;
    int  k;
    for (k  =  0 ; k < check_cnt ; node = save_node[k],k++)
    {
        
        if(node == node->f_node->s_head)
            continue;
        node->prev->next = node->next;
        if(node->next){
            node->next->prev = node->prev;
        }else{ //is end
            node->f_node->s_end = node->prev;
        }
        node->f_node->s_head->prev = node;
        node->prev = NULL;
        node->next = node->f_node->head;
        node->f_node->s_head = node;
    }
    sdk_handle->check_level_cnt = check_cnt;
    //save now mouse check node
    for (k  =  0 ; k < check_cnt ; k++)
        sdk_handle->check_node[k] = save_node[k];
    
    return ;

 }

static window_func_t inline *get_window_func(window_node_t *node)
{
    
    if(node == NULL)
        return NULL;
    
    window_func_t *func = NULL;
    
    if(node->win_type == OBJECT_BAR){
        func = &(((window_node_bar_t *)node->data)->func_set);
    }else if(node->win_type == OBJECT_BUTTION){
        func = &(((window_node_button_t *)node->data)->func_set);
    }else if(node->win_type == OBJECT_LINE){
        func = &(((window_node_line_t *)node->data)->func_set);
    }else if(node->win_type == OBJECT_TXT_WIN){
        func = &(((window_node_text_t *)node->data)->func_set);
    }else if(node->win_type == OBJECT_MENU){
        func = &(((window_node_menu_t *)node->data)->func_set);

    }else{
        func = NULL;
    }

    return func;;
}

static void  _image_window_func_run(void *data)
{
    
    if(sdk_handle->head == NULL)
        return;
    window_node_t  *node = NULL,*save_leave_node[MENU_LEVEL];
    int k = 0,j = 0 ;

    //leave event
    window_func_t *funcs = NULL;
    for(k = MENU_LEVEL -1  ; k >= 0; k--){
    
        node = sdk_handle->last_check_node[k];
        if(!node->check_node)
            continue;

        if(node){
            funcs = get_window_func(node);
        }
        if(funcs->mouse_leave){
            
            funcs->mouse_leave(funcs->data);
        }
        
    }
    
    if(sdk_handle->check_level_cnt == 0)
        return;
    //only run top event 
    node = sdk_handle->check_node[sdk_handle->check_level_cnt -1];
    if(!node->en_node)
        return;
    
    funcs = get_window_func(node);
    if(funcs == NULL)
        return;
    
    if(sdk_handle->mouse_new_data.event == GK_MOUSE_OFFSET){
        if(funcs->offfuncs)    
            funcs->offfuncs(funcs->data);
    }else if(sdk_handle->mouse_new_data.event ==  GK_MOUSE_LEFT_DOWN){
        if(funcs->ldown)    
            funcs->ldown(funcs->data);
    }else if (sdk_handle->mouse_new_data.event ==  GK_MOUSE_LEFT_UP){
        if(funcs->lup)
            funcs->lup(funcs->data);
    }else if(sdk_handle->mouse_new_data.event ==  GK_MOUSE_RIGHT_DOWN){
        if(funcs->rdown)    
            funcs->rdown(funcs->data);
    }else if(sdk_handle->mouse_new_data.event == GK_MOUSE_RIGHT_UP){
        if(funcs->rup)
            funcs->rup(funcs->data);
    }else if(sdk_handle->mouse_new_data.event == GK_MOUSE_LEAVE){
        if(funcs->leave)
            funcs->leave(funcs->data);
    }
    
    //clear buf
    for(k = 0; k < MENU_LEVEL ; k++){
        sdk_handle->last_check_node[k] = NULL;
    }
    //clear flag and save
    for(k = 0 ;k < sdk_handle->check_level_cnt; k++){
        sdk_handle->check_node[k]->check_node = 0;
        sdk_handle->last_check_node[k] = sdk_handle->check_node[k];
    }
    
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



static void  _image_freshen_image(void)
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

static  void*   _image_mouse_event_read_thread(void *data)
{
    
    GK_MOUSE_DATA       mdata;
    memset(&mdata,0x0,sizeof(GK_MOUSE_DATA));

    fd_set              readfds;
    struct  timeval     tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0 ;
    int                 retval = 0;
    while(1)
    {

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
            retval = select(sdk_handle->mouse_fd+1,&readfds,NULL,NULL,&tv);
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

    return NULL;
}

static void* _image_sdk_handle_data_process_thread(void *data)
{
    
    
    int wait_times = 1000*1000 / sdk_handle->disp_fps;

    GK_MOUSE_DATA _ldata;


    
    while(1)
    {
       
        if(sdk_handle->mouse_data_updated){
            
            _ldata = sdk_handle->mouse_new_data;
            
            _image_analysis_mdata();
            _image_obj_func_run();
            _image_freshen_image();


            if(_ldata.x == sdk_handle->mouse_new_data.x &&
                    _ldata.y == sdk_handle->mouse_new_data.y &&
                    _ldata.event == sdk_handle->mouse_new_data.event)
                sdk_handle->mouse_data_updated = 0;

        }else{
            
            usleep(wait_times);
        } 
    
    }
    
    return NULL;

}



static  pthread_t   mouse_thread_id, process_thread_id;
void    Image_SDK_Run(void)
{
    
    pthread_create(&mouse_thread_id,NULL,
            _image_mouse_event_read_thread,NULL);

    pthread_create(&process_thread_id,NULL,
            _image_sdk_handle_data_process_thread,NULL);
  
#if 1  //block use test
    
    pthread_join(mouse_thread_id,NULL);
    pthread_join(process_thread_id,NULL);

#endif

    return ;

}




static void*  _image_fb_init(char *dev_path,int *fd)
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

static void*  _image_fb_deinit(void *mmap_p,int fd)

{
    if(mmap_p == NULL || fd < 0)
        return NULL;


    munmap(mmap_p, VO_SCREE_H * VO_SCREE_W *(VO_COLOR_FMT/8));
    close(fd);
    return NULL;
}
static void _image_fb_push(int xoffset,int yoffset){

    
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





