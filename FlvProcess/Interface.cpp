#include "StdAfx.h"
#include "Interface.h"
#include "FlvFormatParser.h"
#include "EncryptWrapper.h"
#include "DecryptWrapper.h"
#include "Mp4Parser.h"
#include "Mp4EncryptWrapper.h"
#include "Mp4DecryptWrapper.h"
EncryptWrapper *encWrapper = NULL;
DecryptWrapper *decWrapper = NULL;

Mp4EncryptWrapper *mp4EncWrapper = NULL;
Mp4DecryptWrapper *mp4DecWrapper = NULL;
AES				  *gAes = NULL;
int gType = 0;

unsigned char aesKey[] = "gonggonggonggong";
bool init(int type, const char *srcFile, const char *destFile){
	gType = type;
	if (gAes == NULL){
		gAes = new AES(aesKey);
	}

	if (type == Oper_Encrypt_Flv){
		encWrapper = new EncryptWrapper();
		return encWrapper->init(srcFile, destFile);
	} else if (type == Oper_Decrypt_Flv) {
		decWrapper = new DecryptWrapper();
		return decWrapper->init(srcFile, destFile);
	}
	else if (type == Oper_Encrypt_Mp4){
		mp4EncWrapper = new Mp4EncryptWrapper(gAes);
		return mp4EncWrapper->init(srcFile, destFile);
	}
	else if (type == Oper_Decrypt_Mp4) {
		mp4DecWrapper = new Mp4DecryptWrapper(gAes);
		return mp4DecWrapper->init(srcFile, destFile);
	}

	return 0;
}

int encryptFlvFile(){
	if (gType == Oper_Encrypt_Flv){
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

int consumeMp4Data(char *buffer, int dataSize){
	if (mp4DecWrapper == NULL){
		return 0;
	}

	return mp4DecWrapper->getData(buffer, dataSize);
}