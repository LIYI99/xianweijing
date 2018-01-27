#ifndef __NETCAM_OVERLAY_CFG_H__
#define __NETCAM_OVERLAY_CFG_H__

#include<stdint.h>

#define     CONST_SIZE_H        16
#define     FONT_SIZE_16        16
#define     FONT_SIZE_32        32
#define     MAX_STREAM_NUMS     4    

#define kAPP_OVERLAY_CLOCK_NAME         "clock"
#define kAPP_OVERLAY_TITLE_NAME         "title"
#define kAPP_OVERLAY_ID_NAME            "id"



typedef union
{
    uint32_t  argb8888;
    struct {
        unsigned char   b;
        unsigned char   g;
        unsigned char   r;
        unsigned char   a;
    };
}gk_RGBColFormat; //3色定义

typedef struct  overlay_are_info{
    
    int     enable;                     //是否打开
    int     vin;                        //默认是0
    int     streamid;                   //属于那个流通道
    char    title[32];                  //块名称 
    int     font_h;                     //块长度 
    int8_t  font_size;                  //字体
    float   x;                          //x坐标百分比
    float   y;                          //y坐标百分比 
    gk_RGBColFormat color;              //字体定义
    gk_RGBColFormat bcolor;             //背景色定义
    char    buf[128];                   //块显示内容，如若为TEXT文本需填，其它可不填
    void*   canvas;                     //当前块向素缓存及操作方法类（句柄）

}OVERLAY_ARE_INFO;  //每个OSD块区域配置信息


typedef struct  overlay_plan_info{
    
    int                 enable;         //该层是否打开
    int                 streamid;       //该层属于那个流通道
    OVERLAY_ARE_INFO    _time;          //时间块
    OVERLAY_ARE_INFO    _ch;            //通道块
    OVERLAY_ARE_INFO    _text;          //地址块

}OVERLAY_PLAN_INFO;         //每个层的配置信息


typedef struct overlay_cfg_hanlde{
    
    int                 enable;
    OVERLAY_PLAN_INFO   _plan_info[ MAX_STREAM_NUMS ];  //最大四路

}OVERLAY_CFG_HANLDE;    //整个OSD句柄


int overlay_cfg_hanlde_init(void);   //初始化
int overlay_cfg_hanlde_free(void);   //释放
int overlay_cfg_hanlde_enable(int enable);  //打开huo关闭OSD
int overlay_cfg_plan_enable(int streamid,int enable);//打开或关闭某路流
int overlay_cfg_hanlde_set_time(OVERLAY_ARE_INFO *_info); //设置所有道通时间
int overlay_cfg_ch_set(OVERLAY_ARE_INFO  *_info); //设置怕有通道ID
int overlay_cfg_text_set(int plan,OVERLAY_ARE_INFO  *_info);//设置某个层的ＴＥＸＴ块

OVERLAY_CFG_HANLDE* overlay_cfg_get_hanlde(void); //获取ＯＳＤ句柄

void overlay_cfg_free_hanlde(void);//释放句柄,以上两个函数需要同时出现。





#endif
