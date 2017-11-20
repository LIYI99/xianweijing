
#ifndef _AUDIO_PROCESS_H__
#define _AUDIO_PROCESS_H__

typedef enum {
    AUDIO_PROCESS_SAMPLE_RATE_8000  = 8000,
    AUDIO_PROCESS_SAMPLE_RATE_16000 = 16000,
} AUDIO_PROCESS_SmpleRateEnumT;

typedef enum {
    AUDIO_PROCESS_FALSE = 0,
    AUDIO_PROCESS_TRUE  = 1   
} AUDIO_PROCESS_BOOL;

typedef enum {
    AUDIO_PROCESS_AEC_DISABLE   = 0,
    AUDIO_PROCESS_AEC_FIXED, 
    AUDIO_PROCESS_AEC_FLOAT,     
} AUDIO_PROCESS_AecModeT;

typedef struct
{
    AUDIO_PROCESS_SmpleRateEnumT            sampleRate;
    AUDIO_PROCESS_BOOL                      nsEnable;
    AUDIO_PROCESS_BOOL                      agcEnable;
    AUDIO_PROCESS_AecModeT                  aecMode;
    /*0 ~ 3 default 2*/
    unsigned int                            nsParam;
    /*0 ~ 4 default 3*/
    unsigned int                            aecFixedParam;
    /*0 ~ 2 default 0*/
    unsigned int                            aecFloatParam;
    /*0 ~ 90 default 9*/
    unsigned int                            agcParam; 
    unsigned int                            frameSamples;
}audio_process_attribute;

#ifdef __cplusplus
extern "C" {
#endif

int audio_process_init(audio_process_attribute *pstAudioProcessAttr);
int audio_process_proc(void *ai_data, void *ao_data, void *aec_data);
int audio_process_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_PROCESS_H__ */
