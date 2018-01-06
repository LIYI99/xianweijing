#ifndef __XW_MSG_PUB_H__
#define __XW_MSG_PUB_H__


int  socket_send_snap(int fd);
int  socket_send_recod_start(int fd);
int  socket_send_recod_stop(int fd);
int  socket_send_ae_auto(int fd );
int  socket_send_ae_manule(int fd , int value);
int  socket_send_awb_auto(int fd  );
int  socket_send_awb_manul(int fd  );
int  socket_send_flicker(int fd , int vaule);
int  socket_send_contrast(int fd , int value);
int  socket_send_saturation(int fd , int vaule);
int  socket_send_brightness(int fd , int vaule);
int  socket_send_sharpness(int fd , int vaule);
int  socket_send_denoise(int fd , int vaule);
int  socket_send_hue(int fd , int vaule);
int  socket_send_awb_red(int fd , int vaule);
int  socket_send_awb_gree(int fd , int vaule);
int  socket_send_awb_blue(int fd , int vaule);
int  socket_send_awb_color_temp(int fd , int vaule);
int  socket_send_hdr(int fd , int vaule);
int  socket_send_filp(int fd );
int  socket_send_mirror(int fd);
int  socket_send_night(int fd , int vaule);
int  socket_send_exit(int fd );
/////////device to app////////////
//void device_send_info_text_box(char *s);








#endif
