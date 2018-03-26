#include "StdAfx.h"
#include "FFmpegCore.h"

int readPacket(void *opaque, uint8_t *buf, int buf_size){
	FILE *file = (FILE*)opaque;
	if (file == NULL){
		return 0;
	}

	if (!feof(file)){
		int sz = fread(buf, 1, buf_size, file);
		return sz;
	} else {
		return 0;
	}

}

FFmpegCore::FFmpegCore(char *file) : mFilePath(file), mVideoStreamIndex(0), mAudioStreamIndex(0), mInputFmtCtx(NULL), mVideoCdcCtx(NULL), mIOCtx(NULL), mDataFile(NULL){
	av_register_all();
}


FFmpegCore::~FFmpegCore(){
	if (mVideoCdcCtx != NULL){
		avcodec_close(mVideoCdcCtx);
	}
	
	if (mInputFmtCtx != NULL){
		avformat_close_input(&mInputFmtCtx);
	}

	if (mDataFile != NULL){
		fclose(mDataFile);
	}
}

bool FFmpegCore::initFFmpeg(){
	mDataFile = fopen(mFilePath.c_str(), "rb");
	if (mDataFile == NULL){
		return false;
	}

	unsigned char *buffer = (unsigned char*)av_malloc(1024);
	mIOCtx = avio_alloc_context(buffer, 1024, 0, (void*)mDataFile, readPacket, NULL, NULL);
	if (mIOCtx == NULL){
		return false;
	}

	mInputFmtCtx = avformat_alloc_context();
	mInputFmtCtx->pb = mIOCtx;

	int ret = avformat_open_input(&mInputFmtCtx, mFilePath.c_str(), NULL, NULL);
	if (ret < 0){
		return -1;
	}

	ret = avformat_find_stream_info(mInputFmtCtx, NULL);
	if (ret < 0){
		return -1;
	}

	for (int i = 0; i < mInputFmtCtx->nb_streams; i++){
		if (mInputFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO){
			mVideoStreamIndex = i;
			mVideoCdcCtx = mInputFmtCtx->streams[i]->codec;
		} else if (mInputFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO){
			mAudioStreamIndex = i;
		}
	}

	AVCodec *decoder = avcodec_find_decoder(mVideoCdcCtx->codec_id);
	if (decoder == NULL){
		return -1;
	}

	ret = avcodec_open2(mVideoCdcCtx, decoder, NULL);
	if (ret < 0){
		return -1;
	}


	return true;
}

AVPacket *FFmpegCore::getPacket(int pktType){
	AVPacket *pkt = (AVPacket *)av_malloc(sizeof(AVPacket));

	while(true){
		int ret = av_read_frame(mInputFmtCtx, pkt);
		if (ret < 0 ){
			return NULL;
		}

		if (pktType == PKT_VIDEO){
			if (pkt->stream_index != mVideoStreamIndex){
				av_free_packet(pkt);
				continue;
			} else {
				break;
			}
		} else if (pktType == PKT_AUDIO){
			if (pkt->stream_index != mAudioStreamIndex){
				av_free_packet(pkt);
				continue;
			} else {
				break;
			}
		} else {
			//
		}
	}
	return pkt;
}
bool FFmpegCore::decodeVideo(AVPacket *pkt, AVFrame *fram){
	int got_picture = 0;
	int ret = avcodec_decode_video2(mVideoCdcCtx, fram, &got_picture, pkt);
	if (got_picture){
		return true;
	} else {
		return false;
	}
}