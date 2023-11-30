#ifndef __BMP_H_
#define __BMP_H_

#include "stream.h"

#define DSC_SETBMP 0x1000

typedef struct tagDIBStream{
    Stream stream;
} DIBStream;

DIBStream* DS_OpenStream(LPARAM param);

#endif
