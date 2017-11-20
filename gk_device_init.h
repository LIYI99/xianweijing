#ifndef __GK_DEVICE_INIT_H__
#define __GK_DEVICE_INIT_H__


int     gk_device_init(void* data);
int     start_read_venc_thread(void);
void    start_recod(void* filename);
void    stop_recod(void);
void    snphot_jepg(void* filename);


#endif
