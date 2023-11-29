#ifdef abcdefgh


#include <stdio.h>
#include "stream.h"
#include "resource.h"
#include "solid.h"
#include "fade.h"
#include <stdlib.h>
#include <mmsystem.h>
#include "bitmap.h"

int global_ready = 0;
HWND master_dlg = NULL;

/*
	Application "Topology"
Everything begins from the main window where you select the Output Stream. It has a preview. There is also a menu with
additional options.


OUTPUT				STREAMS									HELP
Set Framerate		Add Stream>		Stream types...			About
Exit				Remove Stream>	List of streams...
					Load DVE


Let's say you're creating a solid color stream. The "add stream" thing will call DLL!OpenStream to create a new stream
object. The operations performed are
1.) Allocate a SolidColorStream, set all fields to 0
2.) Set OutputFrame to the DLL's OutputFrame function
3.) Call CreateDIBSection to get an HBITMAP and a pointer to bits to stuff into the Stream header
4.) Create a dialog box using the HMODULE of the DLL, appropriate dialog ID and dialog procedure

The program maintains a list of open streams; the new stream is appended to that list.

The output framerate is used to set a timer interval. Every time the timer goes off, this initiates a rendering process.

We enumerate over all streams twice. The first time, we set a flag to invalidate the current bits. Then, for every 
stream, if it is invalidated, call OutputFrame and then InvalidateRect its dialog HWND. 


When the program opens, a solid-color stream is created.


Table to choose from of DVEs
*/

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
		SendMessage(pStream->hwnd, WM_PAINT, 0, 0);
		pStream->valid = 1;
		//printf("Doing invalidate rect!\n");
	}
}

void DoRender(){
	//printf("Rendering!\n");
	EnumStreams(InvalidateBits);
	EnumStreams(OutputStream);
	//InvalidateRect(master_dlg, NULL, 0);
	SendMessage(master_dlg, WM_PAINT, 0, 0);
}

void add_name(PStream pStream){
	SendDlgItemMessageA(master_dlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)pStream->name);
}

LRESULT CALLBACK PreviewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	PAINTSTRUCT ps;
	HDC memDC;
	int res;
	RECT rect;
	Stream* stream;

	switch(msg){
		case WM_PAINT:
			stream = (Stream*)GetWindowLongPtrA(GetParent(hwnd), DWLP_USER);
			GetWindowRect(hwnd, &rect);

			if(stream){
				hdc = BeginPaint(hwnd, &ps);
				memDC = CreateCompatibleDC(hdc);
				SelectObject(memDC, stream->hDib);
				res = StretchBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, 640, 480, SRCCOPY);
				//printf("%d\n", res);
				DeleteDC(memDC);
				EndPaint(hwnd, &ps);
			}
		default:
			return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
}

LRESULT CALLBACK MPreviewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	/*HDC hdc;
	PAINTSTRUCT ps;
	HDC memDC;
	int res;
	RECT rect;

	switch(msg){
		case WM_PAINT:
			GetWindowRect(hwnd, &rect);

			printf("Master Preview paint message!\n");
			hdc = BeginPaint(hwnd, &ps);
			memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, stream_selected->hDib);
			res = StretchBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, 640, 480, SRCCOPY);
			//printf("%d\n", res);
			DeleteDC(memDC);
			EndPaint(hwnd, &ps);
			break;
		default:
			return DefWindowProcA(hwnd, msg, wParam, lParam);
	}

	return 0;*/

	PAINTSTRUCT ps;

	switch(msg){
		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

int AddStream(PStream pStream){
	int i;

	for(i = 0; i < NUM_STREAMS; i++){
		if(StreamList[i] == 0){
			StreamList[i] = pStream;

			if(master_dlg){
				printf("Adding %s to list\n", pStream->name);
				add_name(pStream);
				//SendDlgItemMessageA(master_dlg, IDC_COMBO3, CB_ADDSTRING, 0, (LPARAM)pStream->name);
			}

			return 1;
		}
	}

	return 0;
}

int FindStreamByName(char* name){
	return 0;
}

void CALLBACK mmproc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
	global_ready = 1;
}

