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
#include "xw_logsrv.h"
#include "ui_setup.h"
#include "xw_msg_pub.h"

static int socket_send_info_to_svr(int fd , img_msg_t *msg)
{
    if(fd <= 0)
        return -1;
    
    int ret  = 0;
    ret  = write(fd,(void *)msg,sizeof(img_msg_t));
    return ret;
}

int  socket_send_snap(int fd ){

    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = IDSCAM_IMG_MSG_CAPTURE ;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;
}

int  socket_send_recod_start(int fd )
{
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = IDSCAM_IMG_MSG_RECORED_START;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}

int  socket_send_recod_stop(int fd){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = IDSCAM_IMG_MSG_RECORED_STOP;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}

int  socket_send_ae_auto(int fd)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = IDSCAM_IMG_MSG_ENABLE_AE;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;


}
int  socket_send_ae_manule(int fd , int value){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = 	IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO;
    *(int *)msg.param =  value;

    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;


}
int  socket_send_awb_auto(int fd)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id = IDSCAM_IMG_MSG_ENABLE_AWB;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}
int  socket_send_awb_manul(int fd)
{
    return 0;

}
int  socket_send_flicker(int fd , int vaule){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_ANTIFLICKER;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;
}

int  socket_send_contrast(int fd , int value){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_CONTRAST;
    *(int*)msg.param = value;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;


}

int  socket_send_saturation(int fd , int vaule)
{
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_SATURATION;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;


}

int  socket_send_brightness(int fd , int vaule)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_BRIGHTNESS;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}
int  socket_send_sharpness(int fd , int vaule){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_SHARPNESS;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}
int  socket_send_denoise(int fd , int vaule){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_DENOISE;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}

int  socket_send_awb_red(int fd , int vaule)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_AWB_SET_RGB_GAIN;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;
 

}
int  socket_send_awb_gree(int fd , int vaule){
    
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_AWB_SET_COLORTEMP;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;
 

}

int  socket_send_awb_blue(int fd , int vaule)
{
    return 0;   
}
int  socket_send_awb_color_temp(int fd , int vaule){
    return 0;
}
int  socket_send_hdr(int fd , int vaule){
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_SENSOR_HDR_OP_MODE;
    *(int*)msg.param = vaule;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}

int  socket_send_filp(int fd)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_FLIP;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;
   
}
int  socket_send_mirror(int fd)
{

     img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_MIRROR;
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}
int  socket_send_night(int fd,int value)
{
    img_msg_t msg;
    memset(&msg,0x0,sizeof(msg));
    msg.msg_id =  IDSCAM_IMG_MSG_SET_BW_MODE;
   *(int*) msg.param = value;
        
    int ret ;
    ret  = write(fd,(void *)&msg,sizeof(img_msg_t));
    return ret;

}


