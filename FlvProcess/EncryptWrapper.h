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
	int getBufferSize() { return mBufferSize; }
	int writeTail(unsigned int sz);
	int writeData(char *data, int sz);

	FlvFormatParser *getParser() { return mParser; }
	std::string getSrcPath() { return mSrcFilePath;}

public:
	FlvFormatParser *mParser;
	fstream *mFile;
	const char *mOutPutFilePath;

	unsigned char *mDataBuf;
	unsigned char *mDataBak;
	std::string mSrcFilePath;
	std::string mDestFilePath;

	int mBufferSize;
};

