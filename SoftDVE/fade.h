#ifndef __FADE_H_
#define __FADE_H_

#include "stream.h"

typedef struct tagFadeStream{
    Stream stream;
    Stream* child0;
    Stream* child1;
    WORD fade_amount;
} FadeStream;

FadeStream* FS_OpenStream();

#endif
