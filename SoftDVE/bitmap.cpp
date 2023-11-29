#include "bitmap.h"
#include "resource.h"
#include <stdio.h>
#include <commdlg.h>

void DS_NegateImage(RGBTriplet* bits){
	int i;

	for(i = 0; i < 640 * 480; i++){
		bits[i].r = 255 - bits[i].r;
		bits[i].g = 255 - bits[i].g;
		bits[i].b = 255 - bits[i].b;
	}
}

int DS_OutputFrame(DIBStream* pStream){
	HDC memDC, physDC;
	HWND hwnd = FindWindowA(NULL, "DS9 Season 1 Episode 3 Past Prologue.m4v - VLC media player");
	RECT rect;

	if(hwnd){
		GetWindowRect(hwnd, &rect);

		physDC = GetDC(hwnd);
		memDC = CreateCompatibleDC(GetDC(NULL));
		SelectObject(memDC, pStream->stream.hDib);
		//BitBlt(memDC, 0, 0, 640, 480, physDC, 0, 0, SRCCOPY);
		StretchBlt(memDC, 0, 0, 640, 480, physDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SRCCOPY);
		DeleteDC(memDC);
		ReleaseDC(hwnd, physDC);
		return 1;
	}

	return 0;
}

void DS_LoadFile(HWND hwnd){
	HDC memDC1;
	HDC memDC2;
	HBITMAP hBmp;
	FILE* fp;
	char strPath[MAX_PATH];
	OPENFILENAMEA ofn;
	DIBStream* stream = (DIBStream*)GetWindowLongPtrA(hwnd, DWLP_USER);

	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Device Independent Bitmaps\0*.*\0";
	ofn.lpstrFile = strPath;
	ofn.nMaxFile = MAX_PATH;

	strPath[0] = 0;

	if(GetOpenFileNameA(&ofn)){
		memDC1 = CreateCompatibleDC(GetDC(NULL));
		memDC2 = CreateCompatibleDC(GetDC(NULL));
		hBmp = (HBITMAP)LoadImageA(NULL, ofn.lpstrFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		SelectObject(memDC1, stream->stream.hDib);
		SelectObject(memDC2, hBmp);
		BitBlt(memDC1, 0, 0, 640, 480, memDC2, 0, 0, SRCCOPY);
		DeleteObject(memDC2);
		DeleteObject(memDC1);
		DeleteObject(hBmp);
	}
}

BOOL CALLBACK DS_DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DIBStream* stream = (DIBStream*)GetWindowLongPtrA(hwnd, DWLP_USER);

	switch(msg){
		case WM_INITDIALOG:
			SetWindowLongPtrA(hwnd, DWLP_USER, lParam);
			return TRUE;
		case WM_PAINT:
			SetWindowTextA(hwnd, stream->stream.name);
			return FALSE;
		case WM_DRAWITEM:
			DrawPreview((PStream)stream, (PDRAWITEMSTRUCT)lParam);
			return TRUE;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDC_BUTTON1){
				DS_LoadFile(hwnd);
			}
			if(LOWORD(wParam) == IDC_CHECK1){
				DS_NegateImage((RGBTriplet*)stream->stream.pBits);
			}

			return TRUE;
		case WM_NOTIFY:
			return TRUE;
		default:
			return FALSE;
	}
}

DIBStream* DS_OpenStream(){
	DIBStream* stream = (DIBStream*)malloc(sizeof(DIBStream));
	memset(stream, 0, sizeof(DIBStream));
	
	stream->stream.OutputFrame = (PStream_OutputFrame)DS_OutputFrame;
	stream->stream.hDib = MakeDib(&(stream->stream.pBits));

	stream->stream.hwnd = CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG4, NULL, DS_DlgProc, (LPARAM)stream);

	ShowWindow(stream->stream.hwnd, SW_SHOW);

	return stream;
}