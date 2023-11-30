#ifndef __KEY_H_
#define __KEY_H_

#include "stream.h"

#define CKC_SET_A 0x1006
#define CKC_SET_B 0x1007

typedef struct tagChromaKeyStream{
    Stream stream;
    Stream* fg;
    Stream* bg;
    RGBTriplet keycolor;
    int max_distance;
} ChromaKeyStream;

ChromaKeyStream* CK_OpenStream(LPARAM param);

#endif
