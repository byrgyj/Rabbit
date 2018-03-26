#pragma once
/*
author: byrgyj@126.com
*/

#include "FlvFormatParser.h"
class VideoResource
{
public:
	VideoResource(const char *file);
	~VideoResource(void);

	int getData(unsigned char *data, int dataSize);
	bool decryptVideo();
private:
	FlvFormatParser *mParser;

	unsigned char *mBuffer;
	int mTotalDataSize;
	int mReadIndex;

	bool mParseFinished;

	std::string mFilePath;
};

