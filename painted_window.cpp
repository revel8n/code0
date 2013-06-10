#include "painted_window.h"
#include "application.h"

// class CPaintedWindow

CPaintedWindow::~CPaintedWindow()
{
}

LRESULT CPaintedWindow::WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//если сообщение - рисование
	if(Msg==WM_PAINT)
	{
		//выполнить рисование
		Paint(gApplication.mhdcBackBuffer);
		//скопировать нарисованное на окно
		PAINTSTRUCT ps;
		HDC hdc=BeginPaint(mhWindow,&ps);
		BitBlt(hdc,ps.rcPaint.left,ps.rcPaint.top,ps.rcPaint.right-ps.rcPaint.left,ps.rcPaint.bottom-ps.rcPaint.top,gApplication.mhdcBackBuffer,ps.rcPaint.left,ps.rcPaint.top,SRCCOPY);
		EndPaint(mhWindow,&ps);

		//вернуть ответ
		return 0;
	}
	else
		//передать сообщение перегружаемой функции
		return PaintedWindowProc(Msg,wParam,lParam);
}
