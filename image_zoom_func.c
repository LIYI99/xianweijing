
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "image_zoom_func.h"
#include "xw_logsrv.h"

int     iamge_zoom_func(image_zoom_t *data)
{
    if(data == NULL)
        return -1;
    if(data->inbuf == NULL || data->outbuf == NULL)
        return -1;

    uint16_t sx,sy,dx,dy,*inrow,*outrow;
    int     xz,yz;

    xz = (data->inwidth <<  16) / data->outwidth;
    yz = (data->inheight << 16) / data->outheight;
    
//    xw_logsrv_debug("x_zoon:%d,y_zoon:%d,outwidth:%d outheight:%d inwidth:%d inheight:%d\n",xz,yz,data->outwidth,data->outheight,data->inwidth,data->inheight);
    //outrow = data->outbuf;

    for(dy = 0 ; dy < data->outheight ;dy++)
    {
        sy = (dy *  yz) >> 16;
        //xw_logsrv_debug("srcy:%d,desty:%d\n",sy,dy);
        outrow = data->outbuf + dy * data->outwidth;
        
        //outrow += data->outwidth;
        inrow  = data->inbuf  + sy * data->inwidth;

        for(dx = 0 ;dx < data->outwidth ;dx++)
        {
            sx = (dx *  xz) >> 16;
            outrow[dx] = inrow[sx];
            //*(data->outbuf+ dy*data->inwidth + dx) = *(data->inbuf + sy *data->inwidth + sx);
        }
    }

    return 0;

}

