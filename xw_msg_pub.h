#ifndef __XW_MSG_PUB_H__
#define __XW_MSG_PUB_H__


/*
 *  init func:
 *  return      0: sesscure
 *          other: fail
 * */

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
int  socket_send_awb_gree(  int vaule);
int  socket_send_awb_blue(  int vaule);
int  socket_send_awb_color_temp(  int vaule);
int  socket_send_hdr(  int vaule);
int  socket_send_filp(void  );
int  socket_send_mirror(void );
int  socket_send_night(  int vaule);
int  socket_send_exit(void  );


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
int  socket_get_awb_gree(  int *vaule , int len);
int  socket_get_awb_blue(  int *vaule , int len);
int  socket_get_awb_color_temp(  int *vaule , int len);
int  socket_get_sdcard_state(  int *vaule , int len);
int  socket_get_get_carture_filename(char *s,int len);
int  socket_get_get_recoder_filename(char *s ,int len );






/////////device to app////////////
//void device_send_info_text_box(char *s);








#endif
