
#ifndef __IMAGE_TEXT_PUT_H__
#define __IMAGE_TEXT_PUT_H__


int     image_text_lib_init(int text_size,char *text_path);
int     image_text_lib_deinit(void);
//int     image_text_lib_put_pixl(char *text, uint8_t *buf);

        
uint8_t *   image_text_lib_put_pixl(char *text);



int     image_text_get_font_size(void);
int     image_text_get_font_width(void);






#endif

