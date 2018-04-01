// Rabbit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FFmpegCore.h"
#include "windows.h"

#include "SdlContainer.h"
int _tmain(int argc, _TCHAR* argv[])
{
	typedef bool(*apiInit)(int);
	typedef int(*apiEncryptFlvFile)(const char *, const char *);


	apiInit init = NULL;
	apiEncryptFlvFile encryptFlvFile = NULL;
	HINSTANCE dll = LoadLibrary(L"FlvProcess.dll");
	if (dll == NULL){
		return 0;
	}

	init = (apiInit)GetProcAddress(dll, "init");
// 	encryptFlvFile = (apiEncryptFlvFile)GetProcAddress(dll, "encryptFlvFile");
// 	if (!init || !encryptFlvFile){
// 		return 0;
// 	}
 	bool bInit = init(2);
	if (!bInit){
		return -1;
	}

	//encryptFlvFile("ffd5.f4v", "enc.flv");

	while(true){
		Sleep(2000);
	}

	return 0;
	AVFormatContext *inputFmtCtx = NULL;
	char *src = "encrypt.flv";

	FFmpegCore ffmCore(src);
	if (!ffmCore.initFFmpeg()){
		return -1;
	}

	SdlContainer Sdl(ffmCore.getVideoWidth(), ffmCore.getVideoHeight());
	if (!Sdl.initSDL()){
		return -1;
	}

	AVFrame *frame = av_frame_alloc();
	AVFrame *framYUV = av_frame_alloc();

	unsigned char *outBuffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  ffmCore.getVideoWidth(), ffmCore.getVideoHeight(),1));
	av_image_fill_arrays(framYUV->data, framYUV->linesize,outBuffer, AV_PIX_FMT_YUV420P,ffmCore.getVideoWidth(), ffmCore.getVideoHeight(),1);

	SwsContext *swsCtx = sws_getContext(ffmCore.getVideoWidth(), ffmCore.getVideoHeight(), ffmCore.getPixFormat(), ffmCore.getVideoWidth(), ffmCore.getVideoHeight(), AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (swsCtx == NULL){
		return -1;
	}

	SDL_Event event;
	while(true){
		SDL_WaitEvent(&event);
		if (event.type == FRAME_REFRESH_EVENT){
			AVPacket *pkt = ffmCore.getPacket(PKT_VIDEO);
			if (pkt == NULL){
				break;
			}
		
			if(ffmCore.decodeVideo(pkt, frame)){
				sws_scale(swsCtx, frame->data, frame->linesize, 0, ffmCore.getVideoHeight(), framYUV->data, framYUV->linesize);
				Sdl.render(framYUV);
			}
			av_free_packet(pkt);
		}
	}

	av_frame_free(&frame);
	av_frame_free(&framYUV);
	sws_freeContext(swsCtx);

	return 0;
}

