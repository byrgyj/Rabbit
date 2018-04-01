
#include "FlvFormatParser.h"
#include <fstream>
class DataBuffer {
public:
	DataBuffer();
	virtual ~DataBuffer();

	int getBufferSize() { return mDataSize; }
	std::string getSrcPath() { return mSrcPath; }

	int init(std::string &destPath);
	int writeTail(unsigned int sz);
	virtual int writeData(char *data, int sz);

public:
	char *mDataBuf;
	char *mDataBak;

	int mDataSize;

	std::string mSrcPath;
	std::string mDestPath;
	fstream *mOutFile;
};

class RingBuffer {
public :
	RingBuffer();
	~RingBuffer();

	int writeData(char *data, int size);
	int readData(char *buffer, int size);

private:
	HANDLE mMutext;
	char *mDataBuffer;
};

class DecryptWrapper : public DataBuffer
{
public:
	DecryptWrapper();
	~DecryptWrapper(void);

	int init();
	int getData(char *buffer, int bufSize);
	virtual int writeData(char *data, int sz);
	FlvFormatParser *getParser() { return mParser; }

private:
	FlvFormatParser *mParser;

	char *mOutputData;
};

