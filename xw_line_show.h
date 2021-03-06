#ifndef __XW_LINE_SHOW_H__
#define __XW_LINE_SHOW_H__

#include <stdint.h>


void    xw_line_show_all(void *data);
void    xw_line_quit_all(void *data);

int     xw_lines_arry_set_order(uint16_t set_order);
int     xw_lines_line_set_param(uint16_t set_order,uint16_t color,uint16_t size);
int     xw_lines_cl_op_all(void *data); //close and open
int     xw_lines_cl_op_line(void *data); //close and open

int     xw_lines_set_select(uint16_t line_order);
int     xw_lines_set_lock(uint8_t lock);
int     xw_save_line_data(char *path);

int     xw_lines_close_all_root(void *data);
int     xw_lines_unlock(void);



#endif
