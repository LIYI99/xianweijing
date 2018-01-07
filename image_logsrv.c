

#include"image_logsrv.h"
#include<string.h>
#include<stdarg.h>
#include<unistd.h>

#define     DEFAULT_PATH    "./logsrv.txt"

struct basic_log_s{
        char*   logs_name;
        unsigned int    level;
        void*   p;
        pthread_mutex_t cache_lock;
        unsigned int    size;
        void*   pos;
        char*   path;
        FILE*   fp;
        unsigned int    logcnt;
        
        pthread_t       write_t;
        pthread_mutex_t cond_lock;
        pthread_cond_t  wait_d;
       
};

static void*    basic_log_write_file(void*data)
{
    if (!data)
        return NULL;
    basic_log_t *s =  (basic_log_t *)data;
    while(1){
        pthread_mutex_lock(&s->cache_lock);
        pthread_cond_wait(&s->wait_d, &s->cache_lock);
        if(s->fp == NULL){
            pthread_mutex_unlock(&s->cache_lock);
            break;
        }    
        
        int n =  s->pos -s->p;
        fwrite(s->p,n,1,s->fp);
        s->pos = s->p;
        pthread_mutex_unlock(&s->cache_lock);
    }
    return NULL;
}

  
static unsigned int     default_path_cnt = 0;
basic_log_t*    basic_log_init(char* logs_name,unsigned int level, unsigned int cache_size,
            char* path)
{
        basic_log_t* s = NULL;
        s = (basic_log_t*)malloc(sizeof(basic_log_t));
        if(s == NULL){
            printf("malloc NULL\n");
            return NULL;

        }
        memset(s,0x0,sizeof(basic_log_t));
        unsigned int size_n = 0;
        if(logs_name)
            size_n = strlen(logs_name);
        else
            size_n = 0;
        if(size_n)
            s->logs_name = (char *)malloc(size_n+1);
        else
            s->logs_name = NULL;
        if(s->logs_name)
            strcpy(s->logs_name,logs_name);
        else
            fprintf(stderr,"logs_name is NULL\n");

        s->size = cache_size;
        s->p =  malloc(s->size);
        if(s->p == NULL)
            goto ERR1;
        s->pos = s->p;
        
        if(path)
            size_n = strlen(path);
        else
        {
            size_n = strlen(DEFAULT_PATH);
            size_n  += 4;
        }
        
        size_n +=1;
        s->path = (char *)malloc(size_n);

        if(s->path == NULL)
            goto ERR2;

        if(s->path && path)
            strcpy(s->path,path);

        else 
            sprintf(s->path,"%s_%d",DEFAULT_PATH,default_path_cnt++);

        s->fp = fopen(s->path,"w+");
        if(s->fp == NULL){
            printf("fopen path fail :%s\n",s->path);
            goto ERR3;
        }
        printf("s->fp:%p\n",s->fp) ;
        s->logcnt = 0;
        s->level = level;

        
        pthread_mutex_init(&s->cache_lock,NULL);
        pthread_mutex_init(&s->cond_lock,NULL);
        pthread_cond_init(&s->wait_d,NULL);
        pthread_create(&s->write_t,NULL, basic_log_write_file,s);


        return s;

    ERR3:

        free(s->path);

    ERR2:
        free(s->p);

    ERR1:
        
        if(s->logs_name)
            free(s->logs_name);
        
        free(s);
        
        return NULL;

}

void    baisc_log_destory(basic_log_t *log_t)
{
    
    if(!log_t)
        return ;
    
    while(log_t->p != log_t->pos){
        pthread_cond_signal(&log_t->wait_d);
        sleep(1);
    }
    
    if(log_t->fp)
        fclose(log_t->fp);
    log_t->fp = NULL;
    pthread_cond_signal(&log_t->wait_d);
    pthread_join(log_t->write_t,NULL);
    
    if(log_t->logs_name)
        free(log_t->logs_name);
    if(log_t->path)
        free(log_t->path);
    if(log_t->p)
        free(log_t->p);
    log_t->p = NULL;
    
    pthread_mutex_destroy(&log_t->cache_lock);
    pthread_mutex_destroy(&log_t->cond_lock);
    pthread_cond_destroy(&log_t->wait_d);
   
    free(log_t);
    
    return;

}
static int testnn = 0;
void    basic_log_error_core(unsigned int  level, basic_log_t *s ,
        const char *fmt, ...)
{
   
    if(s == NULL){
        fprintf(stderr,"log server not open\n");
        return;
    }

   // printf("logp:%p level :%d s->level:%d\n",s,level,level);

    if(level > s->level)
        return ;
        char logs[512];
    va_list args;
    unsigned int n;
    va_start(args,fmt);
    n = vsnprintf(logs, 512,fmt,args);
    va_end(args);
    if(n <= 0 ){
        fprintf(stderr,"log fmt is err\n");
        return;
    }
    
    fprintf(stderr,"%s\n",logs);
    
    if(n > s->size){
        fprintf(stderr,"cache size is samlle!\n");
        return;
    }
    //quitkly mode ,maybe lost messages of write file
    pthread_mutex_lock(&s->cache_lock);
    if((s->size - (s->pos - s->p)) < n){
        pthread_mutex_unlock(&s->cache_lock);
        while((s->size - (s->pos - s->p)) < n)
        {
            pthread_cond_signal(&s->wait_d);
            usleep(10000);
        }
        pthread_mutex_lock(&s->cache_lock);
        memcpy(s->pos,logs,n);
        s->pos += n;
        s->logcnt++;
        pthread_mutex_unlock(&s->cache_lock);   
    }else{
        memcpy(s->pos,logs,n);
        s->pos += n;
        s->logcnt++;
        pthread_mutex_unlock(&s->cache_lock);   
    }
    return ;

}


