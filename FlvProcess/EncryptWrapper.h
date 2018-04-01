#pragma once
#include "FlvFormatParser.h"
#include <fstream>
class EncryptWrapper
{
public:
	EncryptWrapper(void);
	~EncryptWrapper(void);

	bool init(int type);
	int beginEncrypt(const char *srcFilePath, const char *destFilePath);
	int begineSave();
	FlvFormatParser *getParser() { return mParser; }
	char *getSrcPath() { return mSrcFilePath;}

	public:
	FlvFormatParser *mParser;
	fstream *mFile;
	const char *mOutPutFilePath;

	unsigned char *pBuf;
	unsigned char *pBak;
	char *mSrcFilePath;
	char *mDestFilePath;
};

