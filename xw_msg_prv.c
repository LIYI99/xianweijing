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


static int  clt_fd = -1;
pthread_t   socket_id;

static int    connect_fd_process(char *msg,int len);
static void*  socket_client_run(void *data);


int Image_Msg_Start(char *srv)
{
    pthread_create(&socket_id,NULL,socket_client_run,(void *)srv);
    return 0;

}




static int connect_fd_process(char *msg,int len)
{
    
    xw_logsrv_debug("mgs_cmd:%d\n",*(int *)msg);
    
    img_msg_cmd_t   cmd ;
    cmd  = *(img_msg_cmd_t  *)msg;

    Image_Msg_Proess(cmd,(void *)(msg + sizeof(img_msg_cmd_t)),(len  -sizeof(img_msg_cmd_t)));
    
    return 0;

}



static void*  socket_client_run(void *data)
{
    
    if(!data)
        return NULL;
    
    char *sun_path  = (char *)data;


    int connect_fd, ret;
    struct sockaddr_un  idscam_srv_addr;
    connect_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if (connect_fd < 0) {
        xw_logsrv_err("comm socket uninit");
        return NULL;
    }
    
    idscam_srv_addr.sun_family = AF_UNIX;
    strcpy((char *) &idscam_srv_addr.sun_path, sun_path);
    ret = connect(connect_fd, (struct sockaddr *) &idscam_srv_addr, sizeof(idscam_srv_addr));
    if (ret == -1) {
        xw_logsrv_err("cannot connect to the server");
        return NULL;
    }
    
    clt_fd = connect_fd;
    
    struct timeval tv;
    fd_set      readfds;

    char msg[128]; 
    

    for(;;)
    {
        FD_ZERO(&readfds);
        FD_SET(connect_fd,&readfds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        ret = select(connect_fd+1,&readfds,NULL,NULL,&tv);
        if(FD_ISSET(connect_fd,&readfds))
        {
            ret = read(connect_fd,(void *)msg,128);
            if(ret < 4)
            continue;
        }
        if(ret <= 4){
            connect_fd_process(msg,ret);
        }
    }
    
    return NULL;

}



int     Image_Msg_Send(img_msg_cmd_t cmd,void *data,int len)
{
    
    xw_logsrv_debug("SEND MSG CMD:%d\n",cmd);

    switch(cmd)
    {
        case  	IDSCAM_IMG_MSG_CAPTURE :
            socket_send_snap(clt_fd);
            break;
        case    IDSCAM_IMG_MSG_RECORED_START :
            socket_send_recod_start(clt_fd  );
            break;
        case    IDSCAM_IMG_MSG_RECORED_STOP :
            socket_send_recod_stop(clt_fd  );

            break;
        case   	IDSCAM_IMG_MSG_GET_SCAN_PIC_NUM :
            break;
        case    	IDSCAM_IMG_MSG_SCAN_PIC:
            break;
        case     IDSCAM_IMG_MSG_ENABLE_AE:
            socket_send_ae_auto(clt_fd  );

            break;
        case    IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO:
            socket_send_ae_manule(clt_fd, *(int *)data);

        case IDSCAM_IMG_MSG_ENABLE_AWB:
            socket_send_awb_auto(clt_fd  );

            break;
        case IDSCAM_IMG_MSG_AWB_SET_ONE_PUSH:
            socket_send_awb_manul(clt_fd );
            break;
        case IDSCAM_IMG_MSG_ANTIFLICKER:
            socket_send_flicker(clt_fd ,*(int *)data);
            break;
        case IDSCAM_IMG_MSG_CONTRAST:
            socket_send_contrast(clt_fd,*(int *)data);

            break;
        case IDSCAM_IMG_MSG_SATURATION:
            socket_send_saturation(clt_fd,*(int *)data);

            break;
        case IDSCAM_IMG_MSG_BRIGHTNESS:
            socket_send_brightness(clt_fd,*(int *)data );

            break;
        case IDSCAM_IMG_MSG_HUE:
    //            socket_send_hue(clt_fd,* (int *)data);

            break;
        case IDSCAM_IMG_MSG_SHARPNESS:
            socket_send_sharpness(clt_fd,  *(int *)data);

            break;
        case IDSCAM_IMG_MSG_DENOISE:
            socket_send_denoise( clt_fd, *(int *)data);

            break;
        case IDSCAM_IMG_MSG_AWB_SET_RGB_GAIN:
            socket_send_awb_red( clt_fd, *(int *)data);
            break;
        case IDSCAM_IMG_MSG_AWB_SET_COLORTEMP:
            socket_send_awb_color_temp(clt_fd,*(int *)data);

            break;
        case IDSCAM_IMG_MSG_MIRROR:
            socket_send_mirror(clt_fd  );

            break;
        case IDSCAM_IMG_MSG_FLIP:
            socket_send_filp(clt_fd  );

            break;
        case IDSCAM_IMG_MSG_SET_BW_MODE:
            socket_send_night(clt_fd,*(int *)data);

            break;
        case IDSCAM_IMG_MSG_SENSOR_HDR_OP_MODE:
            socket_send_hdr(clt_fd,  *(int *)data);

            break;
        case IDSCAM_IMG_MSG_EXIT:
            //socket_send_exit(clt_fd );

            break;
        default:
            break;



    }
    return 0;
}

int     Image_Msg_Proess(img_msg_cmd_t cmd,void *dat,int len){

    return 0;

}

