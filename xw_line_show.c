#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xw_line_show.h"
#include "image_sdk_core.h"
#include "xw_window_id_df.h"
#include "xw_window_xy_df.h"
#include "xw_logsrv.h"
#include "xw_config.h"
#include "xw_window_def_func.h"

#define     XW_LINE_NUMS_MAX            16   //H:8 w:8    
#define     XW_LINE_CONF_NUMS           8

#define     XW_LINE_SIZE_MAX            3
#define     XW_LINE_SIZE_DEUFALT        1

#define     XW_LINE_RARR_WINDOW_H       1060
#define     XW_LINE_RARR_WINDOW_W       1800


#define     XW_LINE_TEXT_WIN_COLOR      0xfeee
#define     XW_LINE_TEXT_FONT_COLOR     0xf00f

#define     FONT_SIZE                   16


struct xw_line_s{
    struct user_set_node_atrr   _attr;
    window_node_line_t          line;
};

typedef struct xw_lines_s{
    uint8_t             line_arry_state;    // o close 1 open
    uint8_t             lock;               // 
    uint16_t            now_order;         //manger button check conf order
    uint16_t            line_order;         //manger button check line  order
    struct xw_line_s    lines[XW_LINE_CONF_NUMS][XW_LINE_NUMS_MAX];
}xw_lines_t;


static  xw_lines_t              *xw_lt = NULL;
static  FILE                    *xw_fp = NULL;


#define     XW_LINE_T_SAVEFILE_PATH     LINE_SET_PARAMS_FILE_PATH  
//file save and load
static  int  xw_load_line_data(char *path,xw_lines_t *lt);

//line  set add update



//mouse active 
static  void mouse_ldown_theline(void *data);
static  void mouse_offset_theline(void *data);


//init data
static  int  xw_line_t_init(void);

static window_node_line_t*  find_line_for_array(char *node_id)
{
    #define LINE_NODE_ID_LENS   3
    
    int i ;
    for(i = 0 ;i < XW_LINE_NUMS_MAX; i++){
         
        if(xw_lt->lines[xw_lt->now_order][i]._attr.node_id[LINE_NODE_ID_LENS - 1 ] == node_id[LINE_NODE_ID_LENS - 1])
            return &xw_lt->lines[xw_lt->now_order][i].line;
        }
    return NULL;
}



//mouse active 
static  void mouse_ldown_theline(void *data)
{
    
    window_node_line_t *lt  = (window_node_line_t *)data;
    window_node_line_t *sa = NULL;
  
    if(lt->this_node->move_arrt != 0 && xw_lt->lock  == 0)
    {   
                
            sa = find_line_for_array(lt->this_node->node_id);
            if(lt->start_x != lt->end_x){
                lt->start_y = lt->this_node->mouse_data.y;
                lt->end_y   = lt->start_y;
            }else{
                lt->start_x = lt->this_node->mouse_data.x;
                lt->end_x = lt->start_x;

            }
            *sa = *lt;

        Image_SDK_Set_Text_Node_Xy(lt->text_id,lt->start_x,lt->start_y);
        lt->this_node->f_node->freshen_arrt = NEED_FRESHEN;
        
        int i = 0;
        for(i = 0 ;i < XW_LINE_NUMS_MAX;i++ )
        {
            //there .....is not good better
            if(xw_lt->lines[xw_lt->now_order][i]._attr.node_id[2] == 
                    sa->this_node->node_id[2])
            {
                xw_lt->lines[xw_lt->now_order][i].line.start_x = sa->start_x;
                xw_lt->lines[xw_lt->now_order][i].line.start_y = sa->start_y;
                xw_lt->lines[xw_lt->now_order][i].line.end_x = sa->end_x;
                xw_lt->lines[xw_lt->now_order][i].line.end_y = sa->end_y;
                break;
            }
        
        }
        
    }
    
  



       return;


}
static  void mouse_ldown_line_menu(void *data)
{
    
    xw_logsrv_debug("test line man menu left down\n");
    
    Image_SDK_Set_Node_En(XW_MAIN_WINDOW_ID,0);
    Image_SDK_Set_Node_En_Freshen(XW_MAIN_WINDOW_ID,NEED_CLEAR);
    Image_SDK_Set_Node_Submenu(XW_MAIN_WINDOW_ID,0);
    return;
}


