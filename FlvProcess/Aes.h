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
	unsigned char* encryptData(unsigned char* input); // ���ܣ����ݴ�С�̶�Ϊ16Byte
	unsigned char* decryptData(unsigned char* input); // ���ܣ����ݴ�С�̶�Ϊ16Byte
	void* encryptData(void* input, int length = 0);// ���ܣ����ݴ�С������16Byte�ı���
	void* decryptData(void* input, int length);// ���ܣ����ݴ�С������16Byte�ı���

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