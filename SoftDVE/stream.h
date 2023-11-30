#ifndef __STREAM_H_
#define __STREAM_H_

#include <windows.h>

#define NUM_STREAMS 20

typedef DWORD RGBA;

typedef int (*PStream_OutputFrame)(struct tagStream*);
typedef int (*PStream_ProcCmd)(struct tagStream*, int, LPARAM);

typedef struct tagRGBTriplet{
	BYTE b, g, r, a;
} RGBTriplet;

typedef struct tagStream{
	HBITMAP hDib;
	RGBA* pBits;
	BOOL valid;
	PStream_OutputFrame OutputFrame;
	PStream_ProcCmd ProcCmd;
} Stream, *PStream;

typedef void (*StreamProc)(PStream);

PStream CreateStream(int sz, PStream_OutputFrame pFnFrame, PStream_ProcCmd pFnCmd);

HBITMAP MakeDib(RGBA** pvBits);
void DrawPreview(PStream stream, PDRAWITEMSTRUCT pDIS);
void EnumStreams(StreamProc callback);

void OutputFrame(PStream stream);

void DoRender(HWND);

extern PStream StreamList[NUM_STREAMS];

void InitStreams();

#endif
