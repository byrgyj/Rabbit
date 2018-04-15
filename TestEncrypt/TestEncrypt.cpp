// TestEncrypt.cpp : 定义控制台应用程序的入口点。
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
	char *srcFile = "moov_befor_mdat.mp4";
	//char *srcFile = "out_ffd5.mp4";
	//bool bInit = init(3, srcFile, "enc.mp4");
	if (!bInit){
		return -1;
	}

	encryptFlvFile();

	return 0;
}

