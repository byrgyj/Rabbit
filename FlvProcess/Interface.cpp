#include "StdAfx.h"
#include "Interface.h"
#include "FlvFormatParser.h"
#include <fstream>
#include "EncryptWrapper.h"

EncryptWrapper *encWrapper = NULL;
bool init(int type){
	encWrapper = new EncryptWrapper();
	return encWrapper->init(type);
}

int encryptFlvFile(const char *srcFilePath, const char *destFilePath){
	if (encWrapper == NULL || srcFilePath == NULL || destFilePath == NULL){
		return -1;
	}
	if (true){
		encWrapper->beginEncrypt(srcFilePath, destFilePath);
		encWrapper->begineSave();
		return 0;
	} else {
// 		int nBufSize = 2048 * 1024;
// 		int nFlvPos = 0;
// 		unsigned char *pBuf, *pBak;
// 		pBuf = new unsigned char[nBufSize];
// 		pBak = new unsigned char[nBufSize];
// 
// 		fstream fin;
// 		fin.open(srcFilePath, ios_base::in | ios_base::binary);
// 		while (1)
// 		{
// 			int nReadNum = 0;
// 			int nUsedLen = 0;
// 	
// 
// 			fin.read((char *)pBuf + nFlvPos, nBufSize - nFlvPos);
// 			nReadNum = fin.gcount();
// 			if (nReadNum == 0)
// 				break;
// 			nFlvPos += nReadNum;
// 
// 			gFlvParser->Parse(pBuf, nFlvPos, nUsedLen);
// 			if (nFlvPos != nUsedLen)
// 			{
// 				memcpy(pBak, pBuf + nUsedLen, nFlvPos - nUsedLen);
// 				memcpy(pBuf, pBak, nFlvPos - nUsedLen);
// 			}
// 			nFlvPos -= nUsedLen;
// 		}
// 
// 		//dump into flv
// 		gFlvParser->DumpFlv(destFilePath);
// 
// 		delete []pBak;
// 		delete []pBuf;
// 
// 		if (gFlvParser != NULL){
// 			delete gFlvParser;
// 			gFlvParser = NULL;
// 		}
		return 0;
	}
}

int decryptFlvData(const char *srcBuffer, int srcBufferSize, const char *destData, int destDataSize){
	return 0;
}