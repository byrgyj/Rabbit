#include "StdAfx.h"
#include "Interface.h"
#include "FlvFormatParser.h"
#include "EncryptWrapper.h"
#include "DecryptWrapper.h"
#include "Mp4Parser.h"
#include "Mp4EncryptWrapper.h"
EncryptWrapper *encWrapper = NULL;
DecryptWrapper *decWrapper = NULL;

Mp4EncryptWrapper *mp4EncWrapper = NULL;
int gType = 0;
bool init(int type, const char *srcFile, const char *destFile){
	gType = type;
	if (type == Oper_Encrypt){
		encWrapper = new EncryptWrapper();
		return encWrapper->init(srcFile, destFile);
	} else if (type == Oper_Decrypt) {
		decWrapper = new DecryptWrapper();
		return decWrapper->init(srcFile, destFile);
	} else if (type == 3){
		mp4EncWrapper = new Mp4EncryptWrapper();
		return mp4EncWrapper->init(srcFile, destFile);
	}

	return 0;
}

int encryptFlvFile(){
	if (gType == Oper_Encrypt){
		if (encWrapper == NULL){
			return -1;
		}
		encWrapper->beginEncrypt();
		encWrapper->begineSave();
		return 0;
	}
	else if (gType == 3){
		mp4EncWrapper->beginEncrypt();
		return 0;
	}
	
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