#include "fade.h"

#define WeightAvg(num0, num1, amnt)    (((num0 * amnt) + (num1 * (256 - amnt))) >> 8)

int FS_OutputFrame(FadeStream* pStream){
    int i;
	RGBTriplet* pBits;
	RGBTriplet* child0bits;
	RGBTriplet* child1bits;

	if(!pStream->child0 || !pStream->child1) return 0;

	pBits = (RGBTriplet*)pStream->stream.pBits;
	child0bits = (RGBTriplet*)pStream->child0->pBits;
	child1bits = (RGBTriplet*)pStream->child1->pBits;

	OutputFrame(pStream->child0);
	OutputFrame(pStream->child1);

    for(i = 0; i < 640*480; i++){
		pBits[i].r = WeightAvg(child0bits[i].r, child1bits[i].r, pStream->fade_amount);
		pBits[i].g = WeightAvg(child0bits[i].g, child1bits[i].g, pStream->fade_amount);
		pBits[i].b = WeightAvg(child0bits[i].b, child1bits[i].b, pStream->fade_amount);
    }

    return 1;
}

int FS_ProcCmd(FadeStream* pStream, int cmd, LPARAM lParam){
	switch(cmd){
		case FSC_GETPOS:
			return pStream->fade_amount;
		case FSC_SETPOS:
			pStream->fade_amount = lParam;
			return 0;
		default:
			return -1;
	}
}

FadeStream* FS_OpenStream(LPARAM param){
	FadeStream* stream = CreateStream(sizeof(FadeStream), FS_OutputFrame, FS_ProcCmd);
	stream->fade_amount = param;
	return stream;
}