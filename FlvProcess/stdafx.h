// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

enum OperationMode { Oper_Normal, Oper_Encrypt_Flv, Oper_Decrypt_Flv, Oper_Encrypt_Mp4, Oper_Decrypt_Mp4 };

#define RELEASE_POINTER(ptr) if (ptr != NULL) { delete ptr; ptr = NULL; }
#define CLOSE_FILE(file) if (file != NULL) { fclose(file); file = NULL; }

// TODO: reference additional headers your program requires here
