#ifndef __STREAM_H_
#define __STREAM_H_

#include <windows.h>

#define NUM_STREAMS 20

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

typedef void (*StreamProc)(PStream);

HBITMAP MakeDib(RGBA** pvBits);

void EnumStreams(StreamProc callback);
int get_slider_pos(HWND hwnd, int dlgItemID);

extern PStream StreamList[NUM_STREAMS];

#endif