
#ifndef __IMAGE_HOST_EVENT_DECT_H__
#define __IMAGE_HOST_EVENT_DECT_H__



typedef enum{
    IMAGE_HOST_NOTHING,
    IMAGE_MOUSE_HOST_INSTER,
    IMAGE_MOUSE_HOST_MOVE,
}IMAGE_HOST_TYPES;


typedef     struct  image_host_handle{
        int                 host_fd;            //save host socket fd
        IMAGE_HOST_TYPES    retype;     
}image_host_handle_t;


/*
 *  reutrn > 0 init fail
 *  return > 0 init good
 *  return vaule is host int fd
 *
 * */

int  image_host_event_dect_init(void);
int  image_host_event_dect_deinit(int host_fd);

/*
 *  read data process and return host type
 *
 * */
void  image_host_event_dect_process(image_host_handle_t *hosts);







#endif
