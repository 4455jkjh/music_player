#include "native-audio-jni.h"
#include <android/log.h> 
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "onload.h"
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define 	out_sample_fmt AV_SAMPLE_FMT_S16
#define VLOGI(a,b) ((void)__android_log_vprint(ANDROID_LOG_INFO, "ffmpegaudio", a,b)) 
#define VLOGD(a,b) ((void)__android_log_vprint(ANDROID_LOG_DEBUG, "ffmpegaudio", a,b)) 
#define VLOGW(a,b) ((void)__android_log_vprint(ANDROID_LOG_WARN, "ffmpegaudio", a,b)) 
#define VLOGE(a,b) ((void)__android_log_vprint(ANDROID_LOG_ERROR, "ffmpegaudio", a,b)) 
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ffmpegaudio", __VA_ARGS__)) 
AVFormatContext *pFormatCtx,*format;
AVCodecContext *pCodecCtx,*codec;
AVCodec *pCodec,*dec;
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
int tni;
int ret1;
void read(){
	if(packet->size==0){
		av_packet_unref(packet);
		ret=av_read_frame(pFormatCtx,packet);
	}
}
void play(){
	if(pause==1){
		return;
	}
	read();
	if(ret!=0){
		calljava(1);
		return;
	}
	if(packet->stream_index==audioIndex){
		ret1=avcodec_decode_audio4(pCodecCtx,pFrame,&finish,packet);
		packet->size-=ret1;
		if(finish){
    		r=swr_convert(au_convert_ctx,&out_buffer, MAX_AUDIO_FRAME_SIZE,(const uint8_t **)pFrame->data , pFrame->nb_samples);
    		out_buffer_size=av_samples_get_buffer_size(NULL,out_channels ,r,out_sample_fmt, 1);
			AudioWrite(out_buffer,out_buffer_size);
			//av_log(NULL,AV_LOG_INFO,"tni:%d packet size:%6d ret:%6d",tni,packet->size,ret);
		}else{
			if(ret==0){
				read();
				play();
			}
		}
		calljava(2);
	}else{
		packet->size=0;
		read();
		play();
	}

}
void logg(void *a,int b,const char *c,va_list d){
	switch(b){
		case AV_LOG_INFO:
			VLOGI(c,d);
			break;
		case AV_LOG_DEBUG:
			VLOGD(c,d);
			break;
		case AV_LOG_WARNING:
			VLOGW(c,d);
			break;
		case AV_LOG_ERROR:
			VLOGE(c,d);
			break;
	}
}
void init(JNIEnv *env,jobject clz){
	LOGI("%s","init");
	init1(env,clz);
	createEngine();
	av_register_all();
	set_play_callback(play);
	av_log_set_callback(logg);
}
void release(JNIEnv *env,jclass clz){
	if(i==0)
		return;
	LOGI("%s","release");
	pause=1;
	usleep(10000);
	swr_free(&au_convert_ctx);
    av_free(pFrame);  
    avcodec_close(pCodecCtx);  
    avformat_close_input(&pFormatCtx); 
    i=0;
}
void play1(JNIEnv *env,jclass clz){
	LOGI("%s","play");
	pause=0;
	play();
}   
void pause1(JNIEnv *env,jclass clz){
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
jint setData(JNIEnv *env,jclass clz,jstring name){
	pause=1;
	i=0;
	tni=0;
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
	av_log(NULL,AV_LOG_INFO,"解码器:%s",pCodec->name);
	ran= pFormatCtx->streams[audioIndex]->time_base;
	duration = pFormatCtx->streams[audioIndex]->duration;
	if(avcodec_open2(pCodecCtx,pCodec,NULL)<0)return -4;
	pFrame=av_frame_alloc();
	if(pFrame==NULL)return -5;
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_init_packet(packet);
	ret=av_read_frame(pFormatCtx,packet);
	init_swr();
	i=1;
	(*env)->ReleaseStringUTFChars(env,name,file);
	return 0;
}
jboolean isPlaying(JNIEnv *env,jclass clz)
{
	return pause==0?JNI_FALSE:JNI_TRUE;
}
jboolean isinit(JNIEnv *env,jclass clz)
{
	return i==0?JNI_FALSE:JNI_TRUE;
}

jlong getcur(JNIEnv *env,jclass clz)
{
	uint64_t t=packet->pts*av_q2d(ran);
	return t;
}
jlong gettotal(JNIEnv *env,jclass clz)
{
	uint64_t t=duration*av_q2d(ran);
	return t;
}
char info1[1000];
int lenth,d,m,s;
jstring getinfo(JNIEnv *env,jclass clz,jstring name){
	LOGI("%s","getinfo");
	//AVCodecContext *codec;
	AVDictionaryEntry *m = NULL;
	const char *file=(*env)->GetStringUTFChars(env,name,0);
	if(avformat_open_input(&format,file,NULL,NULL)!=0)
		goto end;
	if(avformat_find_stream_info(format,NULL)<0)
		goto end;
	int a=av_find_best_stream(format, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
	codec = format->streams[a]->codec;
	AVRational rr= format->streams[a]->time_base;
	d= (format->streams[a]->duration)*av_q2d(rr);
	m=(d%3600)/60;
	s=d%60;
	/*char metadata[200];
while(m=av_dict_get(format->metadata,"",m,AV_DICT_IGNORE_SUFFIX)){  
    sprintf(metadata,"%s:%s\n",m->key,m->value) ;
}*/
	snprintf(info1,sizeof(info1),
	"文件名: %s\n"
	"时长: %d:%02d\n"
	"解码器: %s\n"
	"声道数: %d\n"
	"采样率: %dHz\n"
	"比特率: %dKbps",
	format->filename,
	m,s,
	dec->name,
	codec->channels,
	codec->sample_rate,
	codec->bit_rate/1000);
	avcodec_close(codec);
	avformat_close_input(&format); 
	(*env)->ReleaseStringUTFChars(env,name,file);
end:
	return 	(*env)->NewStringUTF(env,info1);
}
int curindex=0;
void putindex(JNIEnv *e,jobject o,jint tnij){
	curindex=tnij;
}
jint getindex(JNIEnv *e,jobject o){
	return curindex;
}
static JNINativeMethod methods[] = {
	{ "init", "()V", (void*)init },
	{ "release", "()V", (void*)release },
	{ "play", "()V", (void*)play1},
	{ "pause", "()V", (void*)pause1 },
	{ "setData", "(Ljava/lang/String;)I", (void*)setData },
	{ "isPlaying", "()Z", (void*)isPlaying},
	//{ "isinit", "()Z", (void*) isinit},
	{ "getcur", "()J", (void*)getcur},
	{ "gettotal", "()J", (void*)gettotal },
	{ "getinfo", "(Ljava/lang/String;)Ljava/lang/String;", (void*)getinfo},
	//{"getindex","()I",(void*)getindex},
	//{"putindex","(I)V",(void*)putindex},
};
int register_audio(JNIEnv *env) {
	return jniRegisterNativeMethods(env, "ffmpeg/audio/audio", methods, sizeof(methods) / sizeof(methods[0]));
}