static int  xw_line_t_init(void){

    xw_lt = (xw_lines_t *)malloc(sizeof(xw_lines_t));
    if(xw_lt == NULL)
        return -1;
    memset(xw_lt,0x0,sizeof(xw_lines_t));


    int ret = -1;
    int i = 0,k ;


    if(XW_LINE_T_SAVEFILE_PATH != NULL)
    {
        ret = xw_load_line_data(XW_LINE_T_SAVEFILE_PATH,xw_lt);

    }
    if(ret == 0){
        xw_lt->line_arry_state = 0;
        return 0;
    }

    

    //if not load conf file else need init all config mem
    for(k = 0 ; k < XW_LINE_CONF_NUMS ;k++)
    {
        for(i = 0; i < XW_LINE_NUMS_MAX; i++)
        {
            xw_lt->lines[k][i]._attr.en_node              = 1;
            xw_lt->lines[k][i]._attr.en_freshen           = NEED_CLEAR;
            xw_lt->lines[k][i]._attr.mouse_garp_reflect   = OFFSET_REFLECT | LDOWN_REFLECT|LUP_REFLECT;
            xw_lt->lines[k][i]._attr.move_arrt            = MOUSE_LDOWN_MOVE;
        }
    }
    //init h line node id
    for(k = 0 ;k < XW_LINE_CONF_NUMS ; k++){
        memcpy(xw_lt->lines[k][0]._attr.node_id,XW_LINE_H1_WINDOW_ID,strlen(XW_LINE_H1_WINDOW_ID ));
        memcpy(xw_lt->lines[k][1]._attr.node_id,XW_LINE_H2_WINDOW_ID,strlen(XW_LINE_H2_WINDOW_ID ));
        memcpy(xw_lt->lines[k][2]._attr.node_id,XW_LINE_H3_WINDOW_ID,strlen(XW_LINE_H3_WINDOW_ID ));
        memcpy(xw_lt->lines[k][3]._attr.node_id,XW_LINE_H4_WINDOW_ID,strlen(XW_LINE_H4_WINDOW_ID ));
        memcpy(xw_lt->lines[k][4]._attr.node_id,XW_LINE_H5_WINDOW_ID,strlen(XW_LINE_H5_WINDOW_ID ));
        memcpy(xw_lt->lines[k][5]._attr.node_id,XW_LINE_H6_WINDOW_ID,strlen(XW_LINE_H6_WINDOW_ID ));
        memcpy(xw_lt->lines[k][6]._attr.node_id,XW_LINE_H7_WINDOW_ID,strlen(XW_LINE_H7_WINDOW_ID ));
        memcpy(xw_lt->lines[k][7]._attr.node_id,XW_LINE_H8_WINDOW_ID,strlen(XW_LINE_H8_WINDOW_ID ));
        //init w line node id
        memcpy(xw_lt->lines[k][8]._attr.node_id,XW_LINE_W1_WINDOW_ID,strlen(XW_LINE_W1_WINDOW_ID ));
        memcpy(xw_lt->lines[k][9]._attr.node_id,XW_LINE_W2_WINDOW_ID,strlen(XW_LINE_W2_WINDOW_ID ));
        memcpy(xw_lt->lines[k][10]._attr.node_id,XW_LINE_W3_WINDOW_ID,strlen(XW_LINE_W3_WINDOW_ID ));
        memcpy(xw_lt->lines[k][11]._attr.node_id,XW_LINE_W4_WINDOW_ID,strlen(XW_LINE_W4_WINDOW_ID ));
        memcpy(xw_lt->lines[k][12]._attr.node_id,XW_LINE_W5_WINDOW_ID,strlen(XW_LINE_W5_WINDOW_ID ));
        memcpy(xw_lt->lines[k][13]._attr.node_id,XW_LINE_W6_WINDOW_ID,strlen(XW_LINE_W6_WINDOW_ID ));
        memcpy(xw_lt->lines[k][14]._attr.node_id,XW_LINE_W7_WINDOW_ID,strlen(XW_LINE_W7_WINDOW_ID ));
        memcpy(xw_lt->lines[k][15]._attr.node_id,XW_LINE_W8_WINDOW_ID,strlen(XW_LINE_W8_WINDOW_ID ));
        //linit line text window
#if 1
        memcpy(xw_lt->lines[k][0].line.text_id,XW_LINE_H1_TEXT_WINDOW_ID,strlen(XW_LINE_H1_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][1].line.text_id,XW_LINE_H2_TEXT_WINDOW_ID,strlen(XW_LINE_H2_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][2].line.text_id,XW_LINE_H3_TEXT_WINDOW_ID,strlen(XW_LINE_H3_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][3].line.text_id,XW_LINE_H4_TEXT_WINDOW_ID,strlen(XW_LINE_H4_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][4].line.text_id,XW_LINE_H5_TEXT_WINDOW_ID,strlen(XW_LINE_H5_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][5].line.text_id,XW_LINE_H6_TEXT_WINDOW_ID,strlen(XW_LINE_H6_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][6].line.text_id,XW_LINE_H7_TEXT_WINDOW_ID,strlen(XW_LINE_H7_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][7].line.text_id,XW_LINE_H8_TEXT_WINDOW_ID,strlen(XW_LINE_H8_TEXT_WINDOW_ID ));
        //init w line node id
        memcpy(xw_lt->lines[k][8].line.text_id,XW_LINE_W1_TEXT_WINDOW_ID,strlen(XW_LINE_W1_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][9].line.text_id,XW_LINE_W2_TEXT_WINDOW_ID,strlen(XW_LINE_W2_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][10].line.text_id,XW_LINE_W3_TEXT_WINDOW_ID,strlen(XW_LINE_W3_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][11].line.text_id,XW_LINE_W4_TEXT_WINDOW_ID,strlen(XW_LINE_W4_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][12].line.text_id,XW_LINE_W5_TEXT_WINDOW_ID,strlen(XW_LINE_W5_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][13].line.text_id,XW_LINE_W6_TEXT_WINDOW_ID,strlen(XW_LINE_W6_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][14].line.text_id,XW_LINE_W7_TEXT_WINDOW_ID,strlen(XW_LINE_W7_TEXT_WINDOW_ID ));
        memcpy(xw_lt->lines[k][15].line.text_id,XW_LINE_W8_TEXT_WINDOW_ID,strlen(XW_LINE_W8_TEXT_WINDOW_ID ));
#endif

    }
    //init h line param

    int j,lh = 0 ,lw = 0;
    if(xw_get_node_window_mode() ==  SRCEE_MODE_1080){
        lh = 1070;
        lw = 1910;
    }else if(xw_get_node_window_mode() == SRCEE_MODE_600){
        lh = 590;
        lw = 1015;
    }else{
        xw_logsrv_err("not get srcee mode \n");
    }

    for(j = 0 ; j < XW_LINE_CONF_NUMS ; j++){ 
        for(i = 0 ,k = 100; i < XW_LINE_NUMS_MAX/2; i++,k += 200){

            xw_lt->lines[j][i].line.start_x   = k;
            xw_lt->lines[j][i].line.start_y   = 5;
            xw_lt->lines[j][i].line.end_x     = k;
            xw_lt->lines[j][i].line.end_y     = lh;
            xw_lt->lines[j][i].line.color     = 0xf00f;
            xw_lt->lines[j][i].line.size      =  XW_LINE_SIZE_DEUFALT ;
            xw_lt->lines[j][i].line.video_set.mouse_left_up = mouse_ldown_theline;
            xw_lt->lines[j][i].line.video_set.mouse_left_down = mouse_ldown_theline;
            
        }
    }

    //init w line param
    for(j = 0 ; j < XW_LINE_CONF_NUMS ; j++) {
        for(i = XW_LINE_NUMS_MAX/2 , k = 50  ; i < XW_LINE_NUMS_MAX;i++,k += 100){

            xw_lt->lines[j][i].line.start_x   = 5 ;
            xw_lt->lines[j][i].line.start_y   = k ;
            xw_lt->lines[j][i].line.end_x     = lw ;
            xw_lt->lines[j][i].line.end_y     = k;
            xw_lt->lines[j][i].line.color     = 0xf0f0;
            xw_lt->lines[j][i].line.size      =  XW_LINE_SIZE_DEUFALT;
            xw_lt->lines[j][i].line.video_set.mouse_left_up = mouse_ldown_theline;
            xw_lt->lines[j][i].line.video_set.mouse_left_down = mouse_ldown_theline;
            

        }
    }


    return 0;


}

