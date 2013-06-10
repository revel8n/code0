#include "dangle_grouper.h"

// class CDangleGrouper

CDangleGrouper::CDangleGrouper(HWND hWindow)
{
	//запомнить дескриптор окна
	mhWindow=hWindow;
}

CDangleGrouper::~CDangleGrouper()
{
	//уничтожить все группы
	for(std::list<CDangleGroup*>::iterator i=mGroupsList.begin();i!=mGroupsList.end();i++)
		(*i)->Release();
	mGroupsList.clear();
}

LRESUTL CDangleGrouper::WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_SIZE:
		// Haven't done.
		return 0;
	}

	//все сообщения, которые не обработаны, передавать выделенной группе
	if(mGroupsList.size())
		switch(Msg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_COMMAND:
			(*mSelectedGroupIterator)->PostMessage(Msg,wParam,lParam);
		}
	//выполнить стандартную обработку
	return DefWindowProc(mhWindow,Msg,wParam,lParam);
}

CDangleGroup* CDangleGrouper::GetCurrentGroup()
{
	//если группы есть
	if(mGroupsList.size())
		//вернуть выделенную группу
		return *mSelectedGroupIterator;
	//иначе групп нет
	return NULL;
}
