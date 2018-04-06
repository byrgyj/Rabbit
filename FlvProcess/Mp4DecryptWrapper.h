#ifndef MP4_DECRYPT_WRAPPER_H_
#define MP4_DECRYPT_WRAPPER_H_
/*
author:byrgyj@126.com
date:2018-04-06 16:00
*/

#include "RingBuffer.h"
#include "Mp4Parser.h"
#include <thread>
#include "Aes.h"
class Mp4DecryptWrapper
{
public:
	Mp4DecryptWrapper();
	~Mp4DecryptWrapper();

	bool init(const char *srcFile, const char *destFile);
	int getData(char *buffer, int bufSize);
	int writeData(char *data, int sz);


	Mp4Parser *getParser() { return mParser;  }
	RingBuffer *getRingBuffer() { return mRingBuffer; }

private:
	char *readFromFile(int offset, int size);
	int decryptData(char *data, int size);
	void decryptFunction();

private:
	RingBuffer *mRingBuffer;
	Mp4Parser *mParser;
	std::thread *mThreadObj;
	AES *mAes;
	FILE *mInputFile;
	FILE *mOutFile;
};

#endif