void xw_line_show_all(void *data)
{

    //init xw line data

    int ret = 0;
    ret = xw_line_t_init();
    if(ret < 0)
        return;

   // return;

    struct user_set_node_atrr _attr;
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    memcpy(_attr.node_id,XW_LINE_RARR_WINDOW_ID,strlen(XW_LINE_RARR_WINDOW_ID ));
    _attr.en_node = 0; 
    //ceater line father menu
    window_node_menu_t mt;
    memset(&mt,0x0,sizeof(window_node_menu_t));
    ret = xw_get_node_window_param(XW_LINE_RARR_WINDOW_ID,&mt.x,&mt.y,&mt.w,&mt.h);
    if(ret < 0){
        
        xw_logsrv_err("windows:%s get x,y,w,h fail \n",XW_LINE_RARR_WINDOW_ID);

    }
    xw_logsrv_err("line arrr params x:%d y:%d w:%d h:%d\n",mt.x,mt.y,mt.w,mt.h);

    ret  = Image_SDK_Create_Menu(_attr,mt);
    
    //mt.video_set.mouse_right_down = mouse_ldown_line_menu;

    ret =  Image_SDK_Set_Node_Disp(XW_LINE_RARR_WINDOW_ID,CLOSE_DISP);
    ret =  Image_SDK_Set_Node_Order(XW_LINE_RARR_WINDOW_ID,FIXD_ORDER);


    
    window_node_text_t  _text;
    memset(&_text,0x0,sizeof(window_node_text_t));
    memset(&_attr,0x0,sizeof(struct user_set_node_atrr));
    _attr.en_node = 1;
    ret = xw_get_node_window_param(XW_ISP_SATURATION_TEXT_WINDOW_ID,NULL,NULL,&_text.asc_width,&_text.font_size);
    if(ret <  0){
    
        xw_logsrv_err("window:%s get x,y,w,h fail \n","LINE TEXT");
    }
    _text.win_color =  XW_LINE_TEXT_WIN_COLOR; 
    _text.text_color = XW_LINE_TEXT_FONT_COLOR;
    _text.lens = 2;
    //ceate all line  add text win
    int  i = 0;
    char text_buf[4];

    for(i = 0 ; i < XW_LINE_NUMS_MAX  ; i++ )
    {
        
        //create line window
        ret = Image_SDK_Create_Line(xw_lt->lines[xw_lt->now_order][i]._attr,xw_lt->lines[xw_lt->now_order][i].line);
        //create text window
    

        memcpy(_attr.node_id,xw_lt->lines[xw_lt->now_order][i].line.text_id,strlen(XW_LINE_H1_TEXT_WINDOW_ID));
        _attr.en_node = 1;
        _text.x = xw_lt->lines[xw_lt->now_order][i].line.start_x;
        if(xw_lt->lines[xw_lt->now_order][i].line.start_x != xw_lt->lines[xw_lt->now_order][i].line.end_x){
            _text.y = xw_lt->lines[xw_lt->now_order][i].line.start_y;
        }else{
            _text.y = xw_lt->lines[xw_lt->now_order][i].line.start_y; 
        }
        ret = Image_SDK_Create_Text(_attr,_text); 
        //set text window texts
        if(i < 8){
            
            sprintf(text_buf,"h%d",i+1);
        }else{
            sprintf(text_buf,"w%d",i - 7);
        }
        xw_logsrv_debug("text_buf:%s node_id:%c%c%c create node ret:%d, y:%d,x:%d\n",text_buf,_attr.node_id[0],_attr.node_id[1],_attr.node_id[2],ret,_text.y,_text.x);
        Image_SDK_Set_Text_Node_Text(_attr.node_id,text_buf,2);
        Image_SDK_Set_Node_Move_Atrr(_attr.node_id,MOUSE_LDOWN_MOVE);

    }
         


    return;

}


