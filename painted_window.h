#ifndef QS_PAINTED_WINDOW_H
#define QS_PAINTED_WINDOW_H

/*
‘айл содержит определение класса-надстройки над классом окна CWindow,
управл€ющего рисованием без мерцани€ (с двойным буфером).
*/

#include "window.h"

class CPaintedWindow : public CWindow
{
protected:
	//перегружаема€ функци€ рисовани€
	virtual VOID Paint(HDC hdc) PURE;

	//функци€ обработки сообщений, перегруженна€ из CWindow
	virtual LRESULT WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);

	//перегружаема€ функци€ обработки сообщений
	virtual LRESULT PaintedWindowProc(UINT Msg,WPARAM wParam,LPARAM lParam) PURE;

public:
	//деструктор
	virtual ~CPaintedWindow();
};

#endif
