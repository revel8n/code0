#include "dialog_splash.h"
#include "resource.h"

// class CSplashWindow

CSplashWindow::CSplashWindow()
{
	//инициализация
	mhWindow=NULL;
}

CSplashWindow::~CSplashWindow()
{
	Hide();
}

VOID CSplashWindow::Show(HWND hWndParent,LPCTSTR szText,LPCTSTR szCaption)
{
	//скрыть прежнее окно
	Hide();
	//запомнить параметры
	mszText=szText;
	mszCaption=szCaption;
	//создать новое окно
	mhWindow=CreateDialogParam(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG_SPLASH),hWndParent,StaticDlgProc,(LPARAM)this);
	ShowWindow(mhWindow,SW_NORMAL);
	UpdateWindow(mhWindow);
}

VOID CSplashWindow::Hide()
{
	//если окно есть, уничтожить его
	if(mhWindow)
	{
		DestroyWindow(mhWindow);
		mhWindow=NULL;
	}
}

BOOL CSplashWindow::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//указать текст сообщения и заголовок
		SetDlgItemText(mhWindow,IDC_STATIC_MESSAGE,mszText);
		SetWindowText(mhWindow,mszCaption);
		return TRUE;
	}
	return FALSE;
}
