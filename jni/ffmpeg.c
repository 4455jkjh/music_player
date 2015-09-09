#include "native-audio-jni.h"
#include <android/log.h> 
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "onload.h"
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define 	out_sample_fmt AV_SAMPLE_FMT_S16
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ffmpegaudio", __VA_ARGS__)) 
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVPacket *packet;
AVFrame *pFrame;
SwrContext *au_convert_ctx;
int audioIndex;
int ret,r;
int finish;
int out_sample_rate;
int out_channels; 
uint8_t *out_buffer;
int out_buffer_size;
int i;
int pause;
int ii;
AVRational ran;
void play(){
	if(pause==1){
		r=0;
		return;
	}
	if(packet->size==0){
		av_free_packet(packet);
		ret=av_read_frame(pFormatCtx,packet);
	}
	if(packet->stream_index==audioIndex){
		ret=avcodec_decode_audio4(pCodecCtx,pFrame,&finish,packet);
		if(finish){
			if (i==1){
    			r=swr_convert(au_convert_ctx,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);
    			out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,r,out_sample_fmt, 1);
    		}else{
    			out_buffer_size = av_samples_get_buffer_size(pFrame->linesize,pCodecCtx->channels,pFrame->nb_samples,pCodecCtx->sample_fmt, 1);
    			out_buffer=pFrame->data[0];
    		}
			AudioWrite(out_buffer,out_buffer_size);
			//LOGI("index:%d\taudio:%d\t buffer size:%d\t packet size:%d\t ret:%d\t channel:%d",ii,audioIndex,out_buffer_size,packet->size,ret,pCodecCtx->channels);
			packet->data+=ret;
			packet->size-=ret;
			ii=2;
		}else{
			ii=1;
		}
		calljava(ii);
	}else{
		packet->size=0;
		play();
	}
}
void Java_ffmpeg_audio_audio_init(JNIEnv *env,jobject clz){
	LOGI("%s","init");
	init1(env,clz);
	createEngine();
	av_register_all();
	set_play_callback(play);
}
void Java_ffmpeg_audio_audio_release(JNIEnv *env,jclass clz){
	if(i==0)
		return;
	pause=1;
	usleep(10000);
	swr_free(&au_convert_ctx);
    av_free(pFrame);  
    avcodec_close(pCodecCtx);  
    avformat_close_input(&pFormatCtx); 
    i=0;
}
void Java_ffmpeg_audio_audio_play(JNIEnv *env,jclass clz){
	LOGI("%s","play");
	pause=0;
	play();
}   
void Java_ffmpeg_audio_audio_pause(JNIEnv *env,jclass clz){
	LOGI("%s","pause");
	pause=1;
}
void init_swr(){
	uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;
	//nb_samples: AAC-1024 MP3-1152
	out_sample_rate=pCodecCtx->sample_rate;
	out_channels=av_get_channel_layout_nb_channels(out_channel_layout);

	out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*out_channels);
	  //FIX:Some Codec's Context Information is missing
	int in_channel_layout=av_get_default_channel_layout(pCodecCtx->channels);
	//Swr
	au_convert_ctx = swr_alloc();
	swr_alloc_set_opts(au_convert_ctx,out_channel_layout, out_sample_fmt,         out_sample_rate,
					 in_channel_layout,  pCodecCtx->sample_fmt , pCodecCtx->sample_rate,0, NULL);
	if(swr_init(au_convert_ctx)<0){
		au_convert_ctx=NULL;
	}
	createBufferQueueAudioPlayer(2,out_sample_rate);  
}
char info[50];
uint64_t duration;
jint Java_ffmpeg_audio_audio_setData(JNIEnv *env,jclass clz,jstring name){
	pause=1;
	i=0;
	LOGI("%s","setData");
	const char *file=(*env)->GetStringUTFChars(env,name,0);
	if(avformat_open_input(&pFormatCtx,file,NULL,NULL)!=0)return -1;
	if(avformat_find_stream_info(pFormatCtx,NULL)<0)return -2;
	av_dump_format(pFormatCtx,-1,file,0);
	audioIndex=-1;
	int index;
	audioIndex=av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &pCodec, 0);
	if(audioIndex<0)return -3;
	pCodecCtx = pFormatCtx->streams[audioIndex]->codec;
	ran= pFormatCtx->streams[audioIndex]->time_base;
	duration = pFormatCtx->streams[audioIndex]->duration;
	if(avcodec_open2(pCodecCtx,pCodec,NULL)<0)return -4;
	pFrame=av_frame_alloc();
	if(pFrame==NULL)return -5;
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);
	ret=av_read_frame(pFormatCtx,packet);
	if(av_sample_fmt_is_planar(pCodecCtx->sample_fmt)){
		init_swr();
		i=1;
	}else{
		i=2;
		createBufferQueueAudioPlayer(pCodecCtx->channels,pCodecCtx->sample_rate);
	}
	(*env)->ReleaseStringUTFChars(env,name,file);
	return 0;
}
jboolean Java_ffmpeg_audio_audio_isPlaying(JNIEnv *env,jclass clz)
{
	return pause==0?JNI_FALSE:JNI_TRUE;
}
jboolean Java_ffmpeg_audio_audio_isinit(JNIEnv *env,jclass clz)
{
	return i==0?JNI_FALSE:JNI_TRUE;
}

jlong Java_ffmpeg_audio_audio_getcur(JNIEnv *env,jclass clz)
{
	uint64_t t=packet->pts*av_q2d(ran);
	return t;
}
jlong Java_ffmpeg_audio_audio_gettotal(JNIEnv *env,jclass clz)
{
	uint64_t t=duration*av_q2d(ran);
	return t;
}
char info1[1000];
static void loggg(void *a,int b,const char *c,va_list d){
	vsprintf(info1,c,d);
}
jstring Java_ffmpeg_audio_audio_getinfo(JNIEnv *env,jclass clz,jstring name){
	LOGI("%s","getinfo");
	AVFormatContext *format;
	//AVCodecContext *codec;
	av_log_set_callback(loggg);
	const char *file=(*env)->GetStringUTFChars(env,name,0);
	if(avformat_open_input(&format,file,NULL,NULL)!=0)
		goto end;
	if(avformat_find_stream_info(format,NULL)<0)
		goto end;
	av_dump_format(pFormatCtx,-1,file,0);
	int i;
	for(i=0;i<format->nb_streams;i++){
		
	}
	    avformat_close_input(&format); 
	(*env)->ReleaseStringUTFChars(env,name,file);
end:
	return 	(*env)->NewStringUTF(env,info1);
}
