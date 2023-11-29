#include "key.h"
#include <math.h>
#include "resource.h"
#include <stdio.h>
#include <time.h>

#define ColorDistance(a, b) sqrt((a.r - b.r) * (a.r - b.r) + (a.g - b.g) * (a.g - b.g) + (a.b - b.b) * (a.b - b.b))

int CK_OutputFrame(ChromaKeyStream* pStream){
    int i;

	if(pStream->fg == NULL || pStream->bg == NULL){
		memset(pStream->stream.pBits, 255, 640 * 480 * 4);
		return 0;
	}

	RGBTriplet* fgptr = (RGBTriplet*)pStream->fg->pBits;

    pStream->fg->OutputFrame(pStream->fg);
    pStream->bg->OutputFrame(pStream->bg);

    for(i = 0; i < 640*480; i++){
		/*if(ColorDistance((fgptr[i]), (pStream->keycolor)) <= pStream->max_distance){
            pStream->stream.pBits[i] = pStream->bg->pBits[i];
		}else{
            pStream->stream.pBits[i] = pStream->fg->pBits[i];
        }*/
    }

    return 1;
}


BOOL CALLBACK CK_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	ChromaKeyStream* stream = (ChromaKeyStream*)GetWindowLongPtrA(hwnd, DWLP_USER);
	PDRAWITEMSTRUCT pDis;
	RECT rect;
	HBRUSH brush;

	switch(msg){
		case WM_DRAWITEM:
			pDis = (PDRAWITEMSTRUCT)lParam;

			if(pDis->CtlID == IDC_CUSTOM1){
				DrawPreview((PStream)stream, pDis);			
			}else if(pDis->CtlID == IDC_CUSTOM2){
				rect.bottom = 0;
				rect.top = 200;
				rect.left = 0;
				rect.right = 100;

				brush = CreateSolidBrush(RGB(stream->keycolor.r, stream->keycolor.g, stream->keycolor.b));

				FillRect(pDis->hDC, &rect, brush);

				DeleteObject(brush);
			}
			return TRUE;
		case WM_INITDIALOG:
			SetWindowLongPtrA(hwnd, DWLP_USER, lParam);
			return TRUE;
		case WM_PAINT:
			SetWindowTextA(hwnd, stream->stream.name);
			return FALSE;
		case WM_COMMAND:

			return TRUE;
		case WM_NOTIFY:
			
			return TRUE;
		default:
			return FALSE;
	}
}

ChromaKeyStream* CK_OpenStream(){
	int a = 0;

	ChromaKeyStream* stream = (ChromaKeyStream*)malloc(sizeof(ChromaKeyStream));
	memset(stream, 0, sizeof(ChromaKeyStream));
	
	stream->stream.OutputFrame = (PStream_OutputFrame)CK_OutputFrame;
	stream->stream.hDib = MakeDib(&(stream->stream.pBits));

	stream->stream.hwnd = CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG5, NULL, CK_DlgProc, (LPARAM)stream);

	ShowWindow(stream->stream.hwnd, SW_SHOW);

	srand(time(NULL));

	return stream;
}