//音视频播放进阶学习1
//ffmpeg_video_audio_1利用FFMPEG和SDL实现视频流和音频流的同时播放(不同步)
//下一步实现音频流和视频流的播放的同步，音频重采样参考FFMPE提供的example中resampling_audio.c
//
#include "stdafx.h"
//#include "ffmpeg_video_audio.h"
//
//#define SDL_AUDIO_BUFFER_SIZE 1024
//#define MAX_AUDIO_FRAME_SIZE 192000
//
////数据包队列
//typedef struct PacketQueue {
//	AVPacketList *first_pkt, *last_pkt;
//	int nb_packets;
//	int size;
//	SDL_mutex *mutex;
//	SDL_cond *cond;
//} PacketQueue;
//
//PacketQueue audioq; //音频队列
//
//int quit = 0;
//
////队列初始化函数
//void packet_queue_init(PacketQueue *q) {
//	memset(q, 0, sizeof(PacketQueue));
//	q->mutex = SDL_CreateMutex();
//	q->cond = SDL_CreateCond();
//}
//
////入队函数
//int packet_queue_put(PacketQueue *q, AVPacket *pkt) {
//
//	AVPacketList *pkt1;               //数据包的链表
//	if (av_dup_packet(pkt) < 0) {
//		return -1;
//	}
//	pkt1 = (AVPacketList *)av_malloc(sizeof(AVPacketList));
//	if (!pkt1)
//		return -1;
//	pkt1->pkt = *pkt;
//	pkt1->next = NULL;
//
//
//	SDL_LockMutex(q->mutex);   //加锁
//
//	if (!q->last_pkt)
//		q->first_pkt = pkt1;
//	else
//		q->last_pkt->next = pkt1;
//
//	q->last_pkt = pkt1;
//	q->nb_packets++;
//	q->size += pkt1->pkt.size;
//	SDL_CondSignal(q->cond);        //重启等待信号的变量
//
//	SDL_UnlockMutex(q->mutex);    //解锁
//	return 0;
//}
//
////出队函数
//static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block)
//{
//	AVPacketList *pkt1;         //数据包的链表
//	int ret;
//
//	SDL_LockMutex(q->mutex);    //加锁
//
//	for (;;) {
//
//		if (quit) {
//			ret = -1;
//			break;
//		}
//
//		pkt1 = q->first_pkt;
//		if (pkt1) {
//			q->first_pkt = pkt1->next;
//			if (!q->first_pkt)
//				q->last_pkt = NULL;
//			q->nb_packets--;
//			q->size -= pkt1->pkt.size;
//			*pkt = pkt1->pkt;
//			av_free(pkt1);
//			ret = 1;
//			break;
//		}
//		else if (!block) {
//			ret = 0;
//			break;
//		}
//		else {
//			SDL_CondWait(q->cond, q->mutex);      //等待条件变量，解锁
//		}//end if
//	}//end for
//	SDL_UnlockMutex(q->mutex);
//	return ret;
//}
//
//
////音频解码函数
//int audio_decode_frame(AVCodecContext *aCodecCtx, uint8_t *audio_buf, int buf_size) {
//
//	static AVPacket pkt;
//	static uint8_t *audio_pkt_data = NULL;
//	static int audio_pkt_size = 0;
//	static AVFrame *frame = av_frame_alloc();
//	int got_frame = 0;
//
//	int len1, data_size = 0;
//
//	FILE *pFile = fopen("output.pcm", "a+");
//
//	uint8_t **dst_data = NULL;
//	int dst_linesize = 0;
//
//	//test
//	//int out_nb_samples;
//	int64_t out_channel_layout;    //double channels
//	AVSampleFormat out_sample_fmt;    //Audio sample formats
//	int out_sample_rate;
//	int out_channels;  //channel numbers
//	struct SwrContext *aud_convert_ctx;
//	static int pframm = 0;
//	int ret = 0;
//
//	//Audio out parameter:
//	//nb_samples: AAC-1024 MP3-1152
//	//out_nb_samples = aCodecCtx->frame_size;
//	printf("%d\n",aCodecCtx->channels);
//	out_channel_layout = AV_CH_LAYOUT_STEREO;
//	out_sample_fmt = AV_SAMPLE_FMT_S16;    //Audio sample formats
//	out_sample_rate = aCodecCtx->sample_rate;
//	out_channels = av_get_channel_layout_nb_channels(out_channel_layout);  //channel numbers
//	printf("%d\n", out_channels);
//	//因为解码的数据类型是4位，播放器播放的是16位需要重采样
//	aud_convert_ctx = swr_alloc_set_opts(NULL, out_channel_layout, out_sample_fmt, out_sample_rate,
//		av_get_default_channel_layout(aCodecCtx->channels), aCodecCtx->sample_fmt, aCodecCtx->sample_rate, 0, NULL);
//	swr_init(aud_convert_ctx);   //初始化
//
//
//	for (;;) {
//		while (audio_pkt_size > 0) {			
//			len1 = avcodec_decode_audio4(aCodecCtx, frame, &got_frame, &pkt);
//			if (len1 < 0) {
//				//解码出错
//				audio_pkt_size = 0;
//				break;
//			}
//			audio_pkt_data += len1;
//			audio_pkt_size -= len1;
//
//
//			ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, out_channels,
//				out_sample_rate, (AVSampleFormat)out_sample_fmt, 0);
//
//			if (got_frame)
//			{
//				//printf("Decode %d Frames\n", pframm++);
//				ret = swr_convert(aud_convert_ctx, dst_data, out_sample_rate,(const uint8_t **)frame->data, frame->nb_samples);
//				if (ret < 0)
//				{
//					continue;
//				}
//				//返回给定音频参数的字节数
//				data_size = av_samples_get_buffer_size(&dst_linesize, out_channels,ret, (AVSampleFormat)out_sample_fmt, 1);			
//				if (data_size <= 0) {
//					continue;
//				}		
//			
//			}
//			memcpy(audio_buf, dst_data[0], data_size);
//			/* We have data, return it and come back for more later */
//
//			if (dst_data)
//			{
//				av_freep(&dst_data[0]);
//			}
//			av_freep(&dst_data);
//			dst_data = NULL;
//
//			swr_free(&aud_convert_ctx);
//			return data_size;
//		}//end while
//
//		if (pkt.data)
//			av_free_packet(&pkt);
//
//		if (quit) {
//			return -1;
//		}
//
//		if (packet_queue_get(&audioq, &pkt, 1) < 0) {        //在队列中取出一个数据
//			return -1;
//		}
//		audio_pkt_data = pkt.data;
//		audio_pkt_size = pkt.size;
//	}//end for
//}
//
//
////音频的回调函数
//void audio_callback(void *userdata, Uint8 *stream, int len) {
//	//SDL 2.0
//	SDL_memset(stream, 0, len);
//
//	AVCodecContext *aCodecCtx = (AVCodecContext *)userdata;
//	int len1, audio_size;
//
//	//audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，
//	//这些数据待copy到SDL缓冲区， 当audio_buf_index >= audio_buf_size的时候意味着我
//	//们的缓冲为空，没有数据可供copy，这时候需要调用audio_decode_frame来解码出更多的桢数据 
//	static uint8_t audio_buf[MAX_AUDIO_FRAME_SIZE*3/2];
//	static unsigned int audio_buf_size = 0;
//	static unsigned int audio_buf_index = 0;
//	
//	while (len > 0) {
//		if (audio_buf_index >= audio_buf_size) {
//			/* We have already sent all our data; get more */
//			audio_size = audio_decode_frame(aCodecCtx, audio_buf, audio_buf_size);  //解码音频帧数据
//			
//			//标示没能解码出数据，默认播放静音
//			if (audio_size < 0) {
//				/* If error, output silence */
//				audio_buf_size = 1024; // arbitrary?
//				memset(audio_buf, 0, audio_buf_size);
//			}
//			else {
//				audio_buf_size = audio_size;
//			}//end if
//			audio_buf_index = 0;
//		}//end if
//
//		// 查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy 
//		len1 = audio_buf_size - audio_buf_index;
//		if (len1 > len)
//			len1 = len;		
//
//		memcpy(stream, (uint8_t *)audio_buf + audio_buf_index, len1);
//		len -= len1;
//		stream += len1;
//		audio_buf_index += len1;
//	} //end while
//}
//
//int main(int argc, char *argv[]) {
//	AVFormatContext *pFormatCtx = NULL;
//	int             i, index_video = -1, index_audio = -1;
//	AVCodecContext  *pCodecCtx = NULL;
//	AVCodec         *pCodec = NULL;
//	AVFrame         *pFrame = NULL,*pFrameYUV = NULL;
//	uint8_t         *out_buffer = NULL;
//	AVPacket        *pkt;
//	int             frameFinished;
//	struct SwsContext *img_convert_ctx = NULL;
//
//	//音频编解码上下文
//	AVCodecContext  *aCodecCtx = NULL;
//	AVCodec         *aCodec = NULL;
//
//
//	//SDL2.0
//	SDL_Window      *screen;
//	SDL_Renderer    *sdlrenderer;
//	SDL_Texture     *sdltexture;
//	SDL_Rect        sdlrect;
//	SDL_Event       event;
//	SDL_AudioSpec   desired,spec;
//	
//	//音频
//	int out_nb_samples;
//	int64_t out_channel_layout;    //double channels
//	AVSampleFormat out_sample_fmt;    //Audio sample formats
//	int out_sample_rate;
//	int out_channels;  //channel numbers
//	uint8_t *out_buffer_audio;
//	int out_buffer_size;
//	struct SwrContext *aud_convert_ctx;
//
//
//
//	char * filename = "forrest.mp4";
//
//	// Register all formats and codecs
//	av_register_all();
//	avformat_network_init();
//	pFormatCtx = avformat_alloc_context(); //申请内存
//
//	if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) < 0) //打开输入视频文件
//	{
//		printf("Can't open the input stream.\n");
//		return -1;
//	}
//	if (avformat_find_stream_info(pFormatCtx, NULL)<0)     //从音频文件中得到流媒体信息
//	{
//		printf("Can't find the stream information!\n");
//		return -1;
//	}
//	//输出相关信息
//	av_dump_format(pFormatCtx, 0, filename, 0);
//
//	//查找音视频流数据
//	for (i = 0; i < pFormatCtx->nb_streams; i++)
//	{
//		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)      //如果是视频流，则记录下索引。
//		{
//			index_video = i;
//		}
//		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO)      //如果是音频流，则记录下索引。
//		{
//			index_audio = i;
//			break;
//		}
//	}
//	if (index_video == -1)
//	{
//		printf("Can't find a video stream;\n");
//		return -1;
//	}
//	if (index_audio == -1)
//	{
//		printf("Can't find a audio stream;\n");
//		return -1;
//	}
//	//查找音频解码器信息
//	aCodecCtx = pFormatCtx->streams[index_audio]->codec;
//
//	//查找视频解码器信息
//	pCodecCtx = pFormatCtx->streams[index_video]->codec;
//	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);     //查找解码器
//	if (pCodec == NULL)
//	{
//		printf("Can't find a decoder!\n");
//		return -1;
//	}
//
//	//打开编码器
//	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
//	{
//		printf("Can't open the decoder!\n");
//		return -1;
//	}
//	//存储解码后数据
//	pFrame = av_frame_alloc();  //this only allocates the AVFrame itself, not the data buffers
//	pFrameYUV = av_frame_alloc();
//	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));  //开辟缓冲区
//	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);//帧和申请的内存结合
//
//	//存储解码前数据
//	pkt = (AVPacket *)av_malloc(sizeof(AVPacket));;
//	av_init_packet(pkt);
//	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
//		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//
//	//SDL2.0
//	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
//		fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
//		exit(1);
//	}
//	int screen_w = pCodecCtx->width, screen_h = pCodecCtx->height;
//	screen = SDL_CreateWindow("SDL EVENT TEST", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE); //创建显示窗口
//	if (screen == NULL)
//	{
//		printf("Can't creat a window:%s\n", SDL_GetError());
//		return -1;
//	}
//	sdlrenderer = SDL_CreateRenderer(screen, -1, 0);//创建渲染器
//	sdltexture = SDL_CreateTexture(sdlrenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);//创建纹理
//
//
//	desired.freq = aCodecCtx->sample_rate;        //音频数据的采样率
//	desired.format = AUDIO_S16SYS;		          //音频数据的格式
//	desired.channels = aCodecCtx->channels;       //声道数
//	desired.silence = 0;                          //静音时的值
//	desired.samples = SDL_AUDIO_BUFFER_SIZE;      //音频缓冲区中的采样个数
//	desired.callback = audio_callback;            //回调函数
//	desired.userdata = aCodecCtx;
//	
//	aCodec = avcodec_find_decoder(aCodecCtx->codec_id);     //查找解码器
//	if (aCodec == NULL)
//	{
//		printf("Can't find a decoder!\n");
//		return -1;
//	}
//	if (avcodec_open2(aCodecCtx, aCodec, NULL) < 0)   //打开编码器
//	{
//		printf("Can't open the decoder!\n");
//		return -1;
//	}
//
//	if (SDL_OpenAudio(&desired, &spec)<0){
//		printf("can't open audio.\n");
//		return -1;
//	}
//	packet_queue_init(&audioq);
//	SDL_PauseAudio(0);
//
//
//	while (av_read_frame(pFormatCtx, pkt) >= 0) {
//		// 判断是否是视频流
//		if (pkt->stream_index == index_video) {
//			// 解码视频包数据
//			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, pkt);
//			// 得到帧数据
//			if (frameFinished) {
//				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//					pFrameYUV->data, pFrameYUV->linesize);
//
//				SDL_UpdateTexture(sdltexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]); //设置纹理的数据
//
//				//设置窗口大小
//				sdlrect.x = 0;
//				sdlrect.y = 0;
//				sdlrect.w = screen_w;
//				sdlrect.h = screen_h;
//
//				SDL_RenderCopy(sdlrenderer, sdltexture, NULL, &sdlrect); //复制纹理信息到渲染器目标
//				SDL_RenderPresent(sdlrenderer);//视频渲染显示
//				av_free_packet(pkt);
//			}
//		}else if (pkt->stream_index == index_audio) {
//			packet_queue_put(&audioq, pkt);
//		}
//		else
//		{
//			av_free_packet(pkt);
//		}
//		SDL_Delay(10);
//
//		SDL_PollEvent(&event);
//		switch (event.type) {
//		case SDL_QUIT:
//			quit = 1;
//			SDL_Quit();
//			exit(0);
//			break;
//		default:
//			break;
//		}//end switch
//
//	}//end while
//	
//	sws_freeContext(img_convert_ctx);
//
//	// Free the YUV frame
//	av_free(pFrame);
//	av_frame_free(&pFrameYUV);
//	// Close the codec
//	avcodec_close(pCodecCtx);
//
//	// Close the video file
//	avformat_close_input(&pFormatCtx);
//
//	return 0;
//}
