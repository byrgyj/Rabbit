#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_
#include <mutex>
#include <condition_variable>

class RingBuffer {
public :
	RingBuffer(int size);
	~RingBuffer();

	int writeData(char *data, int size);
	int readData(char *buffer, int size);

	void setEnded(bool flag) { mWriteEnded = flag; }
	bool isEnded() { return mWriteEnded;}

	void dumpToFile();
private:
	std::mutex mMutex;
	std::condition_variable mCondition;

	char *mDataBuffer;
	int mTotalBufferSize;
	int mWriteIndex;
	int mReadIndex;

	bool mWriteEnded;


	int mSavedSize;
	int mReadedSize;
};
#endif