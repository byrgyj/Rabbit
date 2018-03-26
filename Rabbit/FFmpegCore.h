#pragma once
/*
author: byrgyj@126.com
*/
#ifdef _WIN32
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

#endif

#include <string>
enum PacketType {PKT_VIDEO, PKT_AUDIO, PKT_ALL };
class FFmpegCore
{
public:
	FFmpegCore(char *file);
	~FFmpegCore();
	bool initFFmpeg();

	AVPacket *getPacket(int pktType);
	int getVideoWidth() { return mVideoCdcCtx->width; }
	int getVideoHeight() { return mVideoCdcCtx->height; }
	AVPixelFormat getPixFormat() { return mVideoCdcCtx->pix_fmt; }
	bool decodeVideo(AVPacket *pkt, AVFrame *fram);
private:
	std::string mFilePath;
	AVFormatContext *mInputFmtCtx;
	AVCodecContext *mVideoCdcCtx;
	AVIOContext *mIOCtx;
	FILE *mDataFile;

	int mVideoStreamIndex;
	int mAudioStreamIndex;
};

