#include "StdAfx.h"
#include "Mp4DataBuffer.h"


Mp4DataBuffer::Mp4DataBuffer(uint32_t size) : mData(NULL), mCurrentIndex(0), mDataLength(size)
{
	if (size > 0)
		mData = new uint8_t[size];
}


Mp4DataBuffer::~Mp4DataBuffer(void)
{
	if(mData != NULL)
		delete []mData;
}

uint32_t Mp4DataBuffer::readData(void *buf, uint32_t size)
{
	if (buf == NULL || size <= 0)
		return 0;

	size = (size > mDataLength - mCurrentIndex) ? mDataLength - mCurrentIndex : size;

	memcpy(buf, mData+mCurrentIndex, size);

	mCurrentIndex += size;

	return size;
}
void Mp4DataBuffer::appendData(uint8_t *buf, uint32_t size)
{
	if (mData==NULL || size == 0 || buf == NULL)
		return;

	size = mDataLength > size ? size : mDataLength;
	memcpy(mData, buf, size);
	mDataLength += size;
}
uint32_t Mp4DataBuffer::getCurrentIndex()
{
	return mCurrentIndex;
}
void Mp4DataBuffer::setCurrentIndex(uint32_t index)
{
	mCurrentIndex = index;
}

bool Mp4DataBuffer::isEnd()
{
	 return mCurrentIndex == mDataLength; 
}
