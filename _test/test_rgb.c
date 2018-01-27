
#include <stdio.h>
#include <stdint.h>

struct argb_4444{
    uint16_t    r:4;
    uint16_t    b:4;
    uint16_t    g:4;
    uint16_t    a:4;
};
struct rgba_8888{
    

};


int main(void){



    uint16_t test = 0xf00f;
    struct argb_4444  *px = (struct argb_4444 *)&test;
    
    printf("a:%u r:%u g:%u b:%u red:%x\n",px->a,px->r,px->g,px->b,test);
    test = 0xff00;
    printf("a:%u r:%u g:%u b:%u gree:%x\n",px->a,px->r,px->g,px->b,test);
    test = 0xf0f0;
    printf("a:%u r:%u g:%u b:%u blue:%x\n",px->a,px->r,px->g,px->b,test);


    return 0;
}