BOOL CALLBACK MasterDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	HMENU hMenu;
	PAINTSTRUCT ps;
	int i;

	RECT rect;
	HDC memDC;
	HDC hdc;
	HDC physDC;

	printf("Stuff IS happening here...\n");

	switch(msg){
		case WM_DRAWITEM:
			printf("Owner draw control requesting redraw!\n");
			return FALSE;

		case WM_INITDIALOG:
			while(stream_selected == NULL){
				printf("Waiting\n");
			}

			master_dlg = hwnd;
			EnumStreams(add_name);	

			SendDlgItemMessageA(hwnd, IDC_COMBO3, CB_SETCURSEL, lParam, 0);
			hMenu = LoadMenuA(GetModuleHandle(NULL), (LPCSTR)IDR_MENU1);
			SetMenu(hwnd, hMenu);
			global_ready = 1;
			timeSetEvent(33, 0, mmproc, 0, TIME_PERIODIC);
			return TRUE;
		case WM_PAINT:
			printf("Master WM_Paint message!\n");
			hdc = BeginPaint(hwnd, &ps);
			//InvalidateRect(GetDlgItem(hwnd, IDC_CUSTOM1), NULL, TRUE);
			physDC = GetDC(GetDlgItem(hwnd, IDC_CUSTOM1));
			GetWindowRect(GetDlgItem(hwnd, IDC_CUSTOM1), &rect);
			memDC = CreateCompatibleDC(hdc);
			SelectObject(memDC, stream_selected->hDib);
			StretchBlt(physDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top, memDC, 0, 0, 640, 480, SRCCOPY);
			DeleteDC(memDC);
			ReleaseDC(GetDlgItem(hwnd, IDC_CUSTOM1), physDC);

			EndPaint(hwnd, &ps);  
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



void MsgLoop(HWND hwnd){
	MSG msg;

	while(1){
		if(GetMessage(&msg, NULL, 0, 0)){
			//if(!IsDialogMessage(hwnd, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			//}
		}

		if(global_ready){
			DoRender();
			global_ready = 0;
		}
	}
}


void CreatePreviewClass(){
	WNDCLASSA wc;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)MPreviewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); //101
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MPREVIEW";
	RegisterClassA(&wc);

	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)PreviewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); //101
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "PREVIEW";
	RegisterClassA(&wc);
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
		printf("well shit (%p)\n", master_dlg);

		if(PeekMessageA(&msg, master_dlg, 0, 0, PM_REMOVE)){
			if(!IsDialogMessageA(master_dlg, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if(global_ready){
			//DoRender();
			global_ready = 0;
		}
	}
}

void SpawnMasterThread(){
	CreateThread(NULL, 0x10000, (LPTHREAD_START_ROUTINE)MasterMsgLoop, NULL, 0, NULL);
}

DWORD WINAPI SolidMsgLoop(LPVOID lpParam){
	MSG msg;
	SolidColorStream* stream = SCC_OpenStream();
	printf("%p %d\n", stream, GetLastError());
	stream->stream.name = (char*)lpParam;

	if(stream_selected == NULL){
		stream_selected = (PStream)stream;
		printf("Set selected stream!\n");
	}

	AddStream((PStream)stream);

	while(1){
		if(PeekMessageA(&msg, stream->stream.hwnd, 0, 0, PM_REMOVE)){
			//if(!IsDialogMessage(hwnd, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			//}
		}
	}
}

void SpawnSolidThread(char* name){
	CreateThread(NULL, 0x10000, (LPTHREAD_START_ROUTINE)SolidMsgLoop, name, 0, NULL);
}

void SoftDVE_Init(){
	CreatePreviewClass();
	SCC_Init();
	memset(StreamList, 0, sizeof(StreamList));

	timeBeginPeriod(1);
}

int main(){	
	SoftDVE_Init();

	SolidColorStream* stream = SCC_OpenStream();
	printf("%p %d\n", stream, GetLastError());
	stream->stream.name = (char*)malloc(50);
	sprintf(stream->stream.name, "Solid Color Stream 0");

	stream_selected = (PStream)stream;

	AddStream((PStream)stream);

	master_dlg = CreateMasterDialog();

	SolidColorStream* stream2 = SCC_OpenStream();
	printf("%p %d\n", stream2, GetLastError());
	stream2->stream.name = (char*)malloc(50);
	sprintf(stream2->stream.name, "Solid Color Stream 1");

	AddStream((PStream)stream2);

	DIBStream* ds = DS_OpenStream();
	printf("%p %d\n", ds, GetLastError());
	if(ds){
		ds->stream.name = (char*)malloc(50);
		sprintf(ds->stream.name, "Bitmap Stream 0");
		AddStream((PStream)ds);
	}

	FadeStream* fs = FS_OpenStream();
	printf("%p %d\n", fs, GetLastError());
	fs->stream.name = (char*)malloc(50);
	sprintf(fs->stream.name, "Fader Control 0");

	AddStream((PStream)fs);

	MsgLoop(master_dlg);


	/*SpawnMasterThread();
	SpawnSolidThread("Solid Color Stream 0");
	SpawnSolidThread("Solid Color Stream 1");

	while(1){
		DoRender();
		Sleep(10);
	}*/
}



#endif