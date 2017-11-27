#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>

#include "image_text_put.h"



#define  FONT_SIZE_16       16
#define  FONT_SIZE_20       20
#define  FONT_SIZE_24       24
#define  FONT_SIZE_32       32




struct  text_lib_handle{
    int     text_size;
    size_t  asc_width;
    size_t  gb2312_width;
    char*   lib_ptr;
};


struct text_lib_handle*  _text_handle = NULL;


int     image_text_lib_init(int text_size,char* text_path)
{
    if(_text_handle != NULL)
        return -2;
    if(text_path == NULL || text_size < 8)
        return -3;

    _text_handle = (struct text_lib_handle *)malloc(sizeof(struct text_lib_handle));
    if(!_text_handle)
        return -1;
    memset(_text_handle ,0x0,sizeof(struct text_lib_handle));
    
    //asc width set 
    
    _text_handle->text_size =  text_size;

    switch(_text_handle->text_size){

        case FONT_SIZE_16:
            _text_handle->asc_width = 8;
            _text_handle->gb2312_width = 16;
            break;
        case FONT_SIZE_20:
            
            _text_handle->asc_width = 10;
            _text_handle->gb2312_width = 20;

            break;

        case FONT_SIZE_24:
            _text_handle->asc_width = 12;
            _text_handle->gb2312_width = 24;



            break;

        case FONT_SIZE_32:
             _text_handle->asc_width = 16;
            _text_handle->gb2312_width = 32;
   
            break;
        default:
            _text_handle->asc_width = -1;
            break;
    }

    if(_text_handle->asc_width < 0)
        goto ERR1;

    //load lib 
    struct stat text_stat = {0};
    FILE *fp = NULL;
    if( 0 != stat(text_path,&text_stat))
        goto ERR1;

    fp = fopen(text_path,"rb");
    if(fp  == NULL)
        goto ERR1;
   
    _text_handle->lib_ptr = (char *)malloc(text_stat.st_size);
    if(_text_handle->lib_ptr == NULL)
        goto ERR2;
    
    memset(_text_handle->lib_ptr,0x0,text_stat.st_size);

    //load
    fread(_text_handle->lib_ptr,1,text_stat.st_size,fp);

    fclose(fp);
    _text_handle->text_size = text_size;

    return 0;

ERR2:
    
    fclose(fp);
    free(_text_handle->lib_ptr);
    
ERR1:
    
    free(_text_handle);
    return -5;
}

int     image_text_lib_deinit(void){

    
    free(_text_handle->lib_ptr);
    free(_text_handle);
    
    return 0;


}

//only suport  enlish
int     image_text_lib_put_pixl(char *text, uint8_t *buf){

    
    if(text == NULL)
        return -1;
    if(*text >= 0x7f)
        return -2;
    

    int const stride_bytes =  (_text_handle->asc_width +7)/8;
    int const asc_code = *text;
    
    int i,ii,iii;

    for(i = 0 ; i < _text_handle->text_size;i++)
    {
        int stride_pixel = 0;
        	
        for(ii = 0; ii < stride_bytes && stride_pixel < _text_handle->asc_width; ++ii)
        {
			
            off_t const offset_byte = asc_code * _text_handle->text_size * stride_bytes + i * stride_bytes + ii;
			
            for(iii = 0; iii < 8 && stride_pixel < _text_handle->asc_width; ++iii)
            {
						uint8_t const actived_px = 0x80>>iii;//(1<<(8-1-iii));
				        //1 is font,o is back	
                        if(_text_handle->lib_ptr[offset_byte] & actived_px){
                             *buf =  1;
                        }else{
                            
                             *buf = 2;
                        }
                        ++stride_pixel;
                        buf++;

			}
        }
    }
    
    return 0; 

}

