#ifndef __UI_SETUP__
#define __UI_SETUP__

//*****************************************************************************
//*****************************************************************************
//** Enumerated types
//*****************************************************************************
//*****************************************************************************
typedef enum
{
	IDSCAM_IMG_MSG_CAPTURE = 1,
	IDSCAM_IMG_MSG_RECORED_START,
	IDSCAM_IMG_MSG_RECORED_STOP,
	IDSCAM_IMG_MSG_GET_SCAN_PIC_NUM,
	IDSCAM_IMG_MSG_SCAN_PIC,
    IDSCAM_IMG_MSG_ENABLE_AE,
	IDSCAM_IMG_MSG_AE_SET_TARGET_RATIO,
    IDSCAM_IMG_MSG_ENABLE_AWB,
	IDSCAM_IMG_MSG_AWB_SET_ONE_PUSH,
    IDSCAM_IMG_MSG_ANTIFLICKER,
    IDSCAM_IMG_MSG_CONTRAST,
    IDSCAM_IMG_MSG_SATURATION,
	IDSCAM_IMG_MSG_BRIGHTNESS,
	IDSCAM_IMG_MSG_HUE,
	IDSCAM_IMG_MSG_SHARPNESS,
	IDSCAM_IMG_MSG_DENOISE,	
    IDSCAM_IMG_MSG_AWB_SET_RGB_GAIN,
    IDSCAM_IMG_MSG_AWB_SET_COLORTEMP,
    IDSCAM_IMG_MSG_MIRROR,
    IDSCAM_IMG_MSG_FLIP,
	IDSCAM_IMG_MSG_SET_BW_MODE,
	IDSCAM_IMG_MSG_SENSOR_HDR_OP_MODE,
	IDSCAM_EVENT_MSG_SDCARD_STATE,
	IDSCAM_EVENT_GET_CAPTURE_FILENAME,
	IDSCAM_EVENT_GET_RECORED_FILENAME,	
    IDSCAM_IMG_MSG_EXIT,
}img_msg_id_t;

typedef struct img_msg_s
{
    img_msg_id_t msg_id;
    int param[127];
}img_msg_t;



#endif
