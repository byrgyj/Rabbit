#include "StdAfx.h"
#include "VideoResource.h"
#include <fstream>


VideoResource::VideoResource(const char *file):mParser(NULL), mParseFinished(false), mTotalDataSize(0), mReadIndex(0), mFilePath(file), mBuffer(NULL){
	mParser = new FlvFormatParser(2);
}

VideoResource::~VideoResource(){
	if (mBuffer != NULL){
		delete []mBuffer;
	}
}

int VideoResource::getData(unsigned char *data, int dataSize){
	if (!mParseFinished){
		decryptVideo();
	}

	if (mReadIndex == mTotalDataSize){
		return 0;
	} else {
		int requestSize = dataSize < mTotalDataSize - mReadIndex ? dataSize : mTotalDataSize - mReadIndex;
		memcpy(data, mBuffer + mReadIndex, requestSize);

		mReadIndex += requestSize;
		return requestSize;
	}
}

bool VideoResource::decryptVideo(){
	int nBufSize = 2048 * 1024;
	int nFlvPos = 0;
	unsigned char *pBuf, *pBak;
	pBuf = new unsigned char[nBufSize];
	pBak = new unsigned char[nBufSize];

	std::fstream encryptedFile;
	encryptedFile.open(mFilePath.c_str(), ios_base::in | ios_base::binary);

	while (1)
	{
		int nReadNum = 0;
		int nUsedLen = 0;
		encryptedFile.read((char *)pBuf + nFlvPos, nBufSize - nFlvPos);
		nReadNum = encryptedFile.gcount();
		if (nReadNum == 0)
			break;
		nFlvPos += nReadNum;

		mParser->Parse(pBuf, nFlvPos, nUsedLen);
		if (nFlvPos != nUsedLen)
		{
			memcpy(pBak, pBuf + nUsedLen, nFlvPos - nUsedLen);
			memcpy(pBuf, pBak, nFlvPos - nUsedLen);
		}
		nFlvPos -= nUsedLen;
	}

	int bufferSize = 1024 * 1024 * 8;
	if (mBuffer == NULL){
		mBuffer = new unsigned char [bufferSize];
	}

	mTotalDataSize = mParser->DumpFlvToMemoryBuffer(mBuffer, bufferSize);
	mParseFinished = true;

	return true;
}