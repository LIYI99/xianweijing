#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include "xw_window_id_df.h"
#include "gk_image_sdk_new.h"
#include "xw_test_freshen.h"


static int test_state = 0;
static void  test_conorl_button_ldown(void *data)
{
    //window_node_button_t *bt  =  (window_node_button_t *)data;
    
    if(test_state == 0){
        Image_SDK_Set_Node_En("Af",0);
        Image_SDK_Set_Node_En_Freshen("Af",NEED_CLEAR);
        test_state++;
    }else if(test_state == 1){
        Image_SDK_Set_Node_En("Af",1);
        Image_SDK_Set_Node_En_Freshen("Af",NEED_FRESHEN);
        test_state++;
    }else if(test_state == 2){
        //printf("test_state 3 \n"); 
        Image_SDK_Set_Node_En("Ae",0);
        Image_SDK_Set_Node_En_Freshen("Ae",NEED_CLEAR);
        test_state++;

    }else if(test_state == 3){
        
        Image_SDK_Set_Node_En("Ae",1);
        Image_SDK_Set_Node_En_Freshen("Ae",NEED_FRESHEN);
        test_state++;
        //test_state = 0 ;
    }else if(test_state == 4){
        
        Image_SDK_Set_Node_En("Aj",0);
        Image_SDK_Set_Node_En_Freshen("Aj",NEED_CLEAR);
        test_state++;

 
    }else if(test_state == 5){
        Image_SDK_Set_Node_En("Aj",1);
        Image_SDK_Set_Node_En_Freshen("Aj",NEED_FRESHEN);
        test_state++;


    }else if(test_state == 6){
        Image_SDK_Set_Node_En("Ah",0);
        Image_SDK_Set_Node_En_Freshen("Ah",NEED_CLEAR);
        test_state++;


    }else if(test_state == 7){
        Image_SDK_Set_Node_En("Ah",1);
        Image_SDK_Set_Node_En_Freshen("Ah",NEED_FRESHEN);
        test_state = 0;
    }
    
    
}


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
    node_attr.en_node = 1;
    node_attr.en_freshen = 1;
    node_attr.move_arrt = 0;
    
    window_node_button_t  _button;
    
    memset(&_button,0x0,sizeof(window_node_button_t));
    _button.x = 300;
    _button.y = 400;
    _button.w = 80;
    _button.h = 50;
    _button.color = 0xf00f;
    _button.video_set.mouse_left_down =  test_conorl_button_ldown;
    memcpy(node_attr.node_id,"Ad",2);
    Image_SDK_Create_Button(  node_attr,_button);
    //
    _button.video_set.mouse_left_down = NULL;

    _button.x = 550;
    _button.y = 700;
    _button.w = 80;
    _button.h = 80;
    _button.color = 0xff00;
   
    //create 1
    memcpy(node_attr.node_id,"Af",2);
    Image_SDK_Create_Button(  node_attr,_button);


    _button.x = 650;
    _button.y = 700;
    _button.w = 80;
    _button.h = 80;
    _button.color = 0xf0f0;
   
    //create 1
    memcpy(node_attr.node_id,"Ae",2);
    Image_SDK_Create_Button(  node_attr,_button);

     _button.x = 750;
    _button.y = 700;
    _button.w = 80;
    _button.h = 80;
    _button.color = 0xf00f;
   
    //create 1
    memcpy(node_attr.node_id,"Aj",2);
    Image_SDK_Create_Button(  node_attr,_button);

     _button.x = 850;
    _button.y = 700;
    _button.w = 80;
    _button.h = 80;
    _button.color = 0xff00;
    _button.video_set.mouse_left_down = cnt_lines_button_ldow; 
    //create 1
    //node_attr. = 
    memcpy(node_attr.node_id,"Ah",2);
    Image_SDK_Create_Button(  node_attr,_button);
    Image_SDK_Set_Node_Order("Ah",FIXD_ORDER);

 



    return;


}
