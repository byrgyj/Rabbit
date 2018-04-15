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

extern "C" DLL_API bool init(int type, const char *srcFile, const char *destFile);
extern "C" DLL_API int encryptFlvFile();
extern "C" DLL_API int decryptFlvData(const char *srcBuffer, int srcBufferSize, const char *destData, int destDataSize);

extern "C" DLL_API int comsumeFlvData(char *buffer, int dataSize);
extern "C" DLL_API int consumeMp4Data(char *buffer, int dataSize);
extern "C" DLL_API bool seekTo(int millsec);


#ifdef __cplusplus  
}
#endif