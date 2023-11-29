#ifndef __SOLID_H_
#define __SOLID_H_

#include "stream.h"

typedef struct tagSolidColorStream{
    Stream stream;
    RGBTriplet color;
} SolidColorStream;

SolidColorStream* SCC_OpenStream();

#endif
