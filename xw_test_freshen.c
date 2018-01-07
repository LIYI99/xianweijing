#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "xw_window_id_df.h"
#include "image_sdk_core.h"
#include "xw_test_freshen.h"



uint16_t    *lcd_mem = NULL;

static int test_state = 0;
static int lines_state = 0;
static void cnt_lines_button_ldow(void *data)
{
    window_node_button_t *bt = (window_node_button_t *)data;
    bt->color = 0xeeee;

    if(lines_state == 0){
        Image_SDK_Set_Node_En(XW_LINE_RARR_WINDOW_ID ,1);
        Image_SDK_Set_Node_Submenu( XW_LINE_RARR_WINDOW_ID,1);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_FRESHEN);

        lines_state = 1;
    }else{
        Image_SDK_Set_Node_En( XW_LINE_RARR_WINDOW_ID,0);
        Image_SDK_Set_Node_Submenu( XW_LINE_RARR_WINDOW_ID,0);
        Image_SDK_Set_Node_En_Freshen( XW_LINE_RARR_WINDOW_ID,NEED_CLEAR);
        lines_state = 0;
        bt->color = 0xff00;
    }

}

void  xw_test_freshen_show(void)

{
      
    struct user_set_node_atrr  node_attr;
    memset(&node_attr,0x0,sizeof(struct user_set_node_atrr));
    node_attr.en_node = 0;
    node_attr.en_freshen = 0;
    node_attr.move_arrt = 0;

    lcd_mem = (char *)malloc(1024*600*2);

    memset(lcd_mem,0x0,1024*600*2);


    window_node_menu_t  _button;
    memset(&_button,0x0,sizeof(window_node_menu_t));
    _button.x = 50;
    _button.y = 100;
    _button.w = 1024;
    _button.h = 600;
    _button.image_cache = (char *)lcd_mem;
    _button.color = 0xf00f;
    _button.video_set.mouse_left_down =  NULL; //test_conorl_button_ldown;
    memcpy(node_attr.node_id,"Ax",2);
    Image_SDK_Create_Menu(  node_attr,_button);
    Image_SDK_Set_Node_Order("Ax",FIXD_ORDER);


    return;


}
