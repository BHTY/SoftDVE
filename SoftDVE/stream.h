#ifndef __STREAM_H_
#define __STREAM_H_

#include <windows.h>

typedef DWORD RGBA;

typedef int (*PStream_OutputFrame)(void*);

typedef struct tagRGBTriplet{
    BYTE b, g, r, a;
} RGBTriplet;

typedef struct tagStream{
    HWND hwnd;
	HBITMAP hDib;
	RGBA* pBits;
	int valid;
	char* name;
    PStream_OutputFrame OutputFrame;
} Stream, *PStream;

#endif