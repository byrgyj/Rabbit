#include "StdAfx.h"
#include "SdlContainer.h"


int frameRefreshThread(void *opque){
	while(true){
		SDL_Event event;
		event.type = FRAME_REFRESH_EVENT;
		SDL_PushEvent(&event);

		SDL_Delay(40);
	}
	return 0;
}

SdlContainer::SdlContainer(int width, int height) : mVideoWidth(width), mVideoHeight(height) {
}
SdlContainer::~SdlContainer(){
}

bool SdlContainer::initSDL() {
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		return false;
	}

	mWindow = SDL_CreateWindow("player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mVideoWidth, mVideoHeight, SDL_WINDOW_OPENGL);
	if (mWindow == NULL){
		return false;
	}
	mRender = SDL_CreateRenderer(mWindow, -1, 0);
	if (mRender == NULL){
		return false;
	}

	mTexture = SDL_CreateTexture(mRender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, mVideoWidth, mVideoHeight);
	if (mTexture == NULL){
		return false;
	}

	SDL_CreateThread(frameRefreshThread, NULL, NULL);
	return true;
}

void SdlContainer::render(AVFrame *framYUV){
	if (framYUV == NULL){
		return;
	}

	//SDL---------------------------
	SDL_UpdateTexture(mTexture, NULL, framYUV->data[0], framYUV->linesize[0] );  
	SDL_RenderClear( mRender );  
	//SDL_RenderCopy( sdlRenderer, sdlTexture, &sdlRect, &sdlRect );  
	SDL_RenderCopy( mRender, mTexture, NULL, NULL);  
	SDL_RenderPresent( mRender );  
	//SDL End-----------------------
}