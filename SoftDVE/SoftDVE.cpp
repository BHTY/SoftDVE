#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include "dlg.h"
#include "solid.h"
#include "bitmap.h"
#include "gdicap.h"
#include <mmsystem.h>

BOOL CALLBACK drawProc(HWND hwnd, DlgState* pDLG){
	DRAWITEMSTRUCT dis;
	DlgState* pDlgItem;

	pDlgItem = FindControlByHwnd(pDLG, hwnd);

	if(pDlgItem && pDlgItem->type == DLG_IMAGE && pDlgItem->stream){
		dis.hDC = GetDC(hwnd);
		dis.hwndItem = hwnd;

		DrawPreview(pDlgItem->stream, &dis);

		ReleaseDC(hwnd, dis.hDC);
	}
}

LRESULT CALLBACK DialogWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	DlgState* pDLG = GetWindowLongPtrA(hwnd, 0);
	DlgState* pDlgItem;
	NMHDR* pNmhdr;
	LPDRAWITEMSTRUCT pDIS = lParam;
	HBRUSH brush;
	RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	LPCREATESTRUCTA pCS;

	switch(msg){

		case WM_CREATE:
			pCS = lParam;
			SetWindowLongPtrA(hwnd, 0, pCS->lpCreateParams);
			return DefWindowProcA(hwnd, msg, wParam, lParam);

		case WM_DRAWITEM:
			EnumChildWindows(hwnd, drawProc, pDLG);
			/*pDlgItem = FindControlByHwnd(pDLG, pDIS->hwndItem);

			if(pDlgItem){
				printf("Owner drawing control %d!\n", pDlgItem->id);

				if(pDlgItem->stream){
					DrawPreview(pDlgItem->stream, pDIS);
				} else{
					rect.bottom = 800;
					rect.top = 0;
					rect.right = 800;
					rect.left = 0;
					brush = CreateSolidBrush(0);
					FillRect(pDIS->hDC, &rect, brush);
					DeleteObject(brush);
				}
			}*/
			break;

		case WM_COMMAND:
			pDlgItem = FindControlByHwnd(pDLG, lParam);
			
			if(pDlgItem){
				printf("CTRL %d: Button Clicked!\n", pDlgItem->id);
			}

			break;

		case WM_NOTIFY:
			pNmhdr = lParam;
			pDlgItem = FindControlByHwnd(pDLG, pNmhdr->hwndFrom);

			if(pDlgItem){
				printf("CTRL %d: Slider moved to %d!\n", pDlgItem->id, SendMessage(pDlgItem->hwnd, TBM_GETPOS, 0, 0));
			}
			
			break;

		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			//EnumChildWindows(hwnd, drawProc, pDLG);
			EndPaint(hwnd, &ps);
			break;

		default:
			return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
}

void ld_string(FILE* fp, char* instr){
	int idx = -1;
	char c;

	while(1){
		c = fgetc(fp);

		if(c == '"'){
			if(idx == -1){
				idx = 0;
				continue;
			}else{
				break;
			}
		}

		if(idx != -1){
			instr[idx] = c;
			idx++;
		}
	}

	instr[idx] = 0;

}

typedef struct tagTextCreateStruct{
	int id;
	int x, y;
	int sx, sy;
	char caption[50];
} TextCreateStruct;

typedef struct tagButtonCreateStruct{
	int id;
	int x, y;
	int sx, sy;
	char caption[50];
	int style;
} ButtonCreateStruct;

typedef struct tagSliderCreateStruct{
	int id;
	int x, y;
	int sx, sy;
	int orientation;
	int peak;
} SliderCreateStruct;

typedef struct tagImageCreateStruct{
	int id;
	int x, y;
	int sx, sy;
} ImageCreateStruct;