static  int  xw_load_line_data(char *path,xw_lines_t *lt)
{
    
    if(path == NULL || lt == NULL)
        return -1;
    if(xw_fp != NULL)
        return -2;
    int ret = 0;
    xw_fp = fopen(path,"rb+");
    if(!xw_fp)
        return -2;

    ret = fread((void*)lt,1,sizeof(xw_lines_t),xw_fp);
    if(ret < sizeof(xw_lines_t))
    {
        fclose(xw_fp);
        xw_fp = NULL;
        xw_logsrv_err("read config file error ret :%d sizeof(xw_lines_t):%d\n",ret,sizeof(xw_lines_t));
        return -3;

    }
    int k,i ;
       for(k = 0 ; k < XW_LINE_CONF_NUMS ;k++){
        for(i = 0 ;i < XW_LINE_NUMS_MAX ;i ++){
        
            xw_lt->lines[k][i].line.video_set.mouse_left_down = mouse_ldown_theline;
            xw_lt->lines[k][i].line.video_set.mouse_left_up   = mouse_ldown_theline;
        }
    }




    return 0;
    
}


int  xw_save_line_data(char *path)
{
    if(xw_lt == NULL)
        return -1;

    if(xw_fp == NULL)
        xw_fp = fopen(XW_LINE_T_SAVEFILE_PATH,"wb+");
    
    if(xw_fp == NULL)
        return -2;
    int ret ;

    fseek(xw_fp,0,SEEK_SET);
    ret = fwrite((void *)xw_lt,1,sizeof(xw_lines_t),xw_fp);
    fclose(xw_fp);
    xw_logsrv_err("write line data to file \n");
    xw_fp = NULL;
    return 0;
}  


