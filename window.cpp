#include "window.h"

// class CWindow

CWindow::~CWindow()
{
}

LRESULT CALLBACK CWindow::StaticWndProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//объект
	CWindow* pWindow;
	//если сообщение - о создании
	if(Msg==WM_CREATE)
	{
		//запомнить объект
		pWindow=(CWindow*)((CREATESTRUCT*)lParam)->lpCreateParams;
		SetWindowLong(hWnd,GWL_USERDATA,(LONG)pWindow);
		//запомнить окно
		pWindow->mhWindow=hWnd;
	}
	else
		//получить объект
		pWindow=(CWindow*)GetWindowLong(hWnd,GWL_USERDATA);
	//если объект известен
	if(pWindow)
		//передать ему сообщение
		return pWindow->WindowProc(Msg,wParam,lParam);
	//иначе объект неизвестен; выполнить обработку по умолчанию
	return DefWindowProc(hWnd,Msg,wParam,lParam);
}

HWND CWindow::GetWindowHandle()
{
	//вернуть дескриптор окна
	return mhWindow;
}

VOID CWindow::PostMessage(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//отправить окну сообщение
	::PostMessage(mhWindow,Msg,wParam,lParam);
}
