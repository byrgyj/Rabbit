#pragma once
#ifdef _WIN32
extern "C"{
#include <SDL2/SDL.h>
#include <libavformat/avformat.h>
};
#endif
#include <SDL2/SDL.h>

#define FRAME_REFRESH_EVENT (SDL_USEREVENT + 1)

class SdlContainer
{
public:
	SdlContainer(int width, int height);
	~SdlContainer();

	bool initSDL();

	void render(AVFrame *framYUV);
private:

	SDL_Window *mWindow;
	SDL_Texture *mTexture;
	SDL_Renderer *mRender;

	int mVideoWidth;
	int mVideoHeight;
};

