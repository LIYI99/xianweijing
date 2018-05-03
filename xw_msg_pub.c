#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <unistd.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>

#include "ui_setup.h"
#include "xw_msg_pub.h"
//static  int     connet_fd = 0;

int socket_msg_init(void){
    
    return  -1;
}

int  socket_send_snap(void )
{ 
    return -1;
}
int  socket_send_recod_start(void )
{ 
    return -1;
}
int  socket_send_recod_stop(void ){ return -1;}
int  socket_send_ae_auto(void  ){ return -1;}
int  socket_send_ae_manule(  int value){ return -1;}
int  socket_send_awb_auto(void   ){ return -1;}
int  socket_send_awb_manul(void   ){ return -1;}
int  socket_send_flicker(  int vaule){ return -1;}
int  socket_send_contrast(  int value){ return -1;}
int  socket_send_saturation(  int vaule){ return -1;}
int  socket_send_brightness(  int vaule){ return -1;}
int  socket_send_sharpness(  int vaule){ return -1;}
int  socket_send_denoise(  int vaule){ return -1;}
int  socket_send_hue(  int vaule){ return -1;}
int  socket_send_awb_red(  int vaule){ return -1;}
int  socket_send_awb_green(  int vaule){ return -1;}
int  socket_send_awb_blue(  int vaule){ return -1;}
int  socket_send_awb_color_temp(  int vaule){ return -1;}
int  socket_send_hdr(  int vaule){ return -1;}
int  socket_send_filp(void  ){ return -1;}
int  socket_send_mirror(void ){ return -1;}
int  socket_send_night(  int vaule){ return -1;}
int  socket_send_exit(void  ){ return -1;}
int  socket_send_reset_isp(void){return -1;}
int  socket_send_freeze(int vaule){return -1;}

int  socket_send_set_time(int data){return -1;}
  


int  socket_get_ae_auto(void  ){ return -1;}
int  socket_get_ae_manule(  int *vaule , int len){ return -1;}
int  socket_get_awb_auto(void   ){ return -1;}
int  socket_get_awb_manul(void  ){ return -1;}
int  socket_get_flicker(  int *vaule , int len){ return -1;}
int  socket_get_contrast(  int *value,int len){ return -1;}
int  socket_get_saturation(  int *vaule , int len){ return -1;}
int  socket_get_brightness(  int *vaule , int len){ return -1;}
int  socket_get_sharpness(  int *vaule , int len){ return -1;}
int  socket_get_denoise(  int *vaule , int len){ return -1;}
int  socket_get_hue(  int *vaule , int len){ return -1;}
int  socket_get_awb_red(  int *vaule , int len){ return -1;}
int  socket_get_awb_green(  int *vaule , int len){ return -1;}
int  socket_get_awb_blue(  int *vaule , int len){ return -1;}
int  socket_get_awb_color_temp(  int *vaule , int len){ return -1;}
int  socket_get_sdcard_state(  int *vaule , int len){ return -1;}
int  socket_get_get_carture_filename(char *s,int len){ return -1;}
int  socket_get_get_recoder_filename(char *s ,int len ){ return -1;}

// < 0 fail
int  socket_get_get_snap_point(void* data ,int len){ return -1 ;}
//update  en: 0 not ,1: yes
int  socket_send_update_en(int en,int len){return -1;}







