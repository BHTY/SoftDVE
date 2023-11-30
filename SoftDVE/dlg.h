#ifndef __DLG_H_
#define __DLG_H_

#include <windows.h>
#include "stream.h"

typedef struct tagDlgState{
	HWND hwnd;
	int id;
	int type;
	int callback;
	PStream stream; //used only for image controls
	struct tagDlgState* next;
} DlgState;

//find control by hwnd

//add item

#endif
