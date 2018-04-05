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
	consumeData = (apiComsumeFlvData)GetProcAddress(dll, "comsumeFlvData");

	if (init == NULL || consumeData == NULL){
		return 0;
	}

	if (!init(2, "enc.flv",  NULL)){
		return -1;
	}

	char szBuffer[512] = { 0 };
	int readSz = 0;

	std::fstream file("enc_to_dec.flv", std::ios_base::out | std::ios_base::binary);
	while (true){
		memset(szBuffer, 0, 512);
		int sz = consumeData(szBuffer, 512);
		if (sz == 0){
			break;
		}
		readSz += sz;
		file.write(szBuffer, sz);
	}

	file.close();
	return 0;
}

