#ifndef QS_DANGLE_GROUPER_H
#define QS_DANGLE_GROUPER_H

/*
‘айл содержит определение класса, выполн€ющего управление
несколькими группами вкладок DangleGroup.
*/

#include "dangle_group.h"
#include <list>

class CDangleGrouper : public CObject
{
protected:
	//список групп вкладок
	std::list<CDangleGroup*> mGroupsList;
	//текуща€ группа вкладок
	std::list<CDangleGroup*>::iterator mSelectedGroupIterator;
	//главное окно
	HWND mhWindow;

public:
	//конструктор и деструктор
	CDangleGrouper(HWND hWindow);
 	virtual ~CDangleGrouper();

	//процедура обработки сообщений
	LRESULT WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam);

	//получить текущую группу
	CDangleGroup* GetCurrentGroup();
};

#endif
