#ifndef GBK_T_UTF8_H_
#define GBK_T_UTF8_H_
#ifdef __cplusplus
extern "C" {
#endif

extern int gbk_to_utf8(char *gbk, char *utf8, int len);
extern int utf_to_gbk(char *utf8, char *gbk, int len);

#ifdef __cplusplus
}
#endif
#endif 
