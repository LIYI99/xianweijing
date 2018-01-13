
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <fcntl.h>  
#include <unistd.h>
#include <sys/socket.h>  
#include <linux/netlink.h>  
#include "image_host_event_dect.h"
#include "xw_logsrv.h"

#define UEVENT_BUFFER_SIZE 2048  

int  image_host_event_dect_init(void)
{
    
    struct sockaddr_nl client;  
    
    int     host_fd     = -1;   
    int     buffersize  = 1024;
    
    //host_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_KOBJECT_UEVENT);  
    host_fd = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);  
   
    if(host_fd < 0)
        return -1;

    memset(&client, 0, sizeof(client));  
    client.nl_family    = AF_NETLINK;  
    client.nl_pid       = getpid();  
    client.nl_groups    = 1; //receive broadcast message

    setsockopt(host_fd, SOL_SOCKET, SO_RCVBUF, &buffersize, sizeof(buffersize));  
    //bind
    bind(host_fd, (struct sockaddr*)&client, sizeof(client));  

    return host_fd;
}
int  image_host_event_dect_deinit(int host_fd)
{

    if(host_fd >=0 )
        close(host_fd);
    return 0;
}

/*
 *  read data process and return host type
 *
 * */
void  image_host_event_dect_process(image_host_handle_t *hosts)
{
    if(!hosts)
        return ;
    if(hosts->host_fd < 0)
        return ;
    //get data
    int rcvlen = 0;
    char buf[UEVENT_BUFFER_SIZE];
    memset(buf,0x0,UEVENT_BUFFER_SIZE);
    rcvlen = recv(hosts->host_fd, buf, UEVENT_BUFFER_SIZE,0);
    //process data
    if(rcvlen <= 0){
        hosts->retype = IMAGE_HOST_NOTHING;
        return ;
    }
    buf[rcvlen] = '\0';
    //xw_logsrv_err("host get message:%s\n",buf);
    //add mouse0
    if(strstr(buf,"add") && strstr(buf,"mouse0")){ //inster
        hosts->retype = IMAGE_MOUSE_HOST_INSTER;
    }else if(strstr(buf,"remove") && strstr(buf,"mouse0")){ //move
        hosts->retype = IMAGE_MOUSE_HOST_MOVE;
    }else{
    
    }
    return ;
}





