#include "StdAfx.h"
#include "DecryptWrapper.h"
#include <mutex>

DataBuffer::DataBuffer() : mDataSize(2048 * 1024) {
	mDataBuf = new char [mDataSize];
	mDataBak = new char [mDataSize];
}

DataBuffer::~DataBuffer() {
	if (mDataBuf != NULL){
		delete []mDataBuf;
	}

	if (mDataBak != NULL){
		delete []mDataBak;
	}
}

bool DataBuffer::init(const char *srcFile, const char *destFile){
	if (srcFile == NULL) {
		return false;
	}
	mSrcPath = srcFile;

	if (destFile != NULL){
		mDestPath = destFile;
	}
	return 0;
}

int DataBuffer::writeTail(unsigned int sz){
	return 0;
}

int DataBuffer::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		return 0;
	}

	return sz;
}

DecryptWrapper::DecryptWrapper() : DataBuffer(),  mParser(NULL) , mRingBuffer(NULL){
	mOutputData = new char[1024 * 1024 * 8];
}

DecryptWrapper::~DecryptWrapper(void){
	if (mOutputData != NULL){
		delete []mOutputData;
	}

	if (mRingBuffer != NULL){
		delete mRingBuffer;
	}
}

DWORD WINAPI decryptThread(void *param){
	DecryptWrapper *dec = (DecryptWrapper *)param;
	if (dec == NULL){
		return 0;
	}

	int nBufSize = dec->getBufferSize();
	std::string srcFile = dec->getSrcPath();
	int nFlvPos = 0;
	fstream fin;
	fin.open(srcFile.c_str(), ios_base::in | ios_base::binary);
	while (1)
	{
		int nReadNum = 0;
		int nUsedLen = 0;

		fin.read((char *)dec->mDataBuf + nFlvPos, nBufSize - nFlvPos);
		nReadNum = fin.gcount();
		if (nReadNum == 0)
			break;
		nFlvPos += nReadNum;

		dec->getParser()->Parse((unsigned char *)dec->mDataBuf, nFlvPos, nUsedLen);
		if (nFlvPos != nUsedLen)
		{
			memcpy(dec->mDataBak, dec->mDataBuf + nUsedLen, nFlvPos - nUsedLen);
			memcpy(dec->mDataBuf, dec->mDataBak, nFlvPos - nUsedLen);
		}
		nFlvPos -= nUsedLen;
	}
	dec->getParser()->setParseEnd(true);

	return 0;
}

DWORD WINAPI saveDiskThread(void *param){
	DecryptWrapper *dec = (DecryptWrapper *)param;
	if (dec == NULL) {
		return 0;
	}
	FlvFormatParser *parser = dec->getParser();
	if (parser == NULL){
		return 0;
	}

	bool initHeader = false;

	while(true){
		if (!initHeader){
			FlvHeader* header = parser->getFlvHeader();
			if (header != NULL){
				dec->writeData((char *)header->pFlvHeader, header->nHeadSize);
				initHeader = true;
			} else {
				Sleep(100);
				continue;
			}
		} else {
			Tag *cur = parser->getTag();
			if (cur != NULL){
				parser->saveTagToRingBuffer(cur, dec->getRingBuffer());
			} else {
				if (parser->isParserEnd()){
					dec->writeTail(parser->getLastTagSize());
					dec->getRingBuffer()->setEnded(true);
					//dec->getRingBuffer()->dumpToFile();
					break;
				}
				else {
					Sleep(100);
				}
				
			}
		}
	}

	return 0;
}

int DecryptWrapper::init(const char *srcFile, const char *destFile){
	DataBuffer::init(srcFile, destFile);
	mParser = new FlvFormatParser(2);

	mRingBuffer = new RingBuffer(1024 * 1024 * 4);

	CreateThread(NULL, 0, decryptThread, this, 0, NULL);
	CreateThread(NULL, 0, saveDiskThread, this, 0, NULL);
	return 1;
}

int DecryptWrapper::getData(char *buffer, int bufSize){
	if (mParser == NULL || bufSize <= 0 || buffer == NULL){
		return 0;
	}

	int readSz = 0;
	while (readSz == 0)
	{
		readSz = mRingBuffer->readData(buffer, bufSize);
		if (mRingBuffer->isEnded()){
			break;
		}
	}
	
	return readSz;
}

int DecryptWrapper::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		return 0;
	}
	int saveSize = 0;

	while (saveSize < sz)
	{
		int s = mRingBuffer->writeData(data, sz);
		if (s == 0){
			Sleep(100);
		}
		saveSize += s;
	}

	return sz;
}

int DecryptWrapper::writeTail(unsigned int sz){
	unsigned int nn = WriteU32(sz);
	mRingBuffer->writeData((char*)&nn, 4);
	return 0;
}