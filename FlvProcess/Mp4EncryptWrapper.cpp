#include "stdafx.h"
#include "Mp4EncryptWrapper.h"

unsigned char mpKey[] = "gonggonggonggong";
Mp4EncryptWrapper::Mp4EncryptWrapper() : mParser(NULL), mSrcFile(NULL), mOutFile(NULL), mAes(NULL){
}
Mp4EncryptWrapper::~Mp4EncryptWrapper(){
	if (mParser != NULL){
		delete mParser;
	}
	if (mAes != NULL){
		delete mAes;
	}

	if (mOutFile != NULL){
		fclose(mOutFile);
	}

	if (mSrcFile != NULL){
		fclose(mSrcFile);
	}
}

bool Mp4EncryptWrapper::init(const char *srcFile, const char *destFile){
	if (srcFile == NULL || destFile == NULL){
		return false;
	}

	mParser = new Mp4Parser();
	if (mParser == NULL){
		return false;
	}
	mParser->init(srcFile)
		;
	mAes = new AES(mpKey);

	mSrcFile = fopen(srcFile, "rb");
	if (mSrcFile == NULL){
		return false;
	}

	mOutFile = fopen(destFile, "wb");
	if (mOutFile == NULL){
		return false;
	}
	return true;
}

int Mp4EncryptWrapper::beginEncrypt(){
	
	mEncThread = new std::thread(&Mp4EncryptWrapper::encrypThread, this);
	mEncThread->join();

	return 0;
}
void Mp4EncryptWrapper::encrypThread(){

	mParser->parser();

	mp4_stss_box &stssBox = mParser->getStssBox();
	stssBox.number_of_entries;


	int srcPosition = 0;
	for (int i = 0; i < stssBox.number_of_entries; i++){
		int sampleOffset = 0;
		int sampelSize = 0;
		uint32_t index = stssBox.sync_sample_table[i] - 1;
		if (mParser->getSampleOffset(index, sampleOffset, sampelSize)){

			// no encrypted data
			uint8_t *noEncData = getSrcData(srcPosition, sampleOffset - srcPosition);
			fwrite(noEncData, 1, sampleOffset - srcPosition, mOutFile);
			delete[]noEncData;

			// need to encrypted data
			uint8_t *encData = getSrcData(sampleOffset, sampelSize);
			
			int encSize = sampelSize - sampelSize % 16;
			if (encryptData(encData, encSize) != 0){
				fwrite(encData, 1, sampelSize, mOutFile);
			}
			delete[]encData;
			srcPosition = sampelSize + sampleOffset;
		}

		printf("key sample index:%d, sample_offset:%d, sample_size:%d \n", index, sampleOffset, sampelSize);
	}

	fclose(mSrcFile);
	fclose(mOutFile);

	mSrcFile = NULL;
	mOutFile = NULL;

}
uint8_t *Mp4EncryptWrapper::getSrcData(int offset, int size){
	uint8_t *buffer = new uint8_t[size];

	fseek(mSrcFile, offset, SEEK_SET);
	fread(buffer, 1, size, mSrcFile);

	return buffer;
}
int Mp4EncryptWrapper::encryptData(uint8_t *data, int dataSize){
	if (mAes != NULL){
		mAes->encryptData(data, dataSize);

		return 1;
	}

	return 0;
}
