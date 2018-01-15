#ifndef __XW_CONFIG_PARAM_PROCESS_H__
#define __XW_CONFIG_PARAM_PROCESS_H__

#include <stdint.h>

typedef enum{
    
    SRCEE_MODE_1080,
    SRCEE_MODE_600,
    SRCEE_MODE_NUMS

}srcee_mode_type;

int xw_config_def_params_init(srcee_mode_type mode);
int xw_config_def_params_deinit(void);
int xw_get_node_param(char * window_id,uint16_t *x,uint16_t *y,uint16_t *w,uint16_t *h);


#endif
