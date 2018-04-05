#include "StdAfx.h"
#include "EncryptWrapper.h"

EncryptWrapper::EncryptWrapper(void) : mBufferSize(2048 * 1024), mEncThread(NULL), mSaveThread(NULL){
}
EncryptWrapper::~EncryptWrapper(void){
	delete []mDataBak;
	delete []mDataBuf;
}
void encryptRun(void *param){
	EncryptWrapper *enc = (EncryptWrapper*)param;
	if (enc == NULL){
		return;
	}

	int nBufSize = enc->getBufferSize();
	std::string srcFile = enc->getSrcPath();
	int nFlvPos = 0;
	fstream fin;
	fin.open(srcFile.c_str(), ios_base::in | ios_base::binary);
	while (1)
	{
		int nReadNum = 0;
		int nUsedLen = 0;

		fin.read((char *)enc->mDataBuf + nFlvPos, nBufSize - nFlvPos);
		nReadNum = fin.gcount();
		if (nReadNum == 0)
			break;
		nFlvPos += nReadNum;

		enc->getParser()->Parse(enc->mDataBuf, nFlvPos, nUsedLen);
		if (nFlvPos != nUsedLen)
		{
			memcpy(enc->mDataBak, enc->mDataBuf + nUsedLen, nFlvPos - nUsedLen);
			memcpy(enc->mDataBuf, enc->mDataBak, nFlvPos - nUsedLen);
		}
		nFlvPos -= nUsedLen;
	}

	enc->getParser()->setParseEnd(true);
	return;
}

void saveRun(void *param){
	EncryptWrapper *enc = (EncryptWrapper*)param;
	if (enc == NULL) {
		return;
	}
	FlvFormatParser *parser = enc->getParser();
	if (parser == NULL){
		return ;
	}

	bool initHeader = false;
	while (true){
		if (!initHeader){
			FlvHeader* header = parser->getFlvHeader();
			if (header != NULL){
				enc->writeData((char *)header->pFlvHeader, header->nHeadSize);
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
				parser->writeTag(enc->mFile, cur);
			}
			else {
				if (parser->isParserEnd()){
					enc->writeTail(parser->getLastTagSize());
					break;
				}
				else {
					Sleep(10);
				}

			}
		}
	} // end while
}


bool EncryptWrapper::init(const char *srcFile, const char *destFile){
	if (srcFile == NULL || destFile == NULL){
		return false;
	}
	mSrcFilePath = srcFile;
	mOutPutFilePath = destFile;

	mParser = new FlvFormatParser(1);
	return mParser != NULL;
}

int EncryptWrapper::beginEncrypt(){
	if (mParser == NULL){
		return -1;
	}

	mDataBuf = new unsigned char[mBufferSize];
	mDataBak = new unsigned char[mBufferSize];

	mEncThread = new thread(encryptRun, this);
	mEncThread->detach();
	return 0;
}

int EncryptWrapper::begineSave(){
	mFile = new fstream(mOutPutFilePath, ios_base::out | ios_base::binary);

	mSaveThread = new thread(saveRun, this);
	mSaveThread->join();
	return 0;
}

int EncryptWrapper::writeTail(unsigned int sz){
	unsigned int nn = WriteU32(sz);
	mFile->write((char *)&nn, 4);

	mFile->close();
	return 0;
}

int EncryptWrapper::writeData(char *data, int sz){
	if (data == NULL || sz == 0){
		return 0;
	}
	mFile->write(data, sz);

	return sz;
}