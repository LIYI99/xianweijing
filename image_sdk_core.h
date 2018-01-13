#ifndef __GK_IMAGE_SDK_H__
#define __GK_IMAGE_SDK_H__

#include <stdint.h>
#include <pthread.h>
#include "image_mouse_event.h"
#include "image_object_pool.h"
#include "image_xbuddy_mempool.h"

#ifdef __cplusplus
    extern "C"{
#endif

typedef  enum{
    OBJECT_NOR,
    OBJECT_BUTTION,
    OBJECT_MENU,
    OBJECT_LINE,
    OBJECT_TEXT_WIN,
    OBJECT_BAR,
    OBJECT_NUMS,

}WIN_TYPE_S;



typedef struct window_node      window_node_t;

typedef enum {
    NOT_MOVE,
    MOUSE_OFFET_MOVE,
    MOUSE_LDOWN_MOVE,
    MOUSE_LUP_MOVE,
    MOUSE_RDOWN_MOVE,
    MOUSE_RUP_MOVE,
    MOVE_NUMS,
}NODE_MOVE_ARRT;

// V,freshen 
typedef enum{
    NORTHING = 0,
    NEED_FRESHEN,
    NEED_CLEAR,
    NEED_SYNC_FATHER, //not use
    FRESHEN_NUMS,
}NODE_FRESHEN_ARRT;

// V,node disp set
typedef enum{
    OPEN_DISP,
    CLOSE_DISP,
}NODE_VIDEO_ATTR;

//V,now video disp state
typedef enum{
    NOR_STATE,
    CLEAR_STATE,
    VIDEO_STATE,
}NODE_VIDEO_STATE;

typedef enum{
    FREE_ORDER,         //maybe free move order
    FIXD_ORDER,         //not move order
} NODE_ORDER_ATTR;

typedef struct limit_rect   limit_rect_t;

struct limit_rect{
    limit_rect_t    *next; 
    limit_rect_t    *prev;
    uint16_t        x;
    uint16_t        y;
    uint16_t        end_x;
    uint16_t        end_y;
    uint16_t        color;

};


struct window_node{

#define     MENU_LEVEL              5
    // window id save 
    char                node_id[MENU_LEVEL]; // ever level A -z sum 52 level 
    // father sub window list
    window_node_t       *prev;
    window_node_t       *next;
    window_node_t       *s_head;
    window_node_t       *s_end;
    window_node_t       *f_node;   
    limit_rect_t        *limit;
    //window func flags 
    uint8_t             en_node;            // the node is close or open
    uint8_t             check_node;         // the node 

#define         CLOSE_REFLECT       0
#define         OFFSET_REFLECT      1
#define         LDOWN_REFLECT       2
#define         LUP_REFLECT         4
#define         RDOWN_REFLECT       8
#define         RUP_REFLECT         16
    uint8_t             mouse_garp_reflect;
    uint8_t             en_intersection; 
    uint8_t             en_submenu;         // 0: 
    uint8_t             sub_intersenction;   // 0: sub menu not intersenction
    
    NODE_ORDER_ATTR     order_attr;
    NODE_FRESHEN_ARRT   freshen_arrt;
    NODE_VIDEO_ATTR     video_attr;
    NODE_VIDEO_STATE    video_state;
    NODE_MOVE_ARRT      move_arrt;
    
    //mouse check event data recod
    GK_MOUSE_DATA       mouse_data;
    GK_MOUSE_EVENT      last_event; 
    
    //window info and data 
    WIN_TYPE_S          win_type;
    void                *window;
};


typedef void  (*win_func)(void *data);

typedef void  (*win_func_usr)(void *data,uint16_t *fbbuf,
        int scree_w,int scree_h);


typedef struct window_func{
    
    win_func        mouse_offset;
    win_func        mouse_left_down;
    win_func        mouse_left_up;
    win_func        mouse_right_down;
    win_func        mouse_right_up;
    win_func        mouse_leave;
    void            *data;

}window_func_t;

typedef struct window_node_button{
   
    window_node_t   *this_node;
    uint16_t        last_x;
    uint16_t        last_y;
    uint16_t        x;
    uint16_t        y;
    uint16_t        w;
    uint16_t        h;
    uint16_t        size;
    uint32_t        color; 
    char            *image_cache;
    window_func_t   video_set;
    win_func_usr    user_video_freshen; 

}window_node_button_t;


typedef struct window_node_menu{
    window_node_t   *this_node;
    uint16_t        last_x;
    uint16_t        last_y;
    uint16_t        last_w;
    uint16_t        last_h;
    uint16_t        x;
    uint16_t        y;
    uint16_t        w;
    uint16_t        h;
    uint16_t        size;
    uint32_t        color; 
    char            *image_cache;
    window_func_t   video_set;
    win_func_usr    user_video_freshen; 
}window_node_menu_t;

typedef struct window_node_line{
       
    window_node_t   *this_node;
    uint16_t        last_x;
    uint16_t        last_y;
    uint16_t        last_size;

    uint16_t        start_x;
    uint16_t        start_y;
    uint16_t        end_x;
    uint16_t        end_y;
    uint16_t        size;
    uint32_t        color; 
    char            *image_cache;
    window_func_t   video_set;
    win_func_usr    user_video_freshen;
    char            text_id[MENU_LEVEL];   //be related text windows

}window_node_line_t;

typedef struct window_node_text{
       
    window_node_t   *this_node;
    uint16_t        last_x;
    uint16_t        last_y;
    uint16_t        x;
    uint16_t        y;
    uint8_t         font_size;
    uint8_t         asc_width;
    uint16_t        lens;
    uint32_t        win_color;
    uint32_t        text_color;
    char            *text_cache;
    window_func_t   video_set;
    win_func_usr    user_video_freshen;

}window_node_text_t;

typedef struct window_node_bar{
       
    window_node_t   *this_node;
    uint16_t        last_x;
    uint16_t        last_y;
    uint16_t        x;
    uint16_t        y;
    uint16_t        w;
    uint16_t        h;
    uint16_t        max_value;
    uint16_t        now_value;
    uint32_t        bar_color;
    window_func_t   video_set;
    win_func_usr    user_video_freshen;
    uint8_t         nohavetext;
    char            text_id[MENU_LEVEL];   //be related text windows
}window_node_bar_t;

typedef struct window_node_mouse{
    
    uint16_t        x_last;        //old
    uint16_t        y_last;        //old
    uint16_t        x;
    uint16_t        y;
    uint16_t        size;
    uint32_t        color;
    char            *image_p;
    char            *image_cache;
    char            *save_cache;

}window_node_mouse_t;


typedef struct  image_sdk_s{
 
#define     SDK_WORK_STATE_QUIT     0
#define     SDK_WORK_STATE_RUN      1
#define     SDK_WORK_STATE_STOP     2
    uint8_t         en;
    uint8_t         mouse_thread_state;
    uint8_t         event_thread_state;

    //video params
    uint16_t        scree_w;
    uint16_t        scree_h;
    uint16_t        color_fmt;
    uint16_t        disp_fps;
    int             video_fd;
    void            *mmap_p;
    uint8_t         mmap_updated; 
    //mouse
    int             mouse_fd;
    GK_MOUSE_DATA   mouse_new_data;
    uint8_t         need_restart_mouse;
    uint8_t         mouse_data_updated;
    
    //window root node
    window_node_t     *root;
    //pool
    object_pool_t   *window_node_pool;
    object_pool_t   *object_pool;
    object_pool_t   *limit_rect_pool;
    struct buddy_mem_s  *mems;
    
    //mouse check node record
    window_node_t   *last_check_node[MENU_LEVEL];
    window_node_t   *check_node[MENU_LEVEL];
    uint16_t        check_level_cnt;             
    //mouse image node
    window_node_mouse_t *mouse;
}image_sdk_t;


void    Image_SDK_Init(void);
void    Image_SDK_deInit(void);
void    Image_SDK_Run(void);
int     Image_SDK_Reset(void);

void    Image_Fb_Push(int xoffset,int yoffset);


struct user_set_node_atrr{
    char            node_id[MENU_LEVEL]; 
    uint8_t         en_node;
    uint8_t         mouse_garp_reflect;
    NODE_FRESHEN_ARRT   en_freshen;
    NODE_MOVE_ARRT  move_arrt;
};


int    Image_SDK_Create_Button( struct user_set_node_atrr attr,
        window_node_button_t _button);
int    Image_SDK_Create_Menu(struct user_set_node_atrr attr,
        window_node_menu_t _menu);
int    Image_SDK_Create_Line(struct user_set_node_atrr attr,
        window_node_line_t _line);
int    Image_SDK_Create_Text(struct user_set_node_atrr attr,
        window_node_text_t _text);
int    Image_SDK_Create_Bar(struct user_set_node_atrr attr,
        window_node_bar_t _bar);

int     Image_SDK_Destory_node(char *node_id);


int     Image_SDK_Set_Button_Color(char *node_id ,uint16_t color);
int     Image_SDK_Set_Line_Node_Param(char *node_id, window_node_line_t  *lt);
int     Image_SDK_Set_Node_Move_Atrr(char *node_id,NODE_MOVE_ARRT _arrt);
int     Image_SDK_Set_Node_En(char *node_id,uint8_t en);
int     Image_SDK_Set_Node_Order(char *node_id,NODE_ORDER_ATTR _attr);
int     Image_SDK_Set_Node_Submenu(char *node_id,uint8_t en);
int     Image_SDK_Set_Node_Disp(char *node_id,NODE_VIDEO_ATTR _attr);
int     Image_SDK_Set_Node_En_Freshen(char *node_id,NODE_FRESHEN_ARRT  en_freshen);
int     Image_SDK_Set_Text_Node_Text(char *node_id,char *text,int size);
int     Image_SDK_Set_Text_Node_Xy(char *node_id,uint16_t x,uint16_t y);
int     Image_SDK_Set_Bar_Vaule(char *node_id,uint16_t vaule);



//define debug 






#ifdef __cplusplus
}
#endif



#endif
