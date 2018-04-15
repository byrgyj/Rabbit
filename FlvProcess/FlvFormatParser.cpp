#include "stdafx.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include "FlvFormatParser.h"
#include "Aes.h"

using namespace std;

unsigned char key[] = "gonggonggonggong";
AES aes(key);

#define CheckBuffer(x) { if ((nBufSize-nOffset)<(x)) { nUsedLen = nOffset; return 0;} }

int FlvFormatParser::CAudioTag::_aacProfile;
int FlvFormatParser::CAudioTag::_sampleRateIndex;
int FlvFormatParser::CAudioTag::_channelConfig;

static const unsigned int nH264StartCode = 0x01000000;



FlvFormatParser::FlvFormatParser(int operation) : mTagIndex(0), mOperation(operation), mParserEnd(false), nLastTagSize(0)
{
    _pFlvHeader = NULL;
	mAes = new AES(key);

	mMutex = CreateMutex(NULL, false, NULL);
	mEvent = CreateEvent(NULL, true, true, NULL);
}

FlvFormatParser::~FlvFormatParser()
{
// 	for (int i = 0; i < _vpTag.size(); i++)
// 	{
// 		DestroyTag(_vpTag[i]);
// 		delete _vpTag[i];
// 	}

	for (list<Tag *>::iterator it = _vpTag.begin(); it != _vpTag.end(); it++)
	{
		Tag *cur = *it;
		DestroyTag(cur);
		delete cur;
	}
}

int FlvFormatParser::Parse(unsigned char *pBuf, int nBufSize, int &nUsedLen)
{
	int nOffset = 0;
	mParserEnd = false;
	if (_pFlvHeader == 0)
	{
		CheckBuffer(9);
		_pFlvHeader = CreateFlvHeader(pBuf+nOffset);
		nOffset += _pFlvHeader->nHeadSize;
	}

	while (1)
	{
		CheckBuffer(15);
		int nPrevSize = ShowU32(pBuf + nOffset);
		nOffset += 4;

		Tag *pTag = CreateTag(pBuf + nOffset, nBufSize-nOffset);
		if (pTag == NULL)
		{
			nOffset -= 4;
			break;
		}
		pTag->_header.nByteOffset = nOffset;
		nOffset += (11 + pTag->_header.nDataSize);

		mTagIndex++;
		WaitForSingleObject(mEvent, INFINITE);
		_vpTag.push_back(pTag);
		SetEvent(mEvent);
	}

	nUsedLen = nOffset;
	return 0;
}

int FlvFormatParser::PrintInfo()
{
	Stat();

// 	cout << "vnum: " << _sStat.nVideoNum << " , anum: " << _sStat.nAudioNum << " , mnum: " << _sStat.nMetaNum << endl;
// 	cout << "maxTimeStamp: " << _sStat.nMaxTimeStamp << " ,nLengthSize: " << _sStat.nLengthSize << endl;
// 	cout << "Vjj SEI num: " << _vjj->_vVjjSEI.size() << endl;
// 	for (int i = 0; i < _vjj->_vVjjSEI.size(); i++)
// 		cout << "SEI time : " << _vjj->_vVjjSEI[i].nTimeStamp << endl;
	return 1;
}

int FlvFormatParser::DumpH264(const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out|ios_base::binary);

// 	vector<Tag *>::iterator it_tag;
// 	for (it_tag = _vpTag.begin(); it_tag != _vpTag.end(); it_tag++)
// 	{
// 		if ((*it_tag)->_header.nType != 0x09)
// 			continue;
// 
// 		f.write((char *)(*it_tag)->_pMedia, (*it_tag)->_nMediaLen);
// 	}
	f.close();

	return 1;
}

int FlvFormatParser::DumpAAC(const std::string &path)
{
	fstream f;
	f.open(path.c_str(), ios_base::out | ios_base::binary);

	vector<Tag *>::iterator it_tag;
// 	for (it_tag = _vpTag.begin(); it_tag != _vpTag.end(); it_tag++)
// 	{
// 		if ((*it_tag)->_header.nType != 0x08)
// 			continue;
// 
// 		CAudioTag *pAudioTag = (CAudioTag *)(*it_tag);
// 		if (pAudioTag->_nSoundFormat != 10)
// 			continue;
// 
// 		if (pAudioTag->_nMediaLen!=0)
// 			f.write((char *)(*it_tag)->_pMedia, (*it_tag)->_nMediaLen);
// 	}
	f.close();

	return 1;
}

