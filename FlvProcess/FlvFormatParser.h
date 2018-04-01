#ifndef FLVPARSER_1_H
#define FLVPARSER_1_H

#include <vector>
#include <list>
#include <Windows.h>
#include "Aes.h"

using namespace std;

typedef unsigned long long uint64_t;

enum OperationMode { Oper_Normal, Oper_Encrypt, Oper_Decrypt };
typedef struct FlvHeader_s
{
	int nVersion;
	int bHaveVideo, bHaveAudio;
	int nHeadSize;

	unsigned char *pFlvHeader;
} FlvHeader;
struct TagHeader
{
	int nType;
	int nDataSize;
	int nTimeStamp;
	int nTSEx;
	int nStreamID;

	unsigned int nTotalTS;

	TagHeader() : nType(0), nDataSize(0), nTimeStamp(0), nTSEx(0), nStreamID(0), nTotalTS(0) {}
	~TagHeader() {}
};
class Tag
{
public:
	Tag() : _pTagHeader(NULL), _pTagData(NULL), _pMedia(NULL), _nMediaLen(0) {}
	void Init(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen);

	TagHeader _header;
	unsigned char *_pTagHeader;
	unsigned char *_pTagData;
	unsigned char *_pMedia;
	int _nMediaLen;
};

static unsigned int ShowU32(unsigned char *pBuf) { return (pBuf[0] << 24) | (pBuf[1] << 16) | (pBuf[2] << 8) | pBuf[3]; }
static unsigned int ShowU24(unsigned char *pBuf) { return (pBuf[0] << 16) | (pBuf[1] << 8) | (pBuf[2]); }
static unsigned int ShowU16(unsigned char *pBuf) { return (pBuf[0] << 8) | (pBuf[1]); }
static unsigned int ShowU8(unsigned char *pBuf) { return (pBuf[0]); }
static void WriteU64(uint64_t & x, int length, int value)
{
	uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
	x = (x << length) | ((uint64_t)value & mask);
}
static unsigned int WriteU32(unsigned int n)
{
	unsigned int nn = 0;
	unsigned char *p = (unsigned char *)&n;
	unsigned char *pp = (unsigned char *)&nn;
	pp[0] = p[3];
	pp[1] = p[2];
	pp[2] = p[1];
	pp[3] = p[0];
	return nn;
}

static void ConvertToHex(unsigned char *dest, unsigned int value){
	unsigned char *p = (unsigned char *)&value;
	dest[0] = p[2];
	dest[1] = p[1];
	dest[2] = p[0];
}

class FlvFormatParser
{
public:
	FlvFormatParser(int operation = 0);
	virtual ~FlvFormatParser();

	int Parse(unsigned char *pBuf, int nBufSize, int &nUsedLen);
	int PrintInfo();
	int DumpH264(const std::string &path);
	int DumpAAC(const std::string &path);
	int DumpFlv(const std::string &path);
	//int DumpFlvToMemoryBuffer(unsigned char *buffer, int bufferSize);

	FlvHeader* getFlvHeader() { return _pFlvHeader; }
	Tag *getTag();
	int writeTag(fstream *f, Tag *tag);
	int writeTail(fstream *f);

	bool EncryptData(unsigned char *pVideoData, int videoDataSize, unsigned char **encryptedData, int &encryptedSize);
	bool DecryptData(unsigned char *pVideoData, int videoDataSize, unsigned char **decryptedData, int &decryptedSize);

private:
	class CVideoTag : public Tag
	{
	public:
		CVideoTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, FlvFormatParser *pParser, int Operation);

		int _nFrameType;
		int _nCodecID;
		int ParseH264Tag(FlvFormatParser *pParser);
		int ParseH264Configuration(FlvFormatParser *pParser, unsigned char *pTagData);
		int ParseNalu(FlvFormatParser *pParser, unsigned char *pTagData);

	private:
		int mOperation;
	};

	class CAudioTag : public Tag
	{
	public:
		CAudioTag(TagHeader *pHeader, unsigned char *pBuf, int nLeftLen, FlvFormatParser *pParser);

		int _nSoundFormat;
		int _nSoundRate;
		int _nSoundSize;
		int _nSoundType;

		// aac
		static int _aacProfile;
		static int _sampleRateIndex;
		static int _channelConfig;

		int ParseAACTag(FlvFormatParser *pParser);
		int ParseAudioSpecificConfig(FlvFormatParser *pParser, unsigned char *pTagData);
		int ParseRawAAC(FlvFormatParser *pParser, unsigned char *pTagData);
	};

	struct FlvStat
	{
		int nMetaNum, nVideoNum, nAudioNum;
		int nMaxTimeStamp;
		int nLengthSize;

		FlvStat() : nMetaNum(0), nVideoNum(0), nAudioNum(0), nMaxTimeStamp(0), nLengthSize(0){}
		~FlvStat() {}
	};

	friend class Tag;
	
private:

	FlvHeader *CreateFlvHeader(unsigned char *pBuf);
	int DestroyFlvHeader(FlvHeader *pHeader);
	Tag *CreateTag(unsigned char *pBuf, int nLeftLen);
	int DestroyTag(Tag *pTag);
	int Stat();
	int StatVideo(Tag *pTag);
	int IsUserDataTag(Tag *pTag);

public:
	bool isParserEnd() { return mParserEnd; }
	void setParseEnd(bool flag) { mParserEnd = flag; }
	unsigned int getLastTagSize() { return nLastTagSize; }
private:

	FlvHeader* _pFlvHeader;
	//vector<Tag *> _vpTag;
	list<Tag *> _vpTag;
	FlvStat _sStat;
	AES *mAes;
	HANDLE mMutex;
	HANDLE mEvent;


	// H.264
	int _nNalUnitLength;

	int mTagIndex;
	int mOperation;
public:
	unsigned int nLastTagSize;
	bool mParserEnd;
};

#endif // FLVPARSER_H
