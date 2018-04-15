
#ifndef AUDIO_H_
#define AUDIO_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include <SDL2/SDL.h>
#include "player.h"

int prepare_audio(PlayerState *ps);
int play_audio(PlayerState *ps);
void audio_callback(void *userdata, uint8_t *stream, int len);
int audio_decode_frame(PlayerState *ps);

#ifdef __cplusplus
}
#endif
#endif

