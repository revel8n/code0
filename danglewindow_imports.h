#ifndef QS_DANGLEWINDOW_IMPORTS_H
#define QS_DANGLEWINDOW_IMPORTS_H

/*
Файл содержит определение класса окна списка импортируемых функций.
*/

#include "dangle_window.h"
#include <list>

//класс поддержки окна импорта
class CDangleWindowImports : public CDangleWindow
{
public:
	//прототип класса элемента списка импорта
	class CImportListItem;

protected:
	//список импортируемых функций
	std::list<CImportListItem*> mList;

public:
	//конструктор и деструктор
	CDangleWindowImports();
	virtual ~CDangleWindowImports();

	//оконная процедура
	virtual LRESULT DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//рисование
	virtual VOID Paint(HDC hdc);
};

#endif
