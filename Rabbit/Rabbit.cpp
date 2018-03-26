// Rabbit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef _WIN32
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

#include "SdlContainer.h"
#endif

int _tmain(int argc, _TCHAR* argv[])
{
	av_register_all();

	AVFormatContext *inputFmtCtx = NULL;

	char *src = "ffd5.f4v";
	int ret = avformat_open_input(&inputFmtCtx, src, NULL, NULL);
	if (ret < 0){
		return -1;
	}

	ret = avformat_find_stream_info(inputFmtCtx, NULL);
	if (ret < 0){
		return -1;
	}

	int videoIndex = 0;
	int audioIndex = 0;
	AVCodecContext *videoCdcCtx = NULL;

	for (int i = 0; i < inputFmtCtx->nb_streams; i++){
		if (inputFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			videoIndex = i;
			videoCdcCtx = inputFmtCtx->streams[i]->codec;
		} else if (inputFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			audioIndex = i;
		}
	}

	AVCodec *decoder = avcodec_find_decoder(videoCdcCtx->codec_id);
	if (decoder == NULL){
		return -1;
	}

	ret = avcodec_open2(videoCdcCtx, decoder, NULL);
	if (ret < 0){
		return -1;
	}

	SdlContainer Sdl(videoCdcCtx->width, videoCdcCtx->height);
	if (!Sdl.initSDL()){
		return -1;
	}

	AVFrame *frame = av_frame_alloc();
	AVFrame *framYUV = av_frame_alloc();

	unsigned char *outBuffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  videoCdcCtx->width, videoCdcCtx->height,1));
	av_image_fill_arrays(framYUV->data, framYUV->linesize,outBuffer, AV_PIX_FMT_YUV420P,videoCdcCtx->width, videoCdcCtx->height,1);

	SwsContext *swsCtx = sws_getContext(videoCdcCtx->width, videoCdcCtx->height, videoCdcCtx->pix_fmt, videoCdcCtx->width, videoCdcCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (swsCtx == NULL){
		return -1;
	}

	SDL_Event event;
	while(true){
		SDL_WaitEvent(&event);
		if (event.type == FRAME_REFRESH_EVENT){

			AVPacket *pkt = (AVPacket *)av_malloc(sizeof(AVPacket));
			while (true){
			    ret = av_read_frame(inputFmtCtx, pkt);
				if (ret < 0){
					break;
				}

				if (pkt->stream_index != videoIndex){
					av_free_packet(pkt);
					continue;
				} else {
					break;
				}
			}

			if (ret < 0){
				break;
			}
		
			int gotPicture = 0;
			ret = avcodec_decode_video2(videoCdcCtx, frame, &gotPicture, pkt);
			if (gotPicture){
				sws_scale(swsCtx, frame->data, frame->linesize, 0, videoCdcCtx->height, framYUV->data, framYUV->linesize);
				Sdl.render(framYUV);
			}
			av_free_packet(pkt);
		}
	}

	av_frame_free(&frame);
	av_frame_free(&framYUV);
	sws_freeContext(swsCtx);
	avcodec_close(videoCdcCtx);
	avformat_close_input(&inputFmtCtx);

	return 0;
}

