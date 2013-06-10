#include "danglewindow_danglegroup.h"

// class CDangleWindowDangleGroup

CDangleWindowDangleGroup::CDangleWindowDangleGroup(LPCTSTR szName,UINT Pane)
{
	//обнулить переменные
	mpDangleGroup=NULL;
	//указать имя
	mszName=(LPTSTR)szName;
	//запомнить тип панели
	mPane=Pane;
	//инициализировать указатели на панели
	mpPaneMain=NULL;
	mpPaneLeft=NULL;
	mpPaneRight=NULL;
}

CDangleWindowDangleGroup::~CDangleWindowDangleGroup()
{
	//освободить указатели на панели
	SafeRelease(mpPaneMain);
	SafeRelease(mpPaneLeft);
	SafeRelease(mpPaneRight);
	//удалить группу вкладок
	SafeRelease(mpDangleGroup);
}

LRESULT CDangleWindowDangleGroup::DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//некоторые сообщения обрабатываем
	switch(Msg)
	{
	case WM_CREATE:
		{
			//создать группу вкладок
			RECT Rect;
			GetClientRect(mhWindow,&Rect);
			mpDangleGroup=new CDangleGroup(mhWindow,Rect);
		}
		return 0;
	case WM_WINDOWPOSCHANGING:
		{
			//получить структуру с информацией
			WINDOWPOS* pWindowPos=(WINDOWPOS*)lParam;
			//получить размеры родительского окна
			RECT Rect;
			GetClientRect(GetParent(mhWindow),&Rect);
			//изменить новый размер в соответствии с типом панели
			switch(mPane)
			{
			case DWDG_PANE_FLOAT:
				//отменить перемещение и изменение размера
				pWindowPos->flags|=SWP_NOMOVE | SWP_NOSIZE;
				break;
			case DWDG_PANE_FULL:
				pWindowPos->cx=Rect.right;
				pWindowPos->cy=Rect.bottom;
				break;
			case DWDG_PANE_MAIN:
				pWindowPos->x=Rect.right/4;
				pWindowPos->cx=Rect.right*3/4;
				break;
			case DWDG_PANE_LEFT:
				pWindowPos->cx=Rect.right/4;
				break;
			case DWDG_PANE_RIGHT:
				pWindowPos->x=Rect.right*3/4;
				pWindowPos->cx=Rect.right/4;
				break;
			}
		}
		return 0;
	case WM_SIZE:
		//изменить размер группы вкладок
		if(mpDangleGroup)
			MoveWindow(mpDangleGroup->GetWindowHandle(),0,0,LOWORD(lParam),HIWORD(lParam),TRUE);
		return 0;
	case WM_NOTIFY:
		{
			//получить заголовок уведомления
			NMHDR* pNotify=(NMHDR*)lParam;
			//выбор по типу уведомления
			switch(pNotify->code)
			{
			case NDG_CHANGEWINDOW:					//во вложенной группе вкладок переключена вкладка
				{
					//получить расширенную структуру
					NDG_CHANGEWINDOW_STRUCT* pStruct=(NDG_CHANGEWINDOW_STRUCT*)pNotify;

					//запомнить параметры UI
					mUI.hMenu=pStruct->hMenu;
					mUI.hAccelerators=pStruct->hAccelerators;
					
					//отправить уведомление родительской группе вкладок об обновлении
					NDG_WINDOWUPDATED_STRUCT Notify;
					Notify.Header.code=NDG_WINDOWUPDATED;
					Notify.Header.hwndFrom=mhWindow;
					Notify.Header.idFrom=0;
					Notify.hWindow=mhWindow;
					::SendMessage(GetParent(mhWindow),WM_NOTIFY,0,(LPARAM)&Notify);
				}
				return 0;
			}
		}
	}
	//все остальные сообщения переадресовываем вложенной группе вкладок
	if(mpDangleGroup)
		switch(Msg)
		{
		//перенаправляемые сообщения
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_COMMAND:
			mpDangleGroup->PostMessage(Msg,wParam,lParam);
		}
	//выполнить стандартную обработку
	return DefWindowProc(mhWindow,Msg,wParam,lParam);
}

VOID CDangleWindowDangleGroup::Paint(HDC hdc)
{
	//ничего не рисовать
}

CDangleGroup* CDangleWindowDangleGroup::GetGroup()
{
	//вернуть группу
	mpDangleGroup->AddRef();
	return mpDangleGroup;
}

VOID CDangleWindowDangleGroup::CreateStandardPanes()
{
	//левая панель
	mpPaneLeft=new CDangleWindowDangleGroup(TEXT("LeftPane"),DWDG_PANE_LEFT);
	mpDangleGroup->AddWindow(mpPaneLeft);
	//правая панель
	mpPaneRight=new CDangleWindowDangleGroup(TEXT("RightPane"),DWDG_PANE_RIGHT);
	mpDangleGroup->AddWindow(mpPaneRight);
	//главная панель
	mpPaneMain=new CDangleWindowDangleGroup(TEXT("MainPane"),DWDG_PANE_MAIN);
	mpDangleGroup->AddWindow(mpPaneMain);
}

CDangleGroup* CDangleWindowDangleGroup::GetMainPane()
{
	//вернуть вкладку
	return mpPaneMain->GetGroup();
}

CDangleGroup* CDangleWindowDangleGroup::GetLeftPane()
{
	//вернуть вкладку
	return mpPaneLeft->GetGroup();
}

CDangleGroup* CDangleWindowDangleGroup::GetRightPane()
{
	//вернуть вкладку
	return mpPaneRight->GetGroup();
}
