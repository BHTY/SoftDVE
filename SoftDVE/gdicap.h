#ifndef __GDICAP_H_
#define __GDICAP_H_

#include "stream.h"

#define GSC_SETWND 0x1001

typedef struct tagGDIStream{
	Stream stream;
	HWND hwnd;
} GDIStream;

GDIStream* GS_OpenStream(LPARAM lParam);

#endif
