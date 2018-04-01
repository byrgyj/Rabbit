#include "StdAfx.h"
#include "Interface.h"
#include "FlvFormatParser.h"
#include <fstream>
#include "EncryptWrapper.h"
#include "DecryptWrapper.h"

EncryptWrapper *encWrapper = NULL;
DecryptWrapper *decWrapper = NULL;
bool init(int type){
	if (type == 1){
		encWrapper = new EncryptWrapper();
		return encWrapper->init(type);
	} else if (type == 2) {
		decWrapper = new DecryptWrapper();
		return decWrapper->init();
	} else {
		return 0;
	}
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
		return 0;
	}
}

int decryptFlvData(const char *srcBuffer, int srcBufferSize, const char *destData, int destDataSize){

	if (decWrapper == NULL){
		return 0;
	}
	
	return 0;
}