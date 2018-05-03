
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "adi_types.h"
#include "adi_gpio.h"
#include "xw_msg_prv.h"
#include "xw_key_get.h"
#include "xw_logsrv.h"
#include "xw_text_prompt_box.h"


typedef struct 	key_state_param{
	
	int 	key_recod;
	int 	key_ae;
	int 	key_ae_value;
	int 	key_awb_push;
	int 	key_staturation_value;
	int 	key_denosize_value;
	int 	key_freeze;
	
}Key_State_Params;

Key_State_Params _keystateparams;
pthread_t 	 _key_thread_id;
static 	int 	key_thread_quit;

static void*	xw_key_run(void *data);

int xw_key_show(void *data)
{
	
	memset(&_keystateparams,0x0,sizeof(Key_State_Params));
	key_thread_quit = 0;
	pthread_create(&_key_thread_id,NULL,xw_key_run,NULL);
	return 0;
	
}


static void   xw_key_get(img_msg_cmd_t *xcmd)
{
	
	img_msg_cmd_t icmd = IDSCAM_IMG_MSG_NO ;
	img_msg_cmd_t 	arry[11];;
	//1-3 key 1
	//key1:		AE
	//key2:		AWB
	//key3:		freeze
	arry[0] = IDSCAM_IMG_MSG_NO;
	arry[1] = IDSCAM_IMG_MSG_NO;
	arry[2] = IDSCAM_IMG_MSG_NO;
	arry[3] = IDSCAM_IMG_MSG_NO;
	arry[4] = IDSCAM_IMG_MSG_ENABLE_AE;
	arry[5] = IDSCAM_IMG_MSG_ENABLE_AE;
	arry[6] = IDSCAM_IMG_MSG_ENABLE_AWB;
	arry[7] = IDSCAM_IMG_MSG_ENABLE_AWB;
	arry[8] = IDSCAM_IMG_MSG_FREEZE;
	arry[9] = IDSCAM_IMG_MSG_FREEZE;
	arry[10]= IDSCAM_IMG_MSG_NO;

	GADI_GPIO_AdcValue  adcval;
	adcval.value = 0;
	GADI_ERR err;
	int k = 0;
	
	adcval.channel =  GADI_GPIO_ADC_CHANNEL_ONE;
	err = gadi_gpio_read_adc(&adcval);
	if( err == GADI_OK)
	{
		k 	= adcval.value/100;
		icmd 	= arry[k];
	}
	if(adcval.value > 50){
		xw_logsrv_err("gat adc vaule:%d\n",adcval.value);
	}
	if(icmd != IDSCAM_IMG_MSG_NO){
		*xcmd = icmd;
		return ;
	}

	//4-7 //key2 
	//key7: 	sanp
	//key6: 	recod
	//key5: 	statution
	//key4:		desenion
	
	arry[0] = IDSCAM_IMG_MSG_NO;
	arry[1] = IDSCAM_IMG_MSG_NO;
	arry[2] = IDSCAM_IMG_MSG_CAPTURE;
	arry[3] = IDSCAM_IMG_MSG_CAPTURE;
	arry[4] = IDSCAM_IMG_MSG_SATURATION;
	arry[5] = IDSCAM_IMG_MSG_SATURATION;
	arry[6] = IDSCAM_IMG_MSG_DENOISE;
	arry[7] = IDSCAM_IMG_MSG_DENOISE;
	arry[8] = IDSCAM_IMG_MSG_RECORED_START;
	arry[9] = IDSCAM_IMG_MSG_RECORED_START;
	arry[10] = IDSCAM_IMG_MSG_NO;

	adcval.channel =  GADI_GPIO_ADC_CHANNEL_TWO;
	adcval.value   = 0;
	err = gadi_gpio_read_adc(&adcval);
	if(err == GADI_OK)
	{
		k 	= adcval.value/100;
		icmd 	= arry[k];
	}
 	if(adcval.value > 50){
		xw_logsrv_err("gat adc vaule:%d\n",adcval.value);
	}
	if(icmd != IDSCAM_IMG_MSG_NO){
		*xcmd = icmd;
		return ;
	}

		
	return ;	
}


