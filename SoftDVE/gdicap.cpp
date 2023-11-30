#include "gdicap.h"

int GS_OutputFrame(GDIStream* pStream){
	HDC memDC, physDC;
	RECT rect;

	if(pStream->hwnd){
		GetWindowRect(pStream->hwnd, &rect);
		physDC = GetDC(pStream->hwnd);
		memDC = CreateCompatibleDC(GetDC(NULL));
		SelectObject(memDC, pStream->stream.hDib);
		StretchBlt(memDC, 0, 0, 640, 480, physDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
		DeleteDC(memDC);
		ReleaseDC(pStream->hwnd, physDC);
		return 1;
	}

	return 0;
}

int GS_ProcCmd(GDIStream* pStream, int cmd, LPARAM param){
	switch(cmd){
		case GSC_SETWND:
			pStream->hwnd = FindWindowA(NULL, param);
			return pStream->hwnd != 0;
		default:
			return -1;
	}
}

GDIStream* GS_OpenStream(LPARAM lParam){
	GDIStream* stream = CreateStream(sizeof(GDIStream), GS_OutputFrame, GS_ProcCmd);
	stream->hwnd = FindWindowA(NULL, lParam);
	return stream;
}