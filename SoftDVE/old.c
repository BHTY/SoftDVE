/*


// SoftDVE.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include "resource.h"
#include <windows.h>
#include <Commctrl.h>
#include "solid.h"

SolidColorStream* solid;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
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

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
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

HWND InitApp(){
	WNDCLASSA wc;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); //101
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "COLORPREVIEW";
	RegisterClassA(&wc);

	return CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG1, NULL, DlgProc, (LPARAM)solid);
}



int main(int argc, char* argv[])
{
	MSG msg;

	solid = (SolidColorStream*)malloc(sizeof(SolidColorStream));
	memset(solid, 0, sizeof(SolidColorStream));

	HWND hwnd = InitApp();
	printf("%p %d\n", hwnd, GetLastError());

	ShowWindow(hwnd, SW_SHOW);

	while(1){
		if(GetMessage(&msg, NULL, 0, 0)){
			if(!IsDialogMessage(hwnd, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}
}

*/