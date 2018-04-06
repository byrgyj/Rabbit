#include "stdafx.h"
#include "Mp4DecryptWrapper.h"

Mp4DecryptWrapper::Mp4DecryptWrapper(AES *aes) : mParser(NULL), mRingBuffer(NULL), mThreadObj(NULL), mAes(aes){
}

Mp4DecryptWrapper::~Mp4DecryptWrapper(){
	CLOSE_FILE(mInputFile);
	CLOSE_FILE(mOutFile);

	RELEASE_POINTER(mParser);
	RELEASE_POINTER(mRingBuffer);
	RELEASE_POINTER(mThreadObj);
}

bool Mp4DecryptWrapper::init(const char *srcFile, const char *destFile){
	if (srcFile == NULL){
		return false;
	}

	mRingBuffer = new RingBuffer(1024 * 1024 * 4);
	if (mRingBuffer == NULL){
		return false;
	}

	mParser = new Mp4Parser();
	mParser->init(srcFile);

	mInputFile = fopen(srcFile, "rb");
	if (mInputFile == NULL){
		return false;
	}

	mOutFile = fopen(destFile, "wb");

	mThreadObj = new std::thread(&Mp4DecryptWrapper::decryptFunction, this);
	mThreadObj->detach();

	return true;
}

int Mp4DecryptWrapper::getData(char *buffer, int bufSize){
	return mRingBuffer->readData(buffer, bufSize);
}

int Mp4DecryptWrapper::writeData(char *data, int sz){
	if (data == NULL || sz <= 0 || mRingBuffer == NULL){
		return 0;
	}
	
	int savedSize = 0;
	while (savedSize < sz)
	{
		savedSize += mRingBuffer->writeData(data + savedSize, sz - savedSize);
	}

	return 0;
}

void Mp4DecryptWrapper::decryptFunction(){
	mParser->parser();

	mp4_stss_box &stssBox = mParser->getStssBox();
	stssBox.number_of_entries;

	int srcPosition = 0;
	for (int i = 0; i < stssBox.number_of_entries; i++){
		int sampleOffset = 0;
		int sampelSize = 0;
		uint32_t index = stssBox.sync_sample_table[i] - 1;
		if (mParser->getSampleOffset(index, sampleOffset, sampelSize)){
			char *clearData = readFromFile(srcPosition, sampleOffset - srcPosition);
			if (clearData != NULL){
				writeData(clearData, sampleOffset - srcPosition);
				delete[]clearData;
			}
		
			char *encData = readFromFile(sampleOffset, sampelSize);
			if (encData != NULL){
				int sz = sampelSize - sampelSize % 16;
				decryptData(encData, sz);
				writeData(encData, sampelSize);

				delete[]encData;
			}

			srcPosition = sampleOffset + sampelSize;
		}
	}

	CLOSE_FILE(mInputFile);
	CLOSE_FILE(mOutFile);
	mRingBuffer->setEnded(true);
}

char  *Mp4DecryptWrapper::readFromFile(int offset, int size){
	if (mInputFile == NULL || size <= 0){
		return NULL;
	}

	char *data = new char[size];
	fseek(mInputFile, offset, SEEK_SET);
	fread(data, 1, size, mInputFile);

	return data;
}

int Mp4DecryptWrapper::decryptData(char *data, int size){
	if (mAes == NULL){
		return 0;
	}

	mAes->decryptData(data, size);
	return size;
}