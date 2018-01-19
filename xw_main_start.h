#ifndef __XW_MAIN_START_H__
#define __XW_MAIN_START_H__
    
#ifdef  __cplusplus
    extern  "C"{
#endif
typedef enum{
    
    VOUT_MODE_1080,
    VOUT_MODE_600,
    VOUT_MODE_NUMS

}srcee_vout_mode;

/*
 * time < 0 :run go on 
 * */

int     xw_main_ui_start(int times,srcee_vout_mode mode);
int     xw_main_ui_quit(void);



#ifdef  __cplusplus
}
#endif



#endif
