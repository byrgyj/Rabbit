
#include "FlvFormatParser.h"
#include <fstream>
#include <thread>
#include "RingBuffer.h"
class DataBuffer {
public:
	DataBuffer();
	virtual ~DataBuffer();

	int getBufferSize() { return mDataSize; }
	std::string getSrcPath() { return mSrcPath; }
public:
	char *mDataBuf;
	char *mDataBak;

	int mDataSize;

	std::string mSrcPath;
	std::string mDestPath;
	fstream *mOutFile;


};

class DecryptWrapper : public DataBuffer
{
public:
	DecryptWrapper();
	~DecryptWrapper(void);

	bool init(const char *srcFile, const char *destFile);
	int getData(char *buffer, int bufSize);
	virtual int writeData(char *data, int sz);
	FlvFormatParser *getParser() { return mParser; }
	RingBuffer *getRingBuffer() { return mRingBuffer;  }

	int writeTail(unsigned int sz);
private:
	FlvFormatParser *mParser;
	RingBuffer *mRingBuffer;

	thread *mDecThread;
	thread *mSaveThrad;
};

