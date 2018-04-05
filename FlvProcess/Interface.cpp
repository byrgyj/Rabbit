#include "StdAfx.h"
#include "Interface.h"
#include "FlvFormatParser.h"
#include <fstream>
#include "EncryptWrapper.h"
#include "DecryptWrapper.h"

EncryptWrapper *encWrapper = NULL;
DecryptWrapper *decWrapper = NULL;
bool init(int type, const char *srcFile, const char *destFile){
	if (type == 1){
		encWrapper = new EncryptWrapper();
		return encWrapper->init(srcFile, destFile);
	} else if (type == 2) {
		decWrapper = new DecryptWrapper();
		return decWrapper->init(srcFile, destFile);
	} else {
		return 0;
	}
}

int encryptFlvFile(){
	if (encWrapper == NULL){
		return -1;
	}
	encWrapper->beginEncrypt();
	encWrapper->begineSave();
	return 0;

}

int decryptFlvData(const char *srcBuffer, int srcBufferSize, const char *destData, int destDataSize){

	if (decWrapper == NULL){
		return 0;
	}
	
	return 0;
}

int comsumeFlvData(char *buffer, int dataSize){
	if (decWrapper == NULL){
		return 0;
	}

	return decWrapper->getData(buffer, dataSize);
}