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
#include "xw_msg_prv.h"
#include "xw_msg_pub.h"
#include "xw_logsrv.h"



int Image_Msg_Start(void)
{
    
   
    int ret = 0;
    ret = socket_msg_init();
    return ret;

}







int     Image_Msg_Send(img_msg_cmd_t cmd,void *data,int len)
{
    
    int ret = 0  ;
    if(NULL != data){
        ret  = *(int *)data;
    }
    xw_logsrv_err("SEND MSG CMD:%d,SET VALUE:%d\n",cmd,ret);
    
    ret  = 0;

    switch(cmd)
    {
        case  	IDSCAM_IMG_MSG_CAPTURE :
            ret = socket_send_snap();
            break;
        case    IDSCAM_IMG_MSG_RECORED_START :
            ret = socket_send_recod_start( );
            break;
        case    IDSCAM_IMG_MSG_RECORED_STOP :
            ret = socket_send_recod_stop(  );

            break;
        case   	IDSCAM_IMG_MSG_GET_SCAN_PIC_NUM :
            break;
        case    	IDSCAM_IMG_MSG_SCAN_PIC:
            break;
        case     IDSCAM_IMG_MSG_ENABLE_AE:
            ret = socket_send_ae_auto(  );

            break;
        case    IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO:
            ret = socket_send_ae_manule( *(int *)data);
            break;

        case IDSCAM_IMG_MSG_ENABLE_AWB:
            ret = socket_send_awb_auto(  );

            break;
        case IDSCAM_IMG_MSG_AWB_SET_ONE_PUSH:
            ret = socket_send_awb_manul( );
            break;
        case IDSCAM_IMG_MSG_ANTIFLICKER:
            ret = socket_send_flicker( *(int *)data);
            break;
        case IDSCAM_IMG_MSG_CONTRAST:
            ret = socket_send_contrast(*(int *)data);

            break;
        case IDSCAM_IMG_MSG_SATURATION:
            ret = socket_send_saturation(*(int *)data);

            break;
        case IDSCAM_IMG_MSG_BRIGHTNESS:
            ret = socket_send_brightness(*(int *)data );

            break;
        case IDSCAM_IMG_MSG_HUE:
    //      ret = socket_send_hue(* (int *)data);

            break;
        case IDSCAM_IMG_MSG_SHARPNESS:
            ret = socket_send_sharpness(  *(int *)data);

            break;
        case IDSCAM_IMG_MSG_DENOISE:
            ret = socket_send_denoise(  *(int *)data);

            break;
        case IDSCAM_IMG_MSG_AWB_SET_RED_GAIN:
            ret = socket_send_awb_red(  *(int *)data);
            break;
        case IDSCAM_IMG_MSG_AWB_SET_GREEN_GAIN:
            ret = socket_send_awb_green(  *(int *)data);
            break;
        case IDSCAM_IMG_MSG_AWB_SET_BLUE_GAIN:
            ret = socket_send_awb_blue(  *(int *)data);
            break;


        case IDSCAM_IMG_MSG_AWB_SET_COLORTEMP:
            ret = socket_send_awb_color_temp(*(int *)data);

            break;
        case IDSCAM_IMG_MSG_MIRROR:
            ret = socket_send_mirror(  );

            break;
        case IDSCAM_IMG_MSG_FLIP:
            ret = socket_send_filp(  );

            break;
        case IDSCAM_IMG_MSG_SET_BW_MODE:
            ret = socket_send_night(*(int *)data);

            break;
        case IDSCAM_IMG_MSG_SENSOR_HDR_OP_MODE:
            ret = socket_send_hdr(  *(int *)data);

            break;
        case IDSCAM_IMG_MSG_EXIT:
            //ret = socket_send_exit(clt_fd );

            break;
        default:
            break;



    }
    return ret;
}

int     Image_Msg_Get(img_msg_cmd_t cmd,void *data,int len){
    
   // xw_logsrv_debug("XXXXXGET MSG CMD:%d\n",cmd);
    int ret = 0;
    switch(cmd)
    {
        case  IDSCAM_IMG_MSG_GET_ENABLE_AE:
            ret = socket_get_ae_auto();
            break;
        case  IDSCAM_IMG_MSG_GET_SCAN_PIC_NUM:
            ret  = socket_get_ae_manule( (int *)data , len/4);
            break;
        case IDSCAM_IMG_MSG_AE_GET_TARGET_RATIO:
            ret = socket_get_ae_manule(  (int *)data , len/4);

            break;

        case IDSCAM_IMG_MSG_GET_ENABLE_AWB: 
            ret = socket_get_awb_auto();

            break;

        case IDSCAM_IMG_MSG_AWB_GET_ONE_PUSH:
            ret = socket_get_awb_manul( );

            break;
        case IDSCAM_IMG_MSG_GET_ANTIFLICKER: 
            ret = socket_get_flicker(   (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_CONTRAST: 
            ret = socket_get_contrast( (int *)data,len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_SATURATION: 
            ret =  socket_get_saturation(   (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_BRIGHTNESS: 
            ret =  socket_get_brightness(   (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_HUE: 
            ret =  socket_get_hue(   (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_SHARPNESS: 
            ret =   socket_get_sharpness(  (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_GET_DENOISE: 
            ret =   socket_get_denoise(  (int *)data , len/sizeof(int));

            break;
        case IDSCAM_IMG_MSG_AWB_GET_RED_GAIN:
            ret =   socket_get_awb_red(   (int *)data , len/sizeof(int));
            break;
        case IDSCAM_IMG_MSG_AWB_GET_GREEN_GAIN:
            ret =   socket_get_awb_green(   (int *)data , len/sizeof(int));
            break;
        case IDSCAM_IMG_MSG_AWB_GET_BLUE_GAIN:
            ret =   socket_get_awb_blue(   (int *)data , len/sizeof(int));
            break;
        case IDSCAM_IMG_MSG_AWB_GET_COLORTEMP: 
            ret =   socket_get_awb_color_temp( (int *)data , len/sizeof(int));
            break;
        case IDSCAM_IMG_MSG_GET_SDCARD_STATE: 
            ret =   socket_get_sdcard_state( (int *)data , len/sizeof(int));
            break;

        case IDSCAM_EVENT_GET_CAPTURE_FILENAME:
            ret = socket_get_get_carture_filename((char *)data,len);
            break;
        case IDSCAM_EVENT_GET_RECORED_FILENAME: 
            ret =   socket_get_get_recoder_filename((char *)data ,len );
        
        case IDSCAM_IMG_MSG_GET_CAPTURE_POINT: 
            ret =   socket_get_get_snap_point(data ,len);


            break;
        default:
            break;


    }
    xw_logsrv_err("GET MSG CMD:%d get ret:%d\n",cmd,ret);
    
    return ret;
}

int     Image_Msg_Proess(img_msg_cmd_t cmd,void *data,int len){

    

    return 0;
}

