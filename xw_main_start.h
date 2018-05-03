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

typedef enum{
    HDMI_DEV_ONLY,
    HDMI_KEY_DEV,	
}IDS_DEV_MODE;

/*
 * time < 0 :run go on 
 * */

int     xw_main_ui_start(int times,srcee_vout_mode mode,IDS_DEV_MODE devmode);
int     xw_main_ui_quit(void);

/*
 *  update
 * */
int     xw_main_ui_start_update(void);
int     xw_main_ui_quit_update(void);
int     xw_main_ui_push_update_sclce(int vuale);


//push message to window  s len <= 32
int     xw_main_ui_push_message(char *s,int msec);

#ifdef  __cplusplus
}
#endif



#endif