int FlvFormatParser::DumpFlv(const std::string &path)
{
    fstream f;
    f.open(path.c_str(), ios_base::out | ios_base::binary);

    // write flv-header
    f.write((char *)_pFlvHeader->pFlvHeader, _pFlvHeader->nHeadSize);
    unsigned int nLastTagSize = 0;


    // write flv-tag
    list<Tag *>::iterator it_tag;
    for (it_tag = _vpTag.begin(); it_tag != _vpTag.end(); it_tag++)
    {
        unsigned int nn = WriteU32(nLastTagSize);
        f.write((char *)&nn, 4);

		unsigned char test[3] = { 0 };
		ConvertToHex(test, nLastTagSize);
		
        //check duplicate start code
        if ((*it_tag)->_header.nType == 0x09 && *((*it_tag)->_pTagData + 1) == 0x01) {
			int keyFrame = (((*it_tag)->_pTagData[0]) >> 4) & 0xF;		
			CVideoTag *vTag = (CVideoTag*)*it_tag;
			if (keyFrame == 1){
				if (mOperation == Oper_Encrypt_Flv){
					EncryptData(vTag->_pTagData, vTag->_header.nDataSize);

				} else if (mOperation == Oper_Decrypt_Flv){
					unsigned char *decData = NULL;
					int decSize = 0;
					DecryptData(vTag->_pTagData, vTag->_header.nDataSize);

				}
			}
			
            bool duplicate = false;
            unsigned char *pStartCode = (*it_tag)->_pTagData + 5 + _nNalUnitLength;
            //printf("tagsize=%d\n",(*it_tag)->_header.nDataSize);
            unsigned nalu_len = 0;
            unsigned char *p_nalu_len=(unsigned char *)&nalu_len;
            switch (_nNalUnitLength) {
            case 4:
                nalu_len = ShowU32((*it_tag)->_pTagData + 5);
                break;
            case 3:
                nalu_len = ShowU24((*it_tag)->_pTagData + 5);
                break;
            case 2:
                nalu_len = ShowU16((*it_tag)->_pTagData + 5);
                break;
            default:
                nalu_len = ShowU8((*it_tag)->_pTagData + 5);
                break;
            }
            /*
            printf("nalu_len=%u\n",nalu_len);
            printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",(*it_tag)->_pTagData[5],(*it_tag)->_pTagData[6],
                    (*it_tag)->_pTagData[7],(*it_tag)->_pTagData[8],(*it_tag)->_pTagData[9],
                    (*it_tag)->_pTagData[10],(*it_tag)->_pTagData[11],(*it_tag)->_pTagData[12],
                    (*it_tag)->_pTagData[13]);
            */

            unsigned char *pStartCodeRecord = pStartCode;
            int i;
            for (i = 0; i < (*it_tag)->_header.nDataSize - 5 - _nNalUnitLength - 4; ++i) {
                if (pStartCode[i] == 0x00 && pStartCode[i+1] == 0x00 && pStartCode[i+2] == 0x00 &&
                        pStartCode[i+3] == 0x01) {
                    if (pStartCode[i+4] == 0x67) {
                        //printf("duplicate sps found!\n");
                        i += 4;
                        continue;
                    }
                    else if (pStartCode[i+4] == 0x68) {
                        //printf("duplicate pps found!\n");
                        i += 4;
                        continue;
                    }
                    else if (pStartCode[i+4] == 0x06) {
                        //printf("duplicate sei found!\n");
                        i += 4;
                        continue;
                    }
                    else {
                        i += 4;
                        //printf("offset=%d\n",i);
                        duplicate = true;
                        break;
                    }
                }
            }

            if (duplicate) {
                nalu_len -= i;
                (*it_tag)->_header.nDataSize -= i;
                unsigned char *p = (unsigned char *)&((*it_tag)->_header.nDataSize);
                (*it_tag)->_pTagHeader[1] = p[2];
                (*it_tag)->_pTagHeader[2] = p[1];
                (*it_tag)->_pTagHeader[3] = p[0];
                //printf("after,tagsize=%d\n",(int)ShowU24((*it_tag)->_pTagHeader + 1));
                //printf("%x,%x,%x\n",(*it_tag)->_pTagHeader[1],(*it_tag)->_pTagHeader[2],(*it_tag)->_pTagHeader[3]);

                f.write((char *)(*it_tag)->_pTagHeader, 11);
                switch (_nNalUnitLength) {
                case 4:
                    *((*it_tag)->_pTagData + 5) = p_nalu_len[3];
                    *((*it_tag)->_pTagData + 6) = p_nalu_len[2];
                    *((*it_tag)->_pTagData + 7) = p_nalu_len[1];
                    *((*it_tag)->_pTagData + 8) = p_nalu_len[0];
                    break;
                case 3:
                    *((*it_tag)->_pTagData + 5) = p_nalu_len[2];
                    *((*it_tag)->_pTagData + 6) = p_nalu_len[1];
                    *((*it_tag)->_pTagData + 7) = p_nalu_len[0];
                    break;
                case 2:
                    *((*it_tag)->_pTagData + 5) = p_nalu_len[1];
                    *((*it_tag)->_pTagData + 6) = p_nalu_len[0];
                    break;
                default:
                    *((*it_tag)->_pTagData + 5) = p_nalu_len[0];
                    break;
                }
                //printf("after,nalu_len=%d\n",(int)ShowU32((*it_tag)->_pTagData + 5));
                f.write((char *)(*it_tag)->_pTagData, pStartCode - (*it_tag)->_pTagData);
                /*
                printf("%x,%x,%x,%x,%x,%x,%x,%x,%x\n",(*it_tag)->_pTagData[0],(*it_tag)->_pTagData[1],(*it_tag)->_pTagData[2],
                        (*it_tag)->_pTagData[3],(*it_tag)->_pTagData[4],(*it_tag)->_pTagData[5],(*it_tag)->_pTagData[6],
                        (*it_tag)->_pTagData[7],(*it_tag)->_pTagData[8]);
                */
                f.write((char *)pStartCode + i, (*it_tag)->_header.nDataSize - (pStartCode - (*it_tag)->_pTagData));
                /*
                printf("write size:%d\n", (pStartCode - (*it_tag)->_pTagData) +
                        ((*it_tag)->_header.nDataSize - (pStartCode - (*it_tag)->_pTagData)));
                */
            } else {
                f.write((char *)(*it_tag)->_pTagHeader, 11);
                f.write((char *)(*it_tag)->_pTagData, (*it_tag)->_header.nDataSize);
            }
        } else {
            f.write((char *)(*it_tag)->_pTagHeader, 11);
            f.write((char *)(*it_tag)->_pTagData, (*it_tag)->_header.nDataSize);
        }

        nLastTagSize = 11 + (*it_tag)->_header.nDataSize;
    }
    unsigned int nn = WriteU32(nLastTagSize);
    f.write((char *)&nn, 4);

    f.close();

    return 1;
}


