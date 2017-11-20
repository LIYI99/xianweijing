#ifndef __GK_IMAGE_SDK_H__
#define __GK_IMAGE_SDK_H__

#include <stdint.h>
#include "gk_mouse_event.h"
#include "object_pool.h"

#ifdef __cplusplus
    extern "C"{
#endif

typedef  enum{
    OBJECT_BUTTION,
    OBJECT_MENU,
    OBJECT_LINE,
    OBJECT_NUMS,
}OBJECT_S;


struct  lqueue{
    struct lqueue  *prev;
    struct lqueue  *next;
    uint16_t        mouse_en;
    uint16_t        need_push;
    OBJECT_S        object_s;
    void *          data;
};


typedef void  (*object_func)(void *data);

typedef struct image_sdk_button{
    uint16_t        order;
    uint8_t         en_mouse_check;
    uint8_t         en_disp;
    uint8_t         need_push;
    uint8_t         en_follow;

    uint32_t        color; 
    uint16_t        x;
    uint16_t        y;
    uint16_t        w;
    uint16_t        h;
    uint16_t        size;
    
    char            *pictuer_buf;
    
    object_func     mouse_offset;
    object_func     mouse_left_down;
    object_func     mouse_left_up;
    object_func     mouse_right_down;
    object_func     mouse_right_up;
    object_func     mouse_leave;


    void            *data;
}image_sdk_button_t;


typedef struct image_sdk_menu{
    
    uint16_t        order;
    uint8_t        en_mouse_check;
    uint8_t        en_disp;
    uint8_t        need_push;
    uint8_t        en_follow;

    uint32_t        color; 
    uint16_t        x;
    uint16_t        y;
    uint16_t        w;
    uint16_t        h;
    uint16_t        size;
    
    char            *pictuer_buf;
       object_func     mouse_offset;
    object_func     mouse_left_down;
    object_func     mouse_left_up;
    object_func     mouse_right_down;
    object_func     mouse_right_up;
    object_func     mouse_leave;
    
    void            *data;
}image_sdk_menu_t;

typedef struct image_sdk_line{
    uint8_t        order;
    uint8_t        en_mouse_check;
    uint8_t        en_disp;
    uint8_t        need_push;
    uint8_t        en_follow;
    uint32_t        color; 
    uint16_t        start_x;
    uint16_t        start_y;
    uint16_t        end_x;
    uint16_t        end_y;
    uint16_t        size;
    char            *pictuer_buf;
    
    object_func     mouse_offset;
    object_func     mouse_left_down;
    object_func     mouse_left_up;
    object_func     mouse_right_down;
    object_func     mouse_right_up;
    object_func     mouse_leave;

    void            *data;
}image_sdk_line_t;


typedef struct image_sdk_mouse{
    uint16_t        order;
    uint32_t        color; 
    uint16_t        x_o;        //old
    uint16_t        y_o;        //old
    uint16_t        x;
    uint16_t        y;
    
    uint16_t        w;
    uint16_t        h;
    uint16_t        size;
    char            *save_buf;
    
    object_func     mouse_offset;
    object_func     mouse_left_down;
    object_func     mouse_left_up;
    object_func     mouse_right_down;
    object_func     mouse_right_up;
    object_func     mouse_leave;

    void            *data;
}image_sdk_mouse_t;


struct  obj_func_set{
    
    object_func     offset;
    object_func     ldown;
    object_func     lup;
    object_func     rdown;
    object_func     rup;
    object_func     leave;
    void *          data;
};


typedef struct  image_sdk_s{
         
    uint16_t        en;
    uint16_t        scree_w;
    uint16_t        scree_h;
    uint16_t        color_fmt;
    uint16_t        disp_fps;
    
    int             video_fd;
    void            *mmap_p;

    int             mouse_fd;
    GK_MOUSE_DATA   mouse_new_data; 
    
    struct lqueue   *head;
    struct lqueue   *end;
    struct lqueue   *last_run_node;
    struct obj_func_set set;
    object_pool_t       *lqueue_pool;
    object_pool_t       *imageobj_pool;
    image_sdk_mouse_t   *mouse;
}image_sdk_t;




void    Image_SDK_Init(void);
void    Image_SDK_deInit(void);
void    Image_SDK_Run(void);

int    Image_SDK_Create_Button( image_sdk_button_t  _button);
int    Image_SDK_Create_Menu(image_sdk_menu_t _menu);
int    Image_SDK_Create_Line(image_sdk_line_t _line);


#ifdef __cplusplus
}
#endif



#endif
