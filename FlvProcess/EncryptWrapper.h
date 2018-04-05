#pragma once
#include "FlvFormatParser.h"
#include <fstream>
#include <thread>
class EncryptWrapper
{
public:
	EncryptWrapper(void);
	~EncryptWrapper(void);

	bool init(const char *srcFile, const char *destFile);
	int beginEncrypt();
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

	thread *mEncThread;
	thread *mSaveThread;

	std::string mSrcFilePath;
	std::string mDestFilePath;

	int mBufferSize;
};

