typedef struct
{
    int bufId;
    int streamId;
    int maxStreamNum;
}NETCAM_VIDEO_STREAM_Porpety;

int netcam_video_web_stream_init(NETCAM_VIDEO_STREAM_Porpety *streamPro, int streamNum);
int netcam_video_web_stream_exit();

