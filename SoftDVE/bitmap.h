#ifndef __BMP_H_
#define __BMP_H_

#include "stream.h"

typedef struct tagDIBStream{
    Stream stream;
} DIBStream;

DIBStream* DS_OpenStream();

#endif