static void xw_key_process(img_msg_cmd_t xcmd)

{
	int value = 0;
	char sbuf[12];
	xw_logsrv_err("xcmd:%d\n",xcmd);	
	switch(xcmd)
	{
		case IDSCAM_IMG_MSG_CAPTURE:{
			Image_Msg_Send(xcmd,NULL,0);
			break;
		}
		case IDSCAM_IMG_MSG_RECORED_START:{
			if(_keystateparams.key_recod == 0){
				_keystateparams.key_recod = 1;
				value = _keystateparams.key_recod;
				Image_Msg_Send(xcmd,&value,4);
				xw_time_cnt_start(1);

			}else{
				_keystateparams.key_recod = 0;
				value = _keystateparams.key_recod;
				Image_Msg_Send(IDSCAM_IMG_MSG_RECORED_STOP,&value,4);
				xw_time_cnt_start(0);

			}
			break;
		}
		case IDSCAM_IMG_MSG_SATURATION:
		{
			_keystateparams.key_staturation_value +=10;
			if(_keystateparams.key_staturation_value > 100 )
				_keystateparams.key_staturation_value = 0;
			value = _keystateparams.key_staturation_value;
			Image_Msg_Send(xcmd,&value,4);		
			sprintf(sbuf,"stauration:%d",value);
			xw_text_promt_put(sbuf,2000);
			break;
		}
		case IDSCAM_IMG_MSG_DENOISE:{
			_keystateparams.key_denosize_value +=10;
			if(_keystateparams.key_denosize_value > 100)
				_keystateparams.key_denosize_value = 0;
			value = _keystateparams.key_denosize_value;
			Image_Msg_Send(xcmd,&value,4);
			sprintf(sbuf,"denoise:%d",value);
			xw_text_promt_put(sbuf,2000);

			break;
		}
		case IDSCAM_IMG_MSG_ENABLE_AE:{
			if(_keystateparams.key_ae == 0){
				_keystateparams.key_ae = 1;
			  	strcpy(sbuf,"auto");
			}else{
				strcpy(sbuf,"manual");
				_keystateparams.key_ae = 0;
			}
			value = _keystateparams.key_ae_value;
			Image_Msg_Send(xcmd,&value,4);
			xw_text_promt_put(sbuf,2000);

			break;
		}
		case   IDSCAM_IMG_MSG_ENABLE_AWB:{
			if(_keystateparams.key_awb_push == 0){
				_keystateparams.key_awb_push = 1;
				strcpy(sbuf,"awb");
			}else{
				_keystateparams.key_awb_push = 0;
				strcpy(sbuf,"push");
			}
			value = _keystateparams.key_awb_push;
			Image_Msg_Send(xcmd,&value,4);
			xw_text_promt_put(sbuf,2000);
			break;
		}
		case   IDSCAM_IMG_MSG_FREEZE:{
			if(_keystateparams.key_freeze == 0){
				_keystateparams.key_freeze = 1;
				strcpy(sbuf,"freeze");
			}else{
				strcpy(sbuf,"unfreeze");
				_keystateparams.key_freeze = 0;
			}
			value = _keystateparams.key_freeze;
			Image_Msg_Send(xcmd,&value,4);
			xw_text_promt_put(sbuf,2000);
			break;				

		}
		default:
			break;
	}		
	
}

static void*	xw_key_run(void *data)
{


	img_msg_cmd_t xcmd;
	key_thread_quit = 0;
	while(1)
	{	
		xcmd = IDSCAM_IMG_MSG_NO;
		xw_key_get(&xcmd);
		if(xcmd != IDSCAM_IMG_MSG_NO)
		{
			xw_key_process(xcmd);
			usleep(200000);
		}	
		usleep(100000);
		if(key_thread_quit)
			break;

	}
	return NULL;	
}

void 	xw_key_quit(void)
{
	key_thread_quit = 1;
	return ;
}









