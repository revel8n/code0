#ifndef QS_DANGLEWINDOW_DANGLEGROUP_H
#define QS_DANGLEWINDOW_DANGLEGROUP_H

/*
Файл содержит определение класса вкладки - группы вкладок,
который используется для организации рабочего пространства.
*/

#include "dangle_window.h"
#include "dangle_group.h"

//виды панелей
#define DWDG_PANE_FLOAT									0
#define DWDG_PANE_FULL									1
#define DWDG_PANE_MAIN									2
#define DWDG_PANE_LEFT									3
#define DWDG_PANE_RIGHT									4

class CDangleWindowDangleGroup : public CDangleWindow
{
protected:
	//вложенная группа вкладок
	CDangleGroup* mpDangleGroup;
	//вид панели
	UINT mPane;
	//указатели на стандартные вкладки
	CDangleWindowDangleGroup* mpPaneMain;
	CDangleWindowDangleGroup* mpPaneLeft;
	CDangleWindowDangleGroup* mpPaneRight;

public:
	//конструктор и деструктор
	CDangleWindowDangleGroup(LPCTSTR szName,UINT Pane);
	virtual ~CDangleWindowDangleGroup();

	//оконная процедура
	virtual LRESULT DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//рисование
	virtual VOID Paint(HDC hdc);

	//получить вложенную группу вкладок
	CDangleGroup* GetGroup();

	//создать стандартные панели
	VOID CreateStandardPanes();
	//получить вкладку
	CDangleGroup* GetMainPane();
	CDangleGroup* GetLeftPane();
	CDangleGroup* GetRightPane();
};

#endif
