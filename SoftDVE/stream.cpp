#include "stream.h"
#include "dlg.h"

PStream StreamList[NUM_STREAMS];

int AddStream(PStream pStream){
	int i;

	for(i = 0; i < NUM_STREAMS; i++){
		if(StreamList[i] == 0){
			StreamList[i] = pStream;

			return 1;
		}
	}

	return 0;
}

void EnumStreams(StreamProc callback){
	int i;

	for(i = 0; i < NUM_STREAMS; i++){
		if(StreamList[i]){
			callback(StreamList[i]);
		}
	}
}

void InvalidateBits(PStream pStream){
	pStream->valid = 0;
}

void OutputFrame(PStream stream){
	if(!stream->valid){
		stream->OutputFrame(stream);
		stream->valid = 1;
	}
}

void RepaintPreviews(DlgState* pDLG){
	if(pDLG->type == DLG_IMAGE){
		//printf("Invalidating control %d\n", pDLG->id);
		InvalidateRect(pDLG->hwnd, NULL, 0);
	}
}

void DoRender(HWND hwndRoot){
	EnumStreams(InvalidateBits);
	EnumStreams(OutputFrame);
	EnumDlgControls(GetWindowLongPtrA(hwndRoot, 0), RepaintPreviews);
}

void DrawPreview(PStream stream, PDRAWITEMSTRUCT pDIS){
	RECT rect;
	HDC memDC = CreateCompatibleDC(GetDC(NULL));
	SelectObject(memDC, stream->hDib);
	GetWindowRect(pDIS->hwndItem, &rect);

	StretchBlt(pDIS->hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, 640, 480, SRCCOPY);
	DeleteDC(memDC);
}

HBITMAP MakeDib(RGBA** pvBits){
	BITMAPINFO bmi;
	HDC hdc;
	HBITMAP hBmp;

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 640;
	bmi.bmiHeader.biHeight = -480;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biCompression = BI_RGB;

	hdc = GetDC(NULL);
	hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void**)pvBits, NULL, NULL);
	ReleaseDC(NULL, hdc);

	return hBmp;
}

PStream CreateStream(int sz, PStream_OutputFrame pFnFrame, PStream_ProcCmd pFnCmd){
	PStream stream = malloc(sz);
	memset(stream, 0, sz);
	stream->hDib = MakeDib(&(stream->pBits));
	stream->OutputFrame = pFnFrame;
	stream->ProcCmd = pFnCmd;
	AddStream(stream);
	return stream;
}

void InitStreams(){
	memset(StreamList, 0, sizeof(StreamList));
}