Tag *FlvFormatParser::getTag(){
	WaitForSingleObject(mEvent, INFINITE);
	if (_vpTag.empty()){
		return NULL;
	} else {
		Tag *cur = _vpTag.front();
		_vpTag.pop_front();

		SetEvent(mEvent);
		return cur;
	}
}
int FlvFormatParser::writeTail(fstream *f){
	unsigned int nn = WriteU32(nLastTagSize);
	f->write((char *)&nn, 4);

	f->close();
	return 0;
}
int FlvFormatParser::saveTagToRingBuffer(Tag *tag, RingBuffer *ringBuffer){
	int writeSize = 0;
	unsigned int nn = WriteU32(nLastTagSize);
	writeSize += ringBuffer->writeData((char *)&nn, 4);

	//check duplicate start code
	if (tag->_header.nType == 0x09 && *(tag->_pTagData + 1) == 0x01) {
		int keyFrame = ((tag->_pTagData[0]) >> 4) & 0xF;
		CVideoTag *vTag = (CVideoTag*)tag;
		if (keyFrame == 1){
			if (mOperation == Oper_Encrypt_Flv){
				int outSize = 0;
				unsigned char *encData = NULL;
				EncryptData(vTag->_pTagData, vTag->_header.nDataSize);

			}
			else if (mOperation == Oper_Decrypt_Flv){
				unsigned char *decData = NULL;
				int decSize = 0;
				DecryptData(vTag->_pTagData, vTag->_header.nDataSize);

			}
		}

		bool duplicate = false;
		unsigned char *pStartCode = tag->_pTagData + 5 + _nNalUnitLength;
		//printf("tagsize=%d\n",(*it_tag)->_header.nDataSize);
		unsigned nalu_len = 0;
		unsigned char *p_nalu_len = (unsigned char *)&nalu_len;
		switch (_nNalUnitLength) {
		case 4:
			nalu_len = ShowU32(tag->_pTagData + 5);
			break;
		case 3:
			nalu_len = ShowU24(tag->_pTagData + 5);
			break;
		case 2:
			nalu_len = ShowU16(tag->_pTagData + 5);
			break;
		default:
			nalu_len = ShowU8(tag->_pTagData + 5);
			break;
		}

		unsigned char *pStartCodeRecord = pStartCode;
		int i;
		for (i = 0; i < tag->_header.nDataSize - 5 - _nNalUnitLength - 4; ++i) {
			if (pStartCode[i] == 0x00 && pStartCode[i + 1] == 0x00 && pStartCode[i + 2] == 0x00 &&
				pStartCode[i + 3] == 0x01) {
				if (pStartCode[i + 4] == 0x67) {
					//printf("duplicate sps found!\n");
					i += 4;
					continue;
				}
				else if (pStartCode[i + 4] == 0x68) {
					//printf("duplicate pps found!\n");
					i += 4;
					continue;
				}
				else if (pStartCode[i + 4] == 0x06) {
					//printf("duplicate sei found!\n");
					i += 4;
					continue;
				}
				else {
					i += 4;
					//printf("offset=%d\n",i);
					duplicate = true;
					break;
				}
			}
		}

		if (duplicate) {
			nalu_len -= i;
			tag->_header.nDataSize -= i;
			unsigned char *p = (unsigned char *)&(tag->_header.nDataSize);
			tag->_pTagHeader[1] = p[2];
			tag->_pTagHeader[2] = p[1];
			tag->_pTagHeader[3] = p[0];

			ringBuffer->writeData((char *)tag->_pTagHeader, 11);
			switch (_nNalUnitLength) {
			case 4:
				*(tag->_pTagData + 5) = p_nalu_len[3];
				*(tag->_pTagData + 6) = p_nalu_len[2];
				*(tag->_pTagData + 7) = p_nalu_len[1];
				*(tag->_pTagData + 8) = p_nalu_len[0];
				break;
			case 3:
				*(tag->_pTagData + 5) = p_nalu_len[2];
				*(tag->_pTagData + 6) = p_nalu_len[1];
				*(tag->_pTagData + 7) = p_nalu_len[0];
				break;
			case 2:
				*(tag->_pTagData + 5) = p_nalu_len[1];
				*(tag->_pTagData + 6) = p_nalu_len[0];
				break;
			default:
				*(tag->_pTagData + 5) = p_nalu_len[0];
				break;
			}

			writeSize += ringBuffer->writeData((char *)tag->_pTagData, pStartCode - tag->_pTagData);
			writeSize += ringBuffer->writeData((char *)pStartCode + i, tag->_header.nDataSize - (pStartCode - tag->_pTagData));

		}
		else {
			writeSize += ringBuffer->writeData((char *)tag->_pTagHeader, 11);
			writeSize += ringBuffer->writeData((char *)tag->_pTagData, tag->_header.nDataSize);
		}
	}
	else {
		writeSize += ringBuffer->writeData((char *)tag->_pTagHeader, 11);
		writeSize += ringBuffer->writeData((char *)tag->_pTagData, tag->_header.nDataSize);
	}

	nLastTagSize = 11 + tag->_header.nDataSize;

	return writeSize;
}
void FlvFormatParser::saveKeyFrameInfo(fstream *file){
	if (mTimeToOffset.empty() || file == NULL){
		return;
	}

	char tagHeader[11] = { 0 };
	tagHeader[0] = 0x0A;

	int dataSize = mTimeToOffset.size() * 8;
	ConvertToHex((unsigned char*)(tagHeader + 1), dataSize);
	file->write(tagHeader, 11);

	for (int i = 0; i < mTimeToOffset.size(); i++){
		TimeToOffset *to = mTimeToOffset[i];
		unsigned char time[4] = { 0 };
		unsigned char offset[4] = { 0 };
		ConvertToHex4(time, to->time);
		ConvertToHex4(offset, to->offset);
		file->write((char*)time, 4);
		file->write((char*)offset, 4);
		delete to;
	}

	unsigned int tagSize = WriteU32(dataSize + 11);
	file->write((char*)&tagSize, 4);


}

