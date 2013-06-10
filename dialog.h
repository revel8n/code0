#ifndef QS_DIALOG_H
#define QS_DIALOG_H

/*
Файл содержит определение стандартного класса диалога,
используемого для упрощения кодирования диалогов.
*/

#include "windows.h"

class CDialog
{
protected:
	//окно
	HWND mhWindow;

protected:
	//статическая оконная процедура
	static BOOL CALLBACK StaticDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
	//оконная процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam) PURE;

	//вызвать диалог
	INT_PTR Ask(LPCTSTR szTemplate,HWND hWndParent);
};

#endif
