#ifndef QS_DANGLE_WINDOW_H
#define QS_DANGLE_WINDOW_H

/*
Файл содержит определение класса "болтающееся окно",
который олицетворяет собой одно окно программы.

Производные классы должны перегружать функцию Paint,
которая досталась от класса CPaintedWindow.

Создание окна происходит при вызове Create(),
удаление - при удалении объекта.
*/

#include "painted_window.h"

//максимальная рекомендуемая длина имени окна
#define MAX_DANGLE_NAME_LENGTH			30

//структура информации об интерфейсе вкладки
struct DANGLE_WINDOW_UI
{
	HMENU hMenu;										//меню вкладки
	HACCEL hAccelerators;						//таблица акселераторов вкладки
};

class CDangleWindow : public CPaintedWindow
{
protected:
	//имя окна
	LPTSTR mszName;
	//информация об интерфейсе вкладки
	DANGLE_WINDOW_UI mUI;

protected:
	//рисование (остаётся нереализованным)
	virtual VOID Paint(HDC hdc) PURE;
	//оконная процедура
	virtual LRESULT PaintedWindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);

	//перегружаемая оконная процедура
	virtual LRESULT DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam) PURE;

public:
	//конструктор и деструктор
	CDangleWindow();
	virtual ~CDangleWindow();

	//получить имя окна
	LPCTSTR GetName();
	//получить информацию об интерфейсе
	VOID GetUI(DANGLE_WINDOW_UI* pUI);

	//создать окно
	VOID Create(HWND hWndParent,RECT Rect);
	//удалить окно
	VOID Delete();
};

#endif