std::vector<TimeToOffset*> *FlvFormatParser::parserKeyFrameInfo(unsigned char *data, int dataSize){
	if (data == NULL || dataSize < 11){
		return NULL;
	}
	
	int index = 0;
	if (data[index] != 0x0A){
		return NULL;
	}

	std::vector<TimeToOffset*> *vec = new std::vector<TimeToOffset*>;
	index += 11;
	while (index < dataSize) {
		int time = HexToValue(data + index);
		index += 4;
		int offset = HexToValue(data + index);

		vec->push_back(new TimeToOffset(time, offset));
		index += 4;
	};

	
	return vec;

}

int FlvFormatParser::writeTag(fstream *f, Tag *tag){
	unsigned int nn = WriteU32(nLastTagSize);
	f->write((char *)&nn, 4);

	//check duplicate start code
	if (tag->_header.nType == 0x09 && *(tag->_pTagData + 1) == 0x01) {
		int keyFrame = ((tag->_pTagData[0]) >> 4) & 0xF;		
		CVideoTag *vTag = (CVideoTag*)tag;
		if (keyFrame == 1){
			if (mOperation == Oper_Encrypt_Flv){
				int outSize = 0;
				unsigned char *encData = NULL;
				EncryptData(vTag->_pTagData, vTag->_header.nDataSize);
				mTimeToOffset.push_back(new TimeToOffset(vTag->_header.nTimeStamp, vTag->_header.nByteOffset));

			} else if (mOperation == Oper_Decrypt_Flv){
				unsigned char *decData = NULL;
				int decSize = 0;
				DecryptData(vTag->_pTagData, vTag->_header.nDataSize);

			}
		}
			
		bool duplicate = false;
		unsigned char *pStartCode = tag->_pTagData + 5 + _nNalUnitLength;
		//printf("tagsize=%d\n",(*it_tag)->_header.nDataSize);
		unsigned nalu_len = 0;
		unsigned char *p_nalu_len=(unsigned char *)&nalu_len;
		switch (_nNalUnitLength) {
		case 4:
			nalu_len = ShowU32(tag->_pTagData + 5);
			break;
		case 3:
			nalu_len = ShowU24(tag->_pTagData + 5);
			break;
		case 2:
			nalu_len = ShowU16(tag->_pTagData + 5);
			break;
		default:
			nalu_len = ShowU8(tag->_pTagData + 5);
			break;
		}

		unsigned char *pStartCodeRecord = pStartCode;
		int i;
		for (i = 0; i < tag->_header.nDataSize - 5 - _nNalUnitLength - 4; ++i) {
			if (pStartCode[i] == 0x00 && pStartCode[i+1] == 0x00 && pStartCode[i+2] == 0x00 &&
					pStartCode[i+3] == 0x01) {
				if (pStartCode[i+4] == 0x67) {
					//printf("duplicate sps found!\n");
					i += 4;
					continue;
				}
				else if (pStartCode[i+4] == 0x68) {
					//printf("duplicate pps found!\n");
					i += 4;
					continue;
				}
				else if (pStartCode[i+4] == 0x06) {
					//printf("duplicate sei found!\n");
					i += 4;
					continue;
				}
				else {
					i += 4;
					//printf("offset=%d\n",i);
					duplicate = true;
					break;
				}
			}
		}

		if (duplicate) {
			nalu_len -= i;
			tag->_header.nDataSize -= i;
			unsigned char *p = (unsigned char *)&(tag->_header.nDataSize);
			tag->_pTagHeader[1] = p[2];
			tag->_pTagHeader[2] = p[1];
			tag->_pTagHeader[3] = p[0];
              
			f->write((char *)tag->_pTagHeader, 11);
			switch (_nNalUnitLength) {
			case 4:
				*(tag->_pTagData + 5) = p_nalu_len[3];
				*(tag->_pTagData + 6) = p_nalu_len[2];
				*(tag->_pTagData + 7) = p_nalu_len[1];
				*(tag->_pTagData + 8) = p_nalu_len[0];
				break;
			case 3:
				*(tag->_pTagData + 5) = p_nalu_len[2];
				*(tag->_pTagData + 6) = p_nalu_len[1];
				*(tag->_pTagData + 7) = p_nalu_len[0];
				break;
			case 2:
				*(tag->_pTagData + 5) = p_nalu_len[1];
				*(tag->_pTagData + 6) = p_nalu_len[0];
				break;
			default:
				*(tag->_pTagData + 5) = p_nalu_len[0];
				break;
			}

			f->write((char *)tag->_pTagData, pStartCode - tag->_pTagData);
			f->write((char *)pStartCode + i, tag->_header.nDataSize - (pStartCode - tag->_pTagData));
	
		} else {
			f->write((char *)tag->_pTagHeader, 11);
			f->write((char *)tag->_pTagData, tag->_header.nDataSize);
		}
	} else {
		f->write((char *)tag->_pTagHeader, 11);
		f->write((char *)tag->_pTagData, tag->_header.nDataSize);
	}

	nLastTagSize = 11 + tag->_header.nDataSize;

	return 1;
}


