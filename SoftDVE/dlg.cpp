#include "dlg.h"

DlgState* FindControlByHwnd(DlgState* root, HWND hwnd){
	DlgState* cur;

	if(root == NULL || root->next == NULL) return NULL;

	cur = root->next;

	while(cur){
		if(cur->hwnd == hwnd) return cur;

		cur = cur->next;
	}

	return NULL;
}

void AddControlItem(DlgState* root, HWND hwnd, int id, int type, int callback){
	DlgState* old_next = root->next;
	root->next = malloc(sizeof(DlgState));
	root->next->next = old_next;
	root->next->hwnd = hwnd;
	root->next->id = id;
	root->next->type = type;
	root->next->callback = callback;
	root->next->stream = NULL;
}

void EnumDlgControls(DlgState* root, DlgEnumerator callback){
	DlgState* cur;

	if(root == NULL || root->next == NULL) return;

	cur = root->next;

	while(cur){
		callback(cur);

		cur = cur->next;
	}
}