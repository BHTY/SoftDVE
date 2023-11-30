#include "solid.h"

int SCC_OutputFrame(SolidColorStream* pStream){
    int i;
    
    if (pStream == NULL) return 0;

    for(i = 0; i < 640*480; i++){
        pStream->stream.pBits[i] = *(RGBA*)&(pStream->color);
    }

    return 1;
}

int SCC_ProcCmd(SolidColorStream* pStream, int cmd, LPARAM lParam){
	switch(cmd){
		case SCC_GETCOL:
			return *(int*)&(pStream->color);
		case SCC_SETCOL:
			pStream->color = *(RGBTriplet*)&lParam;
			return 0;
		default:
			return -1;
	}
}

SolidColorStream* SCC_OpenStream(LPARAM param){
	SolidColorStream* stream = CreateStream(sizeof(SolidColorStream), SCC_OutputFrame, SCC_ProcCmd);
	stream->color = *(RGBTriplet*)&param;
	return stream;
}