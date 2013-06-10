#ifndef QS_WINDOW_H
#define QS_WINDOW_H

/*
‘айл содержит объ€вление класса, упрощающего управление окнами.
*/

#include "object.h"

class CWindow : public CObject
{
protected:
	//дескриптор окна
	HWND mhWindow;

public:
	//виртуальный деструктор
	virtual ~CWindow();

	//оконна€ процедура
	static LRESULT CALLBACK StaticWndProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);

	//получить окно
	HWND GetWindowHandle();
	//отправить окну сообщение
	VOID PostMessage(UINT Msg,WPARAM wParam,LPARAM lParam);

protected:
	//перегружаема€ оконна€ процедура
	virtual LRESULT WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam) PURE;
};

#endif
