// TestEncrypt.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	typedef bool(*apiInit)(int, const char *, const char *);
	typedef int(*apiEncryptFlvFile)();

	apiInit init = NULL;
	apiEncryptFlvFile encryptFlvFile = NULL;
	HINSTANCE dll = LoadLibrary(L"FlvProcess.dll");
	if (dll == NULL){
		return 0;
	}

	init = (apiInit)GetProcAddress(dll, "init");
	encryptFlvFile = (apiEncryptFlvFile)GetProcAddress(dll, "encryptFlvFile");

	if (init == NULL || encryptFlvFile == NULL){
		return 0;
	}

	bool bInit = init(1, "ffd5.f4v", "enc.flv");
	if (!bInit){
		return -1;
	}

	encryptFlvFile();

	return 0;
}