void AddTextControl(HWND hwnd, TextCreateStruct* tcs){
	HWND hctrl = CreateWindowA("STATIC", tcs->caption, WS_CHILD | WS_VISIBLE | WS_GROUP, tcs->x, tcs->y, tcs->sx, tcs->sy, hwnd, NULL, GetModuleHandle(NULL), NULL);
	DlgState* pDLG = GetWindowLongPtrA(hwnd, 0);
	AddControlItem(pDLG, hctrl, tcs->id, DLG_TEXT, NULL);
}

void AddButtonControl(HWND hwnd, ButtonCreateStruct* bcs, int group){
	int grouping = 0;
	HWND hctrl;
	DlgState* pDLG = GetWindowLongPtrA(hwnd, 0);

	if(group) grouping = WS_GROUP | WS_TABSTOP;

	hctrl = CreateWindowA("BUTTON", bcs->caption, WS_VISIBLE | WS_CHILD | bcs->style | grouping, bcs->x, bcs->y, bcs->sx, bcs->sy, hwnd, NULL, GetModuleHandle(NULL), NULL);
	AddControlItem(pDLG, hctrl, bcs->id, DLG_BUTTON, NULL);
}

void AddImageControl(HWND hwnd, ImageCreateStruct* ics){
	DlgState* pDLG = GetWindowLongPtrA(hwnd, 0);
	HWND hctrl = CreateWindowA("STATIC", NULL, WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, ics->x, ics->y, ics->sx, ics->sy, hwnd, NULL, GetModuleHandle(NULL), NULL);
	AddControlItem(pDLG, hctrl, ics->id, DLG_IMAGE, NULL);
}

void AddSliderControl(HWND hwnd, SliderCreateStruct* scs){
	DlgState* pDLG = GetWindowLongPtrA(hwnd, 0);
	HWND hctrl = CreateWindowA(TRACKBAR_CLASSA, "Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_NOTICKS | TBS_BOTH | TBS_VERT | TBS_ENABLESELRANGE | TBS_FIXEDLENGTH, scs->x, scs->y, scs->sx, scs->sy, hwnd, NULL, GetModuleHandle(NULL), NULL);
	SendMessage(hctrl, TBM_SETRANGEMAX, 0, scs->peak);
	AddControlItem(pDLG, hctrl, scs->id, DLG_SLIDER, NULL);
}

#define DlgFileCmd(cmd) if(strcmp(lineHeader, cmd) == 0)

HWND LoadDialog(FILE* fp){
	DlgState* pDLG = malloc(sizeof(DlgState));

	TextCreateStruct tcs;
	ButtonCreateStruct bcs;
	ImageCreateStruct ics;
	SliderCreateStruct scs;

	HWND hwnd = NULL;
	char lineHeader[512];
	char instr[128];
	char c;

	int eof = 0;
	int res;
	int sX, sY;
	int idx = -1;

	int inside_group = 0;
	int just_started_group = 0;

	memset(pDLG, 0, sizeof(DlgState));

	while(!eof){
		res = fscanf(fp, "%s", lineHeader);
		if(res == -1){eof = -1; break;}

		DlgFileCmd("SIZE"){
			fscanf(fp, "%d %d\n", &sX, &sY);
			printf("Size %d x %d\n", sX, sY);
		} else DlgFileCmd("CAPTION"){
			ld_string(fp, instr);

			hwnd = CreateWindowA("VBDIALOG", instr, WS_OVERLAPPED | WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, sX, sY, NULL, NULL, GetModuleHandle(NULL), pDLG);
		} else DlgFileCmd("SCRIPT"){
			ld_string(fp, instr);
			printf("Loading script %s\n", instr);
		} else DlgFileCmd("TEXT"){
			fscanf(fp, "%d %d %d %d %d", &(tcs.id), &(tcs.x), &(tcs.y), &(tcs.sx), &(tcs.sy));
			ld_string(fp, tcs.caption);
			printf("Creating text control at (%d, %d) of size (%d, %d) with caption %s\n", tcs.x, tcs.y, tcs.sx, tcs.sy, tcs.caption);
			//while(1);
			AddTextControl(hwnd, &tcs);
		} else DlgFileCmd("BUTTON"){
			fscanf(fp, "%d %d %d %d %d", &(bcs.id), &(bcs.x), &(bcs.y), &(bcs.sx), &(bcs.sy));
			ld_string(fp, bcs.caption);
			fscanf(fp, "%d\n", &(bcs.style));
			printf("Creating button control at (%d, %d) of size (%d, %d) with caption %s and style %d\n", bcs.x, bcs.y, bcs.sx, bcs.sy, bcs.caption, bcs.style);
			AddButtonControl(hwnd, &bcs, (!inside_group) | just_started_group);
		} else DlgFileCmd("SLIDER"){
			fscanf(fp, "%d %d %d %d %d %d %d\n", &(scs.id), &(scs.x), &(scs.y), &(scs.sx), &(scs.sy), &(scs.orientation), &(scs.peak));
			printf("Creating slider control at (%d, %d) of size (%d, %d) [orientation %d and peak %d]\n", scs.x, scs.y, scs.sx, scs.sy, scs.orientation, scs.peak);
			AddSliderControl(hwnd, &scs);
		} else DlgFileCmd("IMAGE"){
			fscanf(fp, "%d %d %d %d %d\n", &(ics.id), &(ics.x), &(ics.y), &(ics.sx), &(ics.sy));
			printf("Creating image control at (%d, %d) of size (%d, %d)\n", ics.x, ics.y, ics.sx, ics.sy);
			AddImageControl(hwnd, &ics);
		} else DlgFileCmd("GROUP"){
			fscanf(fp, "\n");
			inside_group = 1;
			just_started_group = 1;
			continue;
		} else DlgFileCmd("ENDGROUP"){
			fscanf(fp, "\n");
			inside_group = 0;
		}

		just_started_group = 0;
	}

	if(hwnd){
		ShowWindow(hwnd, SW_SHOW);
	}

	return hwnd;
}

void InitDlgClass(){
	WNDCLASSA wc;
	wc.style = 0;
	wc.lpfnWndProc = (WNDPROC)DialogWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(DlgState*);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(1)); //101
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "VBDIALOG";
	RegisterClassA(&wc);
}

