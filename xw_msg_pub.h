#ifndef __XW_MSG_PUB_H__
#define __XW_MSG_PUB_H__
#include <stdint.h>


/*
 *  init func:
 *  return      0: sesscure
 *          other: fail
 * */
#ifdef  __cplusplus
    extern  "C"{
#endif

int socket_msg_init(void);



/*
 *  all func:
 *  return  0 or > 0: seussscur
 *               < 0: fail
 *
 * */
int  socket_send_snap(void );
int  socket_send_recod_start(void );
int  socket_send_recod_stop(void );
int  socket_send_ae_auto(void  );
int  socket_send_ae_manule(  int value);
int  socket_send_awb_auto(void   );
int  socket_send_awb_manul(void   );
int  socket_send_flicker(  int vaule);
int  socket_send_contrast(  int value);
int  socket_send_saturation(  int vaule);
int  socket_send_brightness(  int vaule);
int  socket_send_sharpness(  int vaule);
int  socket_send_denoise(  int vaule);
int  socket_send_hue(  int vaule);
int  socket_send_awb_red(  int vaule);
int  socket_send_awb_green(  int vaule);
int  socket_send_awb_blue(  int vaule);
int  socket_send_awb_color_temp(  int vaule);
int  socket_send_hdr(  int vaule);
int  socket_send_filp(void  );
int  socket_send_mirror(void );
int  socket_send_night(  int vaule);
int  socket_send_freeze(int vaule);
int  socket_send_exit(void  );
int  socket_send_set_time(int data); //    

int  socket_get_ae_auto(void  );
int  socket_get_ae_manule(  int *vaule , int len);
int  socket_get_awb_auto(void   );
int  socket_get_awb_manul(void  );
int  socket_get_flicker(  int *vaule , int len);
int  socket_get_contrast(  int *value,int len);
int  socket_get_saturation(  int *vaule , int len);
int  socket_get_brightness(  int *vaule , int len);
int  socket_get_sharpness(  int *vaule , int len);
int  socket_get_denoise(  int *vaule , int len);
int  socket_get_hue(  int *vaule , int len);
int  socket_get_awb_red(  int *vaule , int len);
int  socket_get_awb_green(  int *vaule , int len);
int  socket_get_awb_blue(  int *vaule , int len);
int  socket_get_awb_color_temp(  int *vaule , int len);
int  socket_get_sdcard_state(  int *vaule , int len);
int  socket_get_get_carture_filename(char *s,int len);
int  socket_get_get_recoder_filename(char *s ,int len );

//use get snap point
struct image_get{
   uint8_t     mode;          // 0:firtst get,1:next 2:prve  3: only singbit saml      
   uint8_t     order;          // sigl : 0 - 3 //,4:next 5:prve
   void*       magep_p[4];
   int         nums;
};

int  socket_get_get_snap_point(void* data ,int len);
int  socket_send_reset_isp(void);

//update
int  socket_send_update_en(int en,int len);



#ifdef  __cplusplus
}
#endif





/////////device to app////////////
//void device_send_info_text_box(char *s);








#endif