int  xw_lines_arry_set_order(uint16_t set_order)
{
    
    if(set_order > XW_LINE_CONF_NUMS || xw_lt == NULL)
        return -1;
    if(xw_lt->line_arry_state == 0 || xw_lt->lock != 0 )
        return -2;
    int k = 0,ret = 0;
    
    xw_lt->now_order = set_order;

    for(k = 0 ;k < XW_LINE_NUMS_MAX ; k ++){
        ret = Image_SDK_Set_Line_Node_Param(xw_lt->lines[xw_lt->now_order][k]._attr.node_id,&(xw_lt->lines[xw_lt->now_order][k].line));
        
        ret = Image_SDK_Set_Text_Node_Xy(xw_lt->lines[xw_lt->now_order][k].line.text_id,
                xw_lt->lines[xw_lt->now_order][k].line.start_x,  xw_lt->lines[xw_lt->now_order][k].line.start_y );
        if(xw_lt->lines[xw_lt->now_order][k]._attr.en_node != 0){
            
            Image_SDK_Set_Node_En(xw_lt->lines[xw_lt->now_order][k]._attr.node_id,1);
            Image_SDK_Set_Node_En_Freshen(xw_lt->lines[xw_lt->now_order][k]._attr.node_id,NEED_FRESHEN);

        }else{
            Image_SDK_Set_Node_En(xw_lt->lines[xw_lt->now_order][k]._attr.node_id,0);
            Image_SDK_Set_Node_En_Freshen(xw_lt->lines[xw_lt->now_order][k]._attr.node_id,NEED_CLEAR);
        }

    }
    Image_SDK_Set_Node_En_Freshen(XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);
    return 0;
}
  int  xw_lines_line_set_param(uint16_t set_order,uint16_t color,uint16_t size)
{
     
    if(set_order > XW_LINE_CONF_NUMS || xw_lt == NULL)
        return -1;
    
    if(xw_lt->line_arry_state == 0 || xw_lt->lock != 0)
        
        return -2;

    xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line.color    = color;
    xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line.size     = size;
    int ret = 0;
    ret = Image_SDK_Set_Line_Node_Param(xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.node_id,&(xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line));
    Image_SDK_Set_Text_Node_Xy(xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line.text_id,
                xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line.start_x,  xw_lt->lines[xw_lt->now_order][xw_lt->line_order].line.start_y );

    
    Image_SDK_Set_Node_En_Freshen(XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);

    return ret;
}