bool FlvFormatParser::EncryptData(unsigned char *pVideoData, int srcSize){
	if (pVideoData == NULL || srcSize <= 0){
		return false;
	}

	int videoDataSize = srcSize - 2;

	int supplymentSize = videoDataSize % 16;
	unsigned char *cur = NULL;

	videoDataSize = supplymentSize == 0 ? videoDataSize : videoDataSize - supplymentSize;
	if (videoDataSize > 0){
		(unsigned char*)mAes->encryptData(pVideoData + 2, videoDataSize);
		return true;
	}
	else {
		return false;
	}
}
bool FlvFormatParser::DecryptData(unsigned char *pVideoData, int videoDataSize){
	if (pVideoData == NULL || videoDataSize <= 0){
		return false;
	}

	int validateSize = videoDataSize - 2;
	int supplymentSize = validateSize % 16;
	validateSize = supplymentSize == 0 ? validateSize : validateSize - supplymentSize;
	if (validateSize > 0){
		mAes->decryptData(pVideoData + 2, videoDataSize);
		return true;
	} else {
		return false;
	}
}

int FlvFormatParser::Stat()
{
// 	for (int i = 0; i < _vpTag.size(); i++)
// 	{
// 		switch (_vpTag[i]->_header.nType)
// 		{
// 		case 0x08:
// 			_sStat.nAudioNum++;
// 			break;
// 		case 0x09:
// 			StatVideo(_vpTag[i]);
// 			break;
// 		case 0x12:
// 			_sStat.nMetaNum++;
// 			break;
// 		default:
// 			;
// 		}
// 	}

	return 1;
}

