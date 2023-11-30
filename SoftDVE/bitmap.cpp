#include "bitmap.h"

int DS_LoadFile(DIBStream* stream, LPSTR filename){
	HDC memDC1, memDC2;
	HBITMAP hBMP;

	memDC1 = CreateCompatibleDC(GetDC(NULL));
	memDC2 = CreateCompatibleDC(GetDC(NULL));

	hBMP = LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if(hBMP){

		SelectObject(memDC1, stream->stream.hDib);
		SelectObject(memDC2, hBMP);

		BitBlt(memDC1, 0, 0, 640, 480, memDC2, 0, 0, SRCCOPY);

		DeleteObject(memDC2);
		DeleteObject(memDC1);
		DeleteObject(hBMP);

		return 1;

	}

	return NULL;
}

int DS_ProcCmd(DIBStream* pStream, int cmd, LPARAM param){
	switch(cmd){
		case DSC_SETBMP:
			return DS_LoadFile(pStream, param);
		default:
			return -1;
	}
}

int DS_OutputFrame(DIBStream* pStream){
	return 1;
}

DIBStream* DS_OpenStream(LPARAM param){
	DIBStream* stream = CreateStream(sizeof(DIBStream), DS_OutputFrame, DS_ProcCmd);
	if(param) DS_LoadFile(stream, param);
	return stream;
}