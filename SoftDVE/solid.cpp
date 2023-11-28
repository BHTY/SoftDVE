#include "solid.h"
#include <stdlib.h>
#include "resource.h"
#include <commctrl.h>
#include <stdio.h>

void set_item_text(HWND hwnd, int index, int num){
	char buf[10];
	sprintf(buf, "%02X", num);
	SetDlgItemTextA(hwnd, index, buf);
}

void RedrawDialog(HWND hwnd){
	SolidColorStream* stream = (SolidColorStream*)GetWindowLongPtrA(hwnd, DWLP_USER);
	set_item_text(hwnd, IDC_EDIT1, stream->color.r);
	set_item_text(hwnd, IDC_EDIT2, stream->color.g);
	set_item_text(hwnd, IDC_EDIT3, stream->color.b);
	SendDlgItemMessageA(hwnd, IDC_SLIDER4, TBM_SETPOS, TRUE, (255 - stream->color.r) / 2.55);
	SendDlgItemMessageA(hwnd, IDC_SLIDER2, TBM_SETPOS, TRUE, (255 - stream->color.g) / 2.55);
	SendDlgItemMessageA(hwnd, IDC_SLIDER3, TBM_SETPOS, TRUE, (255 - stream->color.b) / 2.55);
	InvalidateRect(GetDlgItem(hwnd, IDC_CUSTOM2), NULL, TRUE);

	if(stream->stream.name){
		SetWindowTextA(hwnd, stream->stream.name);
	}
}

int get_slider_pos(HWND hwnd, int dlgItemID){
	return SendDlgItemMessageA(hwnd, dlgItemID, TBM_GETPOS, 0, 0);
}

int ceiling(int num, int maxnum){
	if(num > maxnum) return maxnum;
	return num;
}

void sync_with_sliders(HWND hwnd){
	SolidColorStream* stream = (SolidColorStream*)GetWindowLongPtrA(hwnd, DWLP_USER);

	stream->color.r = ceiling(2.55 * (100 - get_slider_pos(hwnd, IDC_SLIDER4)), 255);
	stream->color.g = ceiling(2.55 * (100 - get_slider_pos(hwnd, IDC_SLIDER2)), 255);
	stream->color.b = ceiling(2.55 * (100 - get_slider_pos(hwnd, IDC_SLIDER3)), 255);
}

BOOL CALLBACK SCC_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg){
		case WM_INITDIALOG:
			SetWindowLongPtrA(hwnd, DWLP_USER, lParam);
			return TRUE;
		case WM_PAINT:
			RedrawDialog(hwnd);
			return FALSE;
		case WM_COMMAND:
			//printf("WM_COMMAND!\n");
			return TRUE;
		case WM_NOTIFY:
			sync_with_sliders(hwnd);
			RedrawDialog(hwnd);
			return TRUE;
		default:
			return FALSE;
	}
}

LRESULT CALLBACK SCC_PreviewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	SolidColorStream* stream;
	PAINTSTRUCT ps;
	HDC hdc;
	HBRUSH brush;
	RECT rect;

	switch(msg){
		case WM_PAINT:
			stream = (SolidColorStream*)GetWindowLongPtrA(GetParent(hwnd), DWLP_USER);

			rect.bottom = 0;
			rect.top = 200;
			rect.left = 0;
			rect.right = 100;

			hdc = BeginPaint(hwnd, &ps);
			brush = CreateSolidBrush(RGB(stream->color.r, stream->color.g, stream->color.b));
			FillRect(hdc, &rect, brush);
			DeleteObject(brush);
			EndPaint(hwnd, &ps);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

int SCC_OutputFrame(SolidColorStream* pStream){
    int i;
    
    if (pStream == NULL) return 0;

    for(i = 0; i < 640*480; i++){
        pStream->stream.pBits[i] = *(RGBA*)&(pStream->color);
    }

    return 1;
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

SolidColorStream* SCC_OpenStream(){
	SolidColorStream* stream = (SolidColorStream*)malloc(sizeof(SolidColorStream));
	memset(stream, 0, sizeof(SolidColorStream));

	stream->stream.OutputFrame = (PStream_OutputFrame)SCC_OutputFrame;
	stream->stream.hDib = MakeDib(&(stream->stream.pBits));
	
	stream->stream.hwnd = CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG1, NULL, SCC_DlgProc, (LPARAM)stream);

	ShowWindow(stream->stream.hwnd, SW_SHOW);

	return stream;
}

void SCC_Init(){
	WNDCLASSA wc;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)SCC_PreviewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); //101
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "COLORPREVIEW";
	RegisterClassA(&wc);
}