int FlvFormatParser::StatVideo(Tag *pTag)
{
	_sStat.nVideoNum++;
	_sStat.nMaxTimeStamp = pTag->_header.nTimeStamp;

	if (pTag->_pTagData[0] == 0x17 && pTag->_pTagData[1] == 0x00)
	{
		_sStat.nLengthSize = (pTag->_pTagData[9] & 0x03) + 1;
	}

	return 1;
}

FlvHeader *FlvFormatParser::CreateFlvHeader(unsigned char *pBuf)
{
	FlvHeader *pHeader = new FlvHeader;
	pHeader->nVersion = pBuf[3];
	pHeader->bHaveAudio = (pBuf[4] >> 2) & 0x01;
	pHeader->bHaveVideo = (pBuf[4] >> 0) & 0x01;
	pHeader->nHeadSize = ShowU32(pBuf + 5);

	pHeader->pFlvHeader = new unsigned char[pHeader->nHeadSize];
	memcpy(pHeader->pFlvHeader, pBuf, pHeader->nHeadSize);

	return pHeader;
}

int FlvFormatParser::DestroyFlvHeader(FlvHeader *pHeader)
{
	if (pHeader == NULL)
		return 0;

	delete pHeader->pFlvHeader;
	return 1;
}

void Tag::Init(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen)
{
	memcpy(&_header, pHeader, sizeof(TagHeader));

	_pTagHeader = new unsigned char[11];
	memcpy(_pTagHeader, pBuf, 11);

	_pTagData = new unsigned char[_header.nDataSize];
	memcpy(_pTagData, pBuf + 11, _header.nDataSize);

}

