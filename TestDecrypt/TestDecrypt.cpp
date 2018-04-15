// TestDecrypt.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <fstream>

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE dll = LoadLibrary(L"FlvProcess.dll");
	if (dll == NULL){
		return 0;
	}

	typedef bool(*apiInit)(int, const char *, const char *);
	typedef int(*apiComsumeFlvData)(char *, int);

	apiInit init = NULL;
	apiComsumeFlvData consumeData = NULL;

	init = (apiInit)GetProcAddress(dll, "init");
	//consumeData = (apiComsumeFlvData)GetProcAddress(dll, "consumeMp4Data");
	consumeData = (apiComsumeFlvData)GetProcAddress(dll, "comsumeFlvData");

	if (init == NULL || consumeData == NULL){
		return 0;
	}

	//if (!init(4, "enc1.mp4",  "out_test.mp4")){
	if (!init(2, "enc.flv", "enc_to_dec.flv")){
		return -1;
	}

	char szBuffer[512] = { 0 };
	int readSz = 0;

	std::fstream file("enc_to_dec.flv", std::ios_base::out | std::ios_base::binary);
	while (true){
		memset(szBuffer, 0, 512);
		//printf("getData ...\n");
		int sz = consumeData(szBuffer, 512);
		if (sz == 0){
			break;
		}
		readSz += sz;
		//printf("getData end [%d, %d]\n", sz, readSz);
		file.write(szBuffer, sz);
	}

	file.close();

	getchar();
	return 0;
}

