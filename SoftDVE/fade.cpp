#include "fade.h"
#include "resource.h"
#include <stdio.h>

#define WeightAvg(num0, num1, amnt)    (((num0 * amnt) + (num1 * (256 - amnt))) >> 8)

HWND sortHwnd = NULL;

void FS_AddListEntries(PStream pStream){
	SendDlgItemMessageA(sortHwnd, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)pStream->name);
	SendDlgItemMessageA(sortHwnd, IDC_COMBO4, CB_ADDSTRING, 0, (LPARAM)pStream->name);
}

int FS_OutputFrame(FadeStream* pStream){
    int i;

	if(!pStream->child0 || !pStream->child1) return 0;

	RGBTriplet* pBits = (RGBTriplet*)pStream->stream.pBits;
	RGBTriplet* child0bits = (RGBTriplet*)pStream->child0->pBits;
	RGBTriplet* child1bits = (RGBTriplet*)pStream->child1->pBits;

    pStream->child0->OutputFrame(pStream->child0);
    pStream->child1->OutputFrame(pStream->child1);

    for(i = 0; i < 640*480; i++){
		pBits[i].r = WeightAvg(child0bits[i].r, child1bits[i].r, pStream->fade_amount);
		pBits[i].g = WeightAvg(child0bits[i].g, child1bits[i].g, pStream->fade_amount);
		pBits[i].b = WeightAvg(child0bits[i].b, child1bits[i].b, pStream->fade_amount);
    }

    return 1;
}

BOOL CALLBACK FS_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	FadeStream* stream;

	switch(msg){
		case WM_INITDIALOG:
			sortHwnd = hwnd;

			EnumStreams(FS_AddListEntries);
			SetWindowLongPtrA(hwnd, DWLP_USER, lParam);
			
			SendDlgItemMessageA(hwnd, IDC_COMBO3, CB_SETCURSEL, lParam, 0);
			SendDlgItemMessageA(hwnd, IDC_COMBO4, CB_SETCURSEL, lParam, 0);

			((FadeStream*)(lParam))->child0 = StreamList[0];
			((FadeStream*)(lParam))->child1 = StreamList[1];

			sortHwnd = NULL;

			return TRUE;
		case WM_PAINT:
			//InvalidateRect(GetDlgItem(hwnd, IDC_CUSTOM1), NULL, TRUE);
			return FALSE;
		case WM_COMMAND:
			return TRUE;
		case WM_NOTIFY:
			stream = (FadeStream*)GetWindowLongPtrA(hwnd, DWLP_USER);

			stream->fade_amount = get_slider_pos(hwnd, IDC_SLIDER4) * 2.55;
			return TRUE;
		default:
			return FALSE;
	}
}

FadeStream* FS_OpenStream(){
	FadeStream* stream = (FadeStream*)malloc(sizeof(FadeStream));
	memset(stream, 0, sizeof(FadeStream));
	
	stream->stream.OutputFrame = (PStream_OutputFrame)FS_OutputFrame;
	stream->stream.hDib = MakeDib(&(stream->stream.pBits));

	stream->stream.hwnd = CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG3, NULL, FS_DlgProc, (LPARAM)stream);

	ShowWindow(stream->stream.hwnd, SW_SHOW);

	return stream;
}