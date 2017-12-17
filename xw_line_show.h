#ifndef __XW_LINE_SHOW_H__
#define __XW_LINE_SHOW_H__

#include <stdint.h>


void    xw_line_show_all(void *data);
int     xw_lines_arry_set_order(uint16_t set_order);
int     xw_lines_line_set_order(uint16_t set_order,uint16_t color,uint16_t size);

int     xw_lines_cl_op_all(void *data); //close and open
int     xw_lines_cl_op_line(void *data); //close and open






#endif
