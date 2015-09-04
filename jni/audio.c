#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define 	out_sample_fmt AV_SAMPLE_FMT_S16
AVFormatContext *pFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVPacket *packet;
AVFrame *pFrame;
SwrContext *au_convert_ctx;
int audioIndex=-1;
int ret,r;
int finish;
int out_sample_rate;
int out_channels; 
uint8_t *out_buffer;
int out_buffer_size;
int i;
int index1=0;
AVRational ran;
static void (*play_callback)()=NULL;
void set_play_callback(void (*callback)()){
	play_callback=callback;
}
void cacu(){
	int time=packet->pts*av_q2d(ran);
	int h=time/3600;
	int m=(time-h*3600)/60;
	int s=time%60;
	printf("%d:%d:%02d\n",h,m,s);
	//sprintf(data,"%d:%d",m,s);
}
void play(){
	if(packet->size==0){
		//printf("sss1\n");
		av_free_packet(packet);
		ret=av_read_frame(pFormatCtx,packet);
	}
	//printf("ss\n");
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
			//printf("audio:%d\t index:%d\t buffer size:%d\t packet size:%d\t ret:%d\t channel:%d\n",audioIndex,index1,out_buffer_size,packet->size,ret,pCodecCtx->channels);
			packet->data+=ret;
			packet->size-=ret;
			index1++;
			usleep(30000);
		}else{
			return;
		}
		cacu();
	}else{
		packet->size=0;
		play();
	}
	play_callback();
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
}
int setData(const char *file){
	if(avformat_open_input(&pFormatCtx,file,NULL,NULL)!=0)return -1;
	if(avformat_find_stream_info(pFormatCtx,NULL)<0)return -2;
	av_dump_format(pFormatCtx,-1,file,0);
	audioIndex=av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &pCodec, 0);
	if(audioIndex<0)return -3;
	pCodecCtx = pFormatCtx->streams[audioIndex]->codec;
	ran= pFormatCtx->streams[audioIndex]->time_base;
	uint64_t duration = pFormatCtx->streams[audioIndex]->duration;
	uint64_t i=duration*av_q2d(ran);
	printf("%d  %d:%d",i,i/60,i%60 );
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
		i=0;
	}
	return 0;
}
int main(int argc,const char *argv[]){
	av_register_all();
	setData(argv[1]);
	puts("play");
	set_play_callback(play);
	//play();
}
