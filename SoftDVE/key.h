#ifndef __KEY_H_
#define __KEY_H_

#include "stream.h"

typedef struct tagChromaKeyStream{
    Stream stream;
    Stream* fg;
    Stream* bg;
    RGBTriplet keycolor;
    int max_distance;
} ChromaKeyStream;

#endif
