//#pragma once
#ifndef AES_H
#define AES_H
/*
author: byrgyj@126.com
*/
class AES
{
public:
	AES(unsigned char* key);
	virtual ~AES();
	unsigned char* encryptData(unsigned char* input); // 加密，数据大小固定为16Byte
	unsigned char* decryptData(unsigned char* input); // 解密，数据大小固定为16Byte
	void* encryptData(void* input, int length = 0);// 加密，数据大小必须是16Byte的倍数
	void* decryptData(void* input, int length);// 解密，数据大小必须是16Byte的倍数

private:
	unsigned char mSbox[256];
	unsigned char mInvSbox[256];
	unsigned char w[11][4][4];

	void keyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void subBytes(unsigned char state[][4]);
	void shiftRows(unsigned char state[][4]);
	void mixColumns(unsigned char state[][4]);
	void addRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void invSubBytes(unsigned char state[][4]);
	void invShiftRows(unsigned char state[][4]);
	void invMixColumns(unsigned char state[][4]);
};

#endif