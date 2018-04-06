#ifndef MP4_ENCRYPT_WRAPPER_H_
#define MP4_ENCRYPT_WRAPPER_H_
/*
author:byrgyj@126.com
date:2018-04-05 23:07
*/
#include <string>
#include <fstream>
#include <thread>
#include "Mp4Parser.h"
#include "Aes.h"
class Mp4EncryptWrapper
{
public:
	Mp4EncryptWrapper(AES *aes);
	~Mp4EncryptWrapper();

	bool init(const char *srcFile, const char *destFile);
	int beginEncrypt();
	void encrypThread();
	
private:

	uint8_t *getSrcData(int offset, int size);
	int encryptData(uint8_t *data, int dataSize);

	//std::fstream *mSrcFile;
	//std::fstream *mDestFile;
	FILE *mSrcFile;
	FILE *mOutFile;

	Mp4Parser *mParser;
	AES *mAes;
	std::thread *mEncThread;


	std::string mSrcPath;
	std::string mDestPath;
};

#endif

