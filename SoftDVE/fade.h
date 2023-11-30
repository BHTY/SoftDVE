#ifndef __FADE_H_
#define __FADE_H_

#include "stream.h"

#define FSC_GETPOS 0x1002
#define FSC_SETPOS 0x1003

typedef struct tagFadeStream{
    Stream stream;
    Stream* child0;
    Stream* child1;
    WORD fade_amount;
} FadeStream;

FadeStream* FS_OpenStream(LPARAM param);

#endif
