

#ifndef VIDEO_H_
#define VIDEO_H_
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
#include "packet_queue.h"
int prepare_video(PlayerState *ps);
int play_video(PlayerState *ps);
int decode_and_show(void *arg);
int refresh_fun(void *arg);
double get_audio_clock(PlayerState *ps);
double get_frame_pts(PlayerState *ps, AVFrame *pframe);
double get_delay(PlayerState *ps);

#ifdef __cplusplus
}
#endif
#endif