FlvFormatParser::CVideoTag::CVideoTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, FlvFormatParser *pParser, int Operation) : mOperation(Operation)
{
	Init(pHeader, pBuf, nLeftLen);

	unsigned char *pd = _pTagData;
	_nFrameType = (pd[0] & 0xf0) >> 4;
	_nCodecID = pd[0] & 0x0f;
	if (_header.nType == 0x09 && _nCodecID == 7)
	{
		ParseH264Tag(pParser);
	}
}

FlvFormatParser::CAudioTag::CAudioTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, FlvFormatParser *pParser)
{
	Init(pHeader, pBuf, nLeftLen);

	unsigned char *pd = _pTagData;
	_nSoundFormat = (pd[0] & 0xf0) >> 4;
	_nSoundRate = (pd[0] & 0x0c) >> 2;
	_nSoundSize = (pd[0] & 0x02) >> 1;
	_nSoundType = (pd[0] & 0x01);
	if (_nSoundFormat == 10) // AAC
	{
		ParseAACTag(pParser);
	}
}

int FlvFormatParser::CAudioTag::ParseAACTag(FlvFormatParser *pParser)
{
	unsigned char *pd = _pTagData;
	int nAACPacketType = pd[1];

	if (nAACPacketType == 0)
	{
		ParseAudioSpecificConfig(pParser, pd);
	}
	else if (nAACPacketType == 1)
	{
		ParseRawAAC(pParser, pd);
	}
	else
	{

	}

	return 1;
}

int FlvFormatParser::CAudioTag::ParseAudioSpecificConfig(FlvFormatParser *pParser, unsigned char *pTagData)
{
	unsigned char *pd = _pTagData;

	_aacProfile = ((pd[2]&0xf8)>>3) - 1;
	_sampleRateIndex = ((pd[2]&0x07)<<1) | (pd[3]>>7);
	_channelConfig = (pd[3]>>3) & 0x0f;

	_pMedia = NULL;
	_nMediaLen = 0;

	return 1;
}

int FlvFormatParser::CAudioTag::ParseRawAAC(FlvFormatParser *pParser, unsigned char *pTagData)
{
	uint64_t bits = 0;
	int dataSize = _header.nDataSize - 2;

	WriteU64(bits, 12, 0xFFF);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 2, 0);
	WriteU64(bits, 1, 1);
	WriteU64(bits, 2, _aacProfile);
	WriteU64(bits, 4, _sampleRateIndex);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 3, _channelConfig);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 13, 7 + dataSize);
	WriteU64(bits, 11, 0x7FF);
	WriteU64(bits, 2, 0);

	_nMediaLen = 7 + dataSize;
	_pMedia = new unsigned char[_nMediaLen];
	unsigned char p64[8];
	p64[0] = (unsigned char)(bits >> 56);
	p64[1] = (unsigned char)(bits >> 48);
	p64[2] = (unsigned char)(bits >> 40);
	p64[3] = (unsigned char)(bits >> 32);
	p64[4] = (unsigned char)(bits >> 24);
	p64[5] = (unsigned char)(bits >> 16);
	p64[6] = (unsigned char)(bits >> 8);
	p64[7] = (unsigned char)(bits);

	memcpy(_pMedia, p64+1, 7);
	memcpy(_pMedia + 7, pTagData + 2, dataSize);

	return 1;
}
Tag *FlvFormatParser::CreateTag(unsigned char *pBuf, int nLeftLen)
{
	TagHeader header;
	header.nType = ShowU8(pBuf+0);
	header.nDataSize = ShowU24(pBuf + 1);
	header.nTimeStamp = ShowU24(pBuf + 4);
	header.nTSEx = ShowU8(pBuf + 7);
	header.nStreamID = ShowU24(pBuf + 8);
	header.nTotalTS = (unsigned int)((header.nTSEx << 24)) + header.nTimeStamp;
	cout << "total TS : " << header.nTotalTS << endl;
	//cout << "nLeftLen : " << nLeftLen << " , nDataSize : " << pTag->header.nDataSize << endl;
	if ((header.nDataSize + 11) > nLeftLen)
	{
		return NULL;
	}

	Tag *pTag;
	switch (header.nType) {
	case 0x09:
		pTag = new CVideoTag(&header, pBuf, nLeftLen, this, mOperation);
		break;
	case 0x08:
		pTag = new CAudioTag(&header, pBuf, nLeftLen, this);
		break;
	default:
		pTag = new Tag();
		pTag->Init(&header, pBuf, nLeftLen);
	}
	
	return pTag;
}

