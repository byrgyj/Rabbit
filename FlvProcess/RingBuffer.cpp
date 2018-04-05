#include "stdafx.h"
#include "RingBuffer.h"
#include <fstream>


RingBuffer::RingBuffer(int size) : mTotalBufferSize(size), mReadIndex(0), mWriteIndex(0), mWriteEnded(false), mSavedSize(0), mReadedSize(0){
	if (size > 0){
		mDataBuffer = new char[size];
	}
}
RingBuffer::~RingBuffer() {
	if (mDataBuffer != NULL){
		delete []mDataBuffer;
	}
}

int RingBuffer::writeData(char *data, int size){
	std::unique_lock <std::mutex> lock(mMutex);

	if (mWriteIndex + size <= mTotalBufferSize){
		memcpy(mDataBuffer + mWriteIndex, data, size);
		mWriteIndex += size;

		mSavedSize += size;
		mCondition.notify_all();
		return size;
	} else {

		if (mWriteIndex == mTotalBufferSize){
			mCondition.wait(lock); // buffer full
		}


		if (mReadIndex > 0){
			memcpy(mDataBuffer, mDataBuffer + mReadIndex, mWriteIndex - mReadIndex);
			mWriteIndex = mWriteIndex - mReadIndex;
			mReadIndex = 0;
		}

		int canWrite = size > mTotalBufferSize - mWriteIndex ? mTotalBufferSize - mWriteIndex : size;
		if (canWrite > 0){
			memcpy(mDataBuffer + mWriteIndex, data, canWrite);
			mWriteIndex += canWrite;

			mSavedSize += canWrite;
			mCondition.notify_all();
		}

		return canWrite;

	}

	return 0;
}

int RingBuffer::readData(char *buffer, int size){
	std::unique_lock <std::mutex> lock(mMutex);

	if (mReadIndex == mWriteIndex){
		if (mWriteEnded) {
			return 0;
		}
		else {
			mCondition.wait(lock); 
		}
	}

	if (mReadIndex + size < mWriteIndex){
		memcpy(buffer, mDataBuffer + mReadIndex, size);
		mReadIndex += size;

		mReadedSize += size;
		mCondition.notify_all();
		return size;
	}
	else {
		int canRead = size > mWriteIndex - mReadIndex ? mWriteIndex - mReadIndex : size;
		memcpy(buffer, mDataBuffer + mReadIndex, canRead);

		mReadIndex += canRead;
		mReadedSize += canRead;
		mCondition.notify_all();
		return canRead;
	}

	return 0;
}

void RingBuffer::dumpToFile(){
	std::fstream file("dec_rb.flv", std::ios_base::out | std::ios_base::binary);

	file.write(mDataBuffer, mWriteIndex);

	file.close();

}