int     xw_lines_cl_op_all(void *data)
{
 
    if(xw_lt == NULL)
        return -1;
    if(xw_lt->lock != 0)
        return -2;

     if(xw_lt->line_arry_state == 0)
     {
        xw_logsrv_debug("xw_lines_cl_op_all line:%d:  state:%denbale liens \n",__LINE__,xw_lt->line_arry_state);


        Image_SDK_Set_Node_En(XW_LINE_RARR_WINDOW_ID ,1);
        Image_SDK_Set_Node_Submenu( XW_LINE_RARR_WINDOW_ID,1);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);

        xw_lt->line_arry_state = 1;
        xw_logsrv_debug("line:%d\n",__LINE__);
    }else{

        Image_SDK_Set_Node_En( XW_LINE_RARR_WINDOW_ID,0);
        Image_SDK_Set_Node_Submenu( XW_LINE_RARR_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_CLEAR);
        xw_lt->line_arry_state= 0;
        xw_logsrv_debug("xw_lines_cl_op_all:disebe lines\n");
    }
    
    return 0;

}

int     xw_lines_close_all_root(void *data)
{

    if(xw_lt == NULL)
        return -1;

    if(xw_lt->lock != 0)
        xw_lt->lock = 0;

    Image_SDK_Set_Node_En( XW_LINE_RARR_WINDOW_ID,0);
    Image_SDK_Set_Node_Submenu( XW_LINE_RARR_WINDOW_ID,0);
    Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_CLEAR);
    xw_lt->line_arry_state= 0;
    
    xw_lt->lock = 1;


    return 0;

}


int     xw_lines_cl_op_line(void *data)
{
     
    if(xw_lt == NULL)
        return -1;
    
    if(xw_lt->line_arry_state == 0 || xw_lt->lock != 0)
        return -2;

    if(xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.en_node == 0){
        
        Image_SDK_Set_Node_En(xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.node_id ,1);
        Image_SDK_Set_Node_En_Freshen( xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.node_id  ,NEED_FRESHEN);
        //usleep(40000);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);
        xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.en_node = 1;
    }else{
        
        Image_SDK_Set_Node_En(xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.node_id ,0);
        Image_SDK_Set_Node_En_Freshen( xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.node_id ,NEED_CLEAR);
        //usleep(40000);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);
              
        xw_lt->lines[xw_lt->now_order][xw_lt->line_order]._attr.en_node = 0;
    }

    return 0;
}

int     xw_lines_set_select(uint16_t line_order)
{
    if(xw_lt == NULL)
        return -1;
    
    if(xw_lt->line_arry_state == 0 || xw_lt->lock != 0)
        return -2;
    
    xw_lt->line_order = line_order;
    return 0;
}

int     xw_lines_set_lock(uint8_t  line_order)
{
    
    if(xw_lt == NULL)
        return -1;
    //lock   
    if(xw_lt->lock == 0){
        xw_lt->lock = 1;

    }else  //ulock
    {
        xw_lt->lock = 0;
    }
    return 0;
}

void    xw_line_quit_all(void *data){
    
    if(xw_fp != NULL)
        fclose(xw_fp);

    
    if(xw_lt != NULL)
        free(xw_lt);
    
    return ;


}