void SoftDVEInit(){
	InitDlgClass();
}

int global_ready = 0;

void CALLBACK mmproc(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2){
	global_ready = 1;
}

DWORD WINAPI RenderThread(HWND hwnd){
	while(1){
		if(global_ready){
			global_ready = 0;
			DoRender(hwnd);
		}
	}
}

void StartRenderThread(HWND hwnd){
	timeBeginPeriod(1);
	timeSetEvent(33, 0, mmproc, 0, TIME_PERIODIC);
	CreateThread(NULL, 0x10000, RenderThread, hwnd, 0, NULL);
}

int main(){
	SolidColorStream* matte;
	DIBStream* bitmap;
	GDIStream* gdicap;

	DlgState* pDLG;

	MSG msg;
	HWND hwnd;
	FILE* fp;

	SoftDVEInit();
	
	fp = fopen("form.dlg", "r");
	hwnd = LoadDialog(fp);
	fclose(fp);

	printf("HWND returned %p\n", hwnd);

	matte = SCC_OpenStream(RGB(0x00, 0xFF, 0x00));
	bitmap = DS_OpenStream("c:\\users\\will\\downloads\\star2.bmp");
	gdicap = GS_OpenStream("DS9 Season 1 Episode 3 Past Prologue.m4v - VLC media player");

	pDLG = GetWindowLongPtrA(hwnd, 0);
	FindControlById(pDLG, 13)->stream = gdicap;
	FindControlById(pDLG, 15)->stream = gdicap;
	FindControlById(pDLG, 17)->stream = gdicap;
	FindControlById(pDLG, 19)->stream = matte;
	FindControlById(pDLG, 21)->stream = bitmap;

	//StartRenderThread(hwnd);

	while(1){
		DoRender(hwnd);

		if(GetMessage(&msg, hwnd, 0, 0)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	while(1);
}