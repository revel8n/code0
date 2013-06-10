#ifndef QS_MAIN_WINDOW_H
#define QS_MAIN_WINDOW_H

/*
‘айл содержит объ€вление класса CMainWindow,
управл€ющего главным окном.
*/

#include "window.h"
#include "dangle_group.h"

class CMainWindow : public CWindow
{
protected:
	//группа "болтающихс€" окон
	CDangleGroup* mpDangleGroup;

	//количество меню в исходном меню
	UINT mMainMenuItemsCount;
	//таблица акселераторов текущей вкладки
	HACCEL mhCurrentWindowAccelerators;

protected:
	//обработчики сообщений
	BOOL OnCreate(CREATESTRUCT* pCreateStruct);
	VOID OnClose();
	VOID OnDestroy();
	VOID OnSize(UINT Width,UINT Height);
	BOOL OnCommand(INT ControlID);
	VOID OnNotify(NMHDR* pNotify);
	//оконна€ функци€
	virtual LRESULT WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	CMainWindow();
	~CMainWindow();

	//главна€ функци€ программы
	VOID Show();
};

#endif
