#include "StdAfx.h"
#include "DecryptWrapper.h"

DataBuffer::DataBuffer() : mDataSize(2048 * 1024){
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

int DataBuffer::init(std::string &destPath){
	mOutFile = new fstream(destPath.c_str(), ios_base::out | ios_base::binary);

	mSrcPath = "enc.flv";
	return 0;
}

int DataBuffer::writeTail(unsigned int sz){
	unsigned int nn = WriteU32(sz);
	mOutFile->write((char *)&nn, 4);

	mOutFile->close();
	return 0;
}

int DataBuffer::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		return 0;
	}
	mOutFile->write(data, sz);

	return sz;
}

RingBuffer::RingBuffer() {
	mMutext = CreateMutex(NULL, true, NULL);
}
RingBuffer::~RingBuffer() {

}

int RingBuffer::writeData(char *data, int size){
	return 0;
}

int RingBuffer::readData(char *buffer, int size){
	return 0;
}

DecryptWrapper::DecryptWrapper() : DataBuffer(),  mParser(NULL){
	mOutputData = new char[1024 * 1024 * 8];
}

DecryptWrapper::~DecryptWrapper(void){
	if (mOutputData != NULL){
		delete []mOutputData;
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
				parser->writeTag(dec->mOutFile, cur);
			} else {
				dec->writeTail(parser->getLastTagSize());
				break;
			}
		}
	}

	return 0;
}

int DecryptWrapper::init(){
	std::string dest = "dec.flv";
	DataBuffer::init(dest);
	mParser = new FlvFormatParser(2);
	CreateThread(NULL, 0, decryptThread, this, 0, NULL);
	CreateThread(NULL, 0, saveDiskThread, this, 0, NULL);
	return 1;
}

int DecryptWrapper::getData(char *buffer, int bufSize){
	if (mParser == NULL || bufSize <= 0 || buffer == NULL){
		return 0;
	}

	return 0;
}

int DecryptWrapper::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		return 0;
	}
	mOutFile->write(data, sz);

	return sz;
}
