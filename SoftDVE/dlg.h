#ifndef __DLG_H_
#define __DLG_H_

#include <windows.h>
#include "stream.h"

#define DLG_BUTTON 0
#define DLG_TEXT 1
#define DLG_IMAGE 2
#define DLG_SLIDER 3

typedef struct tagDlgState{
	HWND hwnd;
	int id;
	int type;
	int callback;
	PStream stream; //used only for image controls
	struct tagDlgState* next;
} DlgState;

typedef void (*DlgEnumerator)(DlgState*);

DlgState* FindControlByHwnd(DlgState* root, HWND hwnd);
void AddControlItem(DlgState* root, HWND hwnd, int id, int type, int callback);
void EnumDlgControls(DlgState* root, DlgEnumerator callback);

#endif
