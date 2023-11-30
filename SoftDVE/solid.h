#ifndef __SOLID_H_
#define __SOLID_H_

#include "stream.h"

#define SCC_SETCOL 0x1004
#define SCC_GETCOL 0x1005

typedef struct tagSolidColorStream{
    Stream stream;
    RGBTriplet color;
} SolidColorStream;

SolidColorStream* SCC_OpenStream(LPARAM param);

#endif