int FlvFormatParser::DestroyTag(Tag *pTag)
{
	if (pTag->_pMedia != NULL)
		delete []pTag->_pMedia;
	if (pTag->_pTagData!=NULL)
		delete []pTag->_pTagData;
	if (pTag->_pTagHeader != NULL)
		delete []pTag->_pTagHeader;

	return 1;
}

int FlvFormatParser::CVideoTag::ParseH264Tag(FlvFormatParser *pParser)
{
	unsigned char *pd = _pTagData;
	int nAVCPacketType = pd[1];
	int nCompositionTime = ShowU24(pd + 2);

	if (nAVCPacketType == 0)
	{
		ParseH264Configuration(pParser, pd);
	}
	else if (nAVCPacketType == 1)
	{
		if (mOperation == Oper_Normal || mOperation == Oper_Encrypt_Flv){
			ParseNalu(pParser, pd);
		}		
	}
	else
	{

	}
	return 1;
}

int FlvFormatParser::CVideoTag::ParseH264Configuration(FlvFormatParser *pParser, unsigned char *pTagData)
{
	unsigned char *pd = pTagData;

	pParser->_nNalUnitLength = (pd[9] & 0x03) + 1;

	int sps_size, pps_size;
	sps_size = ShowU16(pd + 11);
	pps_size = ShowU16(pd + 11 + (2 + sps_size) + 1);
	
	_nMediaLen = 4 + sps_size + 4 + pps_size;
	_pMedia = new unsigned char[_nMediaLen];
	memcpy(_pMedia, &nH264StartCode, 4);
	memcpy(_pMedia + 4, pd + 11 + 2, sps_size);
	memcpy(_pMedia + 4 + sps_size, &nH264StartCode, 4);
	memcpy(_pMedia + 4 + sps_size + 4, pd + 11 + 2 + sps_size + 2 + 1, pps_size);

	return 1;
}

int FlvFormatParser::CVideoTag::ParseNalu(FlvFormatParser *pParser, unsigned char *pTagData)
{
	unsigned char *pd = pTagData;
	int nOffset = 0;

	_pMedia = new unsigned char[_header.nDataSize+10];
	_nMediaLen = 0;

	nOffset = 5;
	while (1)
	{
		if (nOffset >= _header.nDataSize)
			break;

		int nNaluLen;
		switch (pParser->_nNalUnitLength)
		{
		case 4:
			nNaluLen = ShowU32(pd + nOffset);
			break;
		case 3:
			nNaluLen = ShowU24(pd + nOffset);
			break;
		case 2:
			nNaluLen = ShowU16(pd + nOffset);
			break;
		default:
			nNaluLen = ShowU8(pd + nOffset);
		}
		memcpy(_pMedia + _nMediaLen, &nH264StartCode, 4);
		memcpy(_pMedia + _nMediaLen + 4, pd + nOffset + pParser->_nNalUnitLength, nNaluLen);
		_nMediaLen += (4 + nNaluLen);
		nOffset += (pParser->_nNalUnitLength + nNaluLen);
	}

	return 1;
}
