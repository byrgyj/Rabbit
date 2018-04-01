#pragma once

#ifdef __cplusplus  
extern "C"  
{ 
#endif

#ifdef FLVPROCESS_EXPORTS
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspce(dllimport)
#endif

extern "C" DLL_API bool init(int type);
extern "C" DLL_API int encryptFlvFile(const char *srcFilePath, const char *destFilePath);
extern "C" DLL_API int decryptFlvData(const char *srcBuffer, int srcBufferSize, const char *destData, int destDataSize);


#ifdef __cplusplus  
}
#endif