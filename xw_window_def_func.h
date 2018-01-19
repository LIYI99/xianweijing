#ifndef __XW_WINDOW_DEF_FUNC_H__
#define __XW_WINDOW_DEF_FUNC_H__

#include <stdint.h>

typedef enum{
    
    SRCEE_MODE_1080,
    SRCEE_MODE_600,
    SRCEE_MODE_NUMS

}srcee_mode_type;

int xw_window_def_params_init(srcee_mode_type mode);
int xw_window_def_params_deinit(void);
int xw_get_node_window_param(char * window_id,uint16_t *x,uint16_t *y,uint16_t *w,uint16_t *h);


#endif
