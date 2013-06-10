#ifndef QS_DANGLE_GROUP_H
#define QS_DANGLE_GROUP_H

/*
Файл содержит класс CDangleGroup, управляющий набором
окон и объектов класса CDangleWindow.
Проще говоря, это окно с набором вкладок, под которым выводится содержимое выбранной вкладки.

Важно, что собственно окнами (HWND) управляет именно CDandleGroup, и именно он должен
указывать окна объектам CDandleWindow.
*/

#include "window.h"
#include "dangle_window.h"
#include <list>

//уведомления
#define NDG_CHANGEWINDOW						0x7777			//произошло переключение окон
//структура уведомления о переключении окон
struct NDG_CHANGEWINDOW_STRUCT
{
	NMHDR Header;
	HMENU hMenu;													//меню нового окна (NULL, если нет)
	HACCEL hAccelerators;									//таблица акселераторов нового окна
};
#define NDG_WINDOWUPDATED						0x7778			//вкладка обновилась, нужно обновить меню, таблицу акселераторов и т.д.
struct NDG_WINDOWUPDATED_STRUCT
{
	NMHDR Header;
	HWND hWindow;													//окно вкладки, чтобы его можно было найти
};

class CDangleGroup : public CWindow
{
protected:
	typedef std::list<CDangleWindow*> DGWL;
	typedef DGWL::iterator DGWLI;

	//панель переключения вкладок
	HWND mhTabbar;
	//флаг видимости панели вкладок
	BOOL mTabbarVisible;

	//список окон
	DGWL mWindowsList;
	//текущее выбранное окно
	DGWLI mSelectedWindowIterator;

protected:
	//обработчики сообщений
	VOID OnCreate(CREATESTRUCT* pCreateStruct);
	VOID OnSize(UINT Width,UINT Height);
	BOOL OnCommand(UINT CommandID,UINT Code,HWND hControl);
	VOID OnNotify(NMHDR* pNotify);

	//процедура обработки сообщений
	virtual LRESULT WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//конструктор и деструктор
	CDangleGroup(HWND hWndParent,RECT Rect,BOOL TabbarVisible=TRUE);
	~CDangleGroup();

	//добавить окно в группу
	VOID AddWindow(CDangleWindow* pWindow);
	//получить указатель на список окон
	DGWL* GetWindows();
	//закрыть текущее окно
	VOID CloseSelectedWindow();

	//получить текущее окно
	CDangleWindow* GetCurrentWindow();
};

#endif
