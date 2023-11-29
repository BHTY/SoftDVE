#include <stdio.h>
#include "stream.h"
#include "resource.h"
#include "solid.h"
#include "fade.h"
#include <stdlib.h>
#include <mmsystem.h>
#include "bitmap.h"
#include "key.h"

HWND master_dlg = NULL;

PStream StreamList[NUM_STREAMS];
PStream stream_selected = NULL;

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

void OutputStream(PStream pStream){
	//if it's invalidated, call outputframe
	if(!pStream->valid){
		pStream->OutputFrame(pStream);
		//InvalidateRect(pStream->hwnd, NULL, 0);
		
		//SendMessage(pStream->hwnd, WM_PAINT, 0, 0);
		InvalidateRect(FindWindowExA(pStream->hwnd, NULL, "STATIC", NULL), NULL, 0);
		
		pStream->valid = 1;
		//printf("Doing invalidate rect!\n");
	}
}

void DoRender(){
	//printf("Rendering!\n");
	EnumStreams(InvalidateBits);
	EnumStreams(OutputStream);
	//InvalidateRect(master_dlg, NULL, 0);
	//SendMessage(master_dlg, WM_PAINT, 0, 0);
	//InvalidateRect(FindWindowExA(master_dlg, NULL, "STATIC", NULL), NULL, 0);
	InvalidateRect(GetDlgItem(master_dlg, IDC_CUSTOM1), NULL, 0);
}



void add_name(PStream pStream){
	SendDlgItemMessageA(master_dlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)pStream->name);
}

int AddStream(PStream pStream){
	int i;

	for(i = 0; i < NUM_STREAMS; i++){
		if(StreamList[i] == 0){
			StreamList[i] = pStream;

			if(master_dlg){
				printf("Adding %s to list\n", pStream->name);
				add_name(pStream);
				SendMessage(HWND_BROADCAST, WM_REFRESH_STREAMS, 0, 0);
				//SendDlgItemMessageA(master_dlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)pStream->name);
			}

			return 1;
		}
	}

	return 0;
}

void DrawPreview(PStream stream, PDRAWITEMSTRUCT pDis){
	RECT rect;
	HDC memDC = CreateCompatibleDC(GetDC(NULL));
	SelectObject(memDC, stream->hDib);
	GetWindowRect(pDis->hwndItem, &rect);
	StretchBlt(pDis->hDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, 640, 480, SRCCOPY);
	DeleteDC(memDC);
}

typedef struct tagStreamOpenStruct{
	char* title;
	POpenStream OpenStream;
	int done;
} StreamOpenStruct, *PStreamOpenStruct;

DWORD WINAPI StreamMsgLoop(PStreamOpenStruct pSOS){
	MSG msg;
	PStream stream = pSOS->OpenStream();
	stream->name = pSOS->title;

	printf("%p %p %d\n", stream, stream->hwnd, GetLastError());

	pSOS->done = 1;

	if(stream_selected == NULL){ //this is the first stream!
		stream_selected = stream;
	}

	AddStream(stream);

	while(1){
		if(PeekMessageA(&msg, stream->hwnd, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

HANDLE OpenStream(PStreamOpenStruct pSOS){
	HANDLE handle;
	pSOS->done = 0;
	handle = CreateThread(NULL, 0x10000, (LPTHREAD_START_ROUTINE)StreamMsgLoop, pSOS, 0, NULL);
	return handle;
}

BOOL CALLBACK MasterDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HMENU hMenu;
	int i;

	switch(msg){
		case WM_DRAWITEM:
			
			if(stream_selected){
				DrawPreview(stream_selected, (PDRAWITEMSTRUCT)lParam);
			}

			return FALSE;

		case WM_INITDIALOG:
			master_dlg = hwnd;

			hMenu = LoadMenuA(GetModuleHandle(NULL), (LPCSTR)IDR_MENU1);
			SetMenu(hwnd, hMenu);

			SendDlgItemMessageA(hwnd, IDC_COMBO3, CB_SETCURSEL, lParam, 0);
			
			return TRUE;
		case WM_PAINT:

			return FALSE;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDC_COMBO3 && HIWORD(wParam) == CBN_SELCHANGE){
				i = SendDlgItemMessageA(hwnd, IDC_COMBO3, CB_GETCURSEL, 0, 0);
				printf("Switching input to %d! (%p)\n", i, SendDlgItemMessageA(hwnd, IDC_COMBO3, CB_GETITEMDATA, i, 0));
				stream_selected = StreamList[i];
			}


			return TRUE;
		case WM_NOTIFY:
			return TRUE;
		default:
			return FALSE;
	}
}

HWND CreateMasterDialog(){
	HWND hwnd = CreateDialogParamA(GetModuleHandle(NULL), (LPCSTR)IDD_DIALOG2, NULL, MasterDlgProc, (LPARAM)0);
	ShowWindow(hwnd, SW_SHOW);
	return hwnd;
}

DWORD WINAPI MasterMsgLoop(LPVOID lpParam){
	MSG msg;

	master_dlg = CreateMasterDialog();

	printf("%p %d\n", master_dlg, GetLastError());

	while(1){
		if(PeekMessageA(&msg, master_dlg, 0, 0, PM_REMOVE)){
			//if(!IsDialogMessageA(master_dlg, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			//}
		}
	}
}


void SoftDVE_Init(){
	memset(StreamList, 0, sizeof(StreamList));
	timeBeginPeriod(1);
	CreateThread(NULL, 0x10000, (LPTHREAD_START_ROUTINE)MasterMsgLoop, NULL, 0, NULL);
}


void print_stream(PStream stream){
	printf("Stream Name: %s\n", stream->name);
}


int main(){
	SoftDVE_Init();

	StreamOpenStruct sos, sos2, sos3, sos4;

	sos.title = "Solid Color Stream 0";
	sos.OpenStream = (POpenStream)SCC_OpenStream;

	OpenStream(&sos);

	sos2.title = "Bitmap Stream 0";
	sos2.OpenStream = (POpenStream)DS_OpenStream;

	OpenStream(&sos2);

	sos3.title = "Fade Control 0";
	sos3.OpenStream = (POpenStream)FS_OpenStream;

	OpenStream(&sos3);

	sos4.title = "Chroma Key 0";
	sos4.OpenStream = (POpenStream)CK_OpenStream;

	OpenStream(&sos4);

	while(1){
		printf("%d %d %d %d\n", sos.done, sos2.done, sos3.done, sos4.done);
		if(sos.done || sos2.done || sos3.done, sos4.done){
			break;
		}
	}

	printf("About to print streams\n");
	EnumStreams(print_stream);
	printf("Done printing\n");

	while(1){
		DoRender();
		Sleep(10);
	}
}