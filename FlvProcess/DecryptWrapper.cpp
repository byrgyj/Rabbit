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

DecryptWrapper::DecryptWrapper() : DataBuffer(),  mParser(NULL) , mRingBuffer(NULL), mSaveThrad(NULL), mDecThread(NULL){

}

DecryptWrapper::~DecryptWrapper(void){
	if (mRingBuffer != NULL){
		delete mRingBuffer;
	}
}

void decryptThread(void *param){
	DecryptWrapper *dec = (DecryptWrapper *)param;
	if (dec == NULL){
		return;
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

	printf("decryptThread Finished \n");
}

void saveDiskThread(void *param){
	DecryptWrapper *dec = (DecryptWrapper *)param;
	if (dec == NULL) {
		return;
	}
	FlvFormatParser *parser = dec->getParser();
	if (parser == NULL){
		return;
	}

	bool initHeader = false;

	while (true){
		if (!initHeader){
			FlvHeader* header = parser->getFlvHeader();
			if (header != NULL){
				dec->writeData((char *)header->pFlvHeader, header->nHeadSize);
				initHeader = true;
			}
			else {
				Sleep(10);
				continue;
			}
		}
		else {
			Tag *cur = parser->getTag();
			if (cur != NULL){
				parser->saveTagToRingBuffer(cur, dec->getRingBuffer());
			}
			else {
				if (parser->isParserEnd()){
					dec->writeTail(parser->getLastTagSize());
					dec->getRingBuffer()->setEnded(true);
					printf("save all data \n");
					break;
				}
				else {
					Sleep(10);
				}

			}
		}
	}

	printf("saveDiskThread Finished \n");
}

bool DecryptWrapper::init(const char *srcFile, const char *destFile){
	if (srcFile == NULL){
		return false;
	}
	
	mSrcPath = srcFile;
	if (destFile != NULL){
		mDestPath = destFile;
	}

	mParser = new FlvFormatParser(2);
	mRingBuffer = new RingBuffer(1024 * 1024 * 2);
	mDecThread = new thread(decryptThread, this);
	mDecThread->detach();

	mSaveThrad = new thread(saveDiskThread, this);
	mSaveThrad->detach();

	return true;
}

int DecryptWrapper::getData(char *buffer, int bufSize){
	if (mParser == NULL || bufSize <= 0 || buffer == NULL){
		printf("getData[%d] \n", 0);
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
	
	printf("getData[%d] \n", readSz);
	return readSz;
}

int DecryptWrapper::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		printf("writeData:%s \n", 0);
		return 0;
	}
	int saveSize = 0;

	while (saveSize < sz)
	{
		int s = mRingBuffer->writeData(data, sz);
		if (s == 0){
			Sleep(10);
		}
		saveSize += s;
	}

	printf("writeData:%d \n", saveSize);
	return saveSize;
}

int DecryptWrapper::writeTail(unsigned int sz){
	unsigned int nn = WriteU32(sz);
	mRingBuffer->writeData((char*)&nn, 4);
	return 0;
}