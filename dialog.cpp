#include "dialog.h"
#include "application.h"

// class CDialog

BOOL CALLBACK CDialog::StaticDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//указатель на объект-диалог
	CDialog* pDialog;
	//если это сообщение-инициализация
	if(Msg==WM_INITDIALOG)
	{
		//запомнить объект
		pDialog=(CDialog*)lParam;
		SetWindowLong(hWnd,GWL_USERDATA,(LONG)lParam);
		//запомнить окно диалога
		pDialog->mhWindow=hWnd;
	}
	else
    //получить объект
		pDialog=(CDialog*)GetWindowLong(hWnd,GWL_USERDATA);

	//обработка некоторых сообщений
	switch(Msg)
	{
	case WM_INITDIALOG:
		//сделать диалог полупрозрачным
		SetWindowLong(hWnd,GWL_EXSTYLE,GetWindowLong(hWnd,GWL_EXSTYLE) /*| WS_EX_TOOLWINDOW */| WS_EX_LAYERED);
		SetLayeredWindowAttributes(hWnd,0,255*90/100,LWA_ALPHA);
		//удалить команду закрытия окна
//		RemoveMenu(GetSystemMenu(hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND);
		break;
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORBTN:
//	case WM_CTLCOLOREDIT:
//	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORSCROLLBAR:
		SetBkColor((HDC)wParam,gApplication.mBrushSelectColor);
		return (BOOL)gApplication.mhBrushSelect;
	}

	//передать диалогу сообщение
	return pDialog ? pDialog->DlgProc(Msg,wParam,lParam) : FALSE;
}

INT_PTR CDialog::Ask(LPCTSTR szTemplate,HWND hWndParent)
{
	//вызвать диалог, передав ему в качестве параметра сам объект-диалог
	return DialogBoxParam(GetModuleHandle(NULL),szTemplate,hWndParent,StaticDlgProc,(LPARAM)this);
}
