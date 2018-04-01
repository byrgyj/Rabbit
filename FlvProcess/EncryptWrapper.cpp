#include "StdAfx.h"
#include "EncryptWrapper.h"
#include <fstream>

EncryptWrapper::EncryptWrapper(void)
{
}


EncryptWrapper::~EncryptWrapper(void)
{
	delete []pBak;
	delete []pBuf;
}

DWORD WINAPI encryptThead(void *param){
	EncryptWrapper *enc = (EncryptWrapper*)param;
	if (enc == NULL){
		return 0;
	}

	int nBufSize = 2048 * 1024;
	int nFlvPos = 0;
	fstream fin;
	fin.open("ffd5.f4v", ios_base::in | ios_base::binary);
	while (1)
	{
		int nReadNum = 0;
		int nUsedLen = 0;

		fin.read((char *)enc->pBuf + nFlvPos, nBufSize - nFlvPos);
		nReadNum = fin.gcount();
		if (nReadNum == 0)
			break;
		nFlvPos += nReadNum;

		enc->mParser->Parse(enc->pBuf, nFlvPos, nUsedLen);
		if (nFlvPos != nUsedLen)
		{
			memcpy(enc->pBak, enc->pBuf + nUsedLen, nFlvPos - nUsedLen);
			memcpy(enc->pBuf, enc->pBak, nFlvPos - nUsedLen);
		}
		nFlvPos -= nUsedLen;
	}
	enc->mParser->mParserEnd = true;
	return 0;

}

DWORD WINAPI saveThread(void *param){
	EncryptWrapper *enc = (EncryptWrapper*)param;
	FlvFormatParser *parser = enc->getParser();
	bool initHeader = false;
	

// 	while (true){
// 		Sleep(1000);
// 
// 		if (parser->isParserEnd()){
// 			std::string outFile = "enc.flv";
// 			parser->DumpFlv(outFile);
// 			break;
// 		}
// 		
// 	}
	while(true){
		FlvHeader* header = parser->getFlvHeader();
		if (!initHeader){
			if (header != NULL){
				enc->mFile->write((char *)header->pFlvHeader, header->nHeadSize);
				initHeader = true;
			} else {
				Sleep(100);
				continue;
			}
		} else {
			Tag *cur = parser->getTag();
			if (cur != NULL){
				parser->writeTag(enc->mFile, cur);
			} else {
				parser->writeTail(enc->mFile);
				break;
			}
		}
	}


	return 0;
}

bool EncryptWrapper::init(int type){
	mParser = new FlvFormatParser(type);
	return true;
}

int EncryptWrapper::beginEncrypt(const char *srcFilePath, const char *destFilePath){
	if (mParser == NULL || srcFilePath == NULL || destFilePath == NULL){
		return -1;
	}

	//mSrcFilePath = srcFilePath;
	//mDestFilePath = destFilePath;

	mOutPutFilePath = destFilePath;

	int nBufSize = 2048 * 1024;
	int nFlvPos = 0;
	//unsigned char *pBuf, *pBak;
	pBuf = new unsigned char[nBufSize];
	pBak = new unsigned char[nBufSize];

	CreateThread(NULL, 0, encryptThead, this, 0, NULL);

// 	while (1)
// 	{
// 		int nReadNum = 0;
// 		int nUsedLen = 0;
// 		fstream fin;
// 		fin.open(srcFilePath, ios_base::in | ios_base::binary);
// 
// 		fin.read((char *)pBuf + nFlvPos, nBufSize - nFlvPos);
// 		nReadNum = fin.gcount();
// 		if (nReadNum == 0)
// 			break;
// 		nFlvPos += nReadNum;
// 
// 		mParser->Parse(pBuf, nFlvPos, nUsedLen);
// 		if (nFlvPos != nUsedLen)
// 		{
// 			memcpy(pBak, pBuf + nUsedLen, nFlvPos - nUsedLen);
// 			memcpy(pBuf, pBak, nFlvPos - nUsedLen);
// 		}
// 		nFlvPos -= nUsedLen;
// 	}

	//dump into flv
// 	gFlvParser->DumpFlv(destFilePath);
// 
//  	delete []pBak;
//  	delete []pBuf;
// 
// 	if (gFlvParser != NULL){
// 		delete gFlvParser;
// 		gFlvParser = NULL;
// 	}

	return 0;
}

int EncryptWrapper::begineSave(){
	mFile = new fstream(mOutPutFilePath, ios_base::out | ios_base::binary);

	CreateThread(NULL, 0, saveThread, this, 0, NULL);

	return 0;
}