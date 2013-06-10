#include "dangle_group.h"
#include "application.h"
#include <commctrl.h>

// константы

//отступ по краям заголовков вкладок
#define DANGLE_GROUP_TAB_BORDER					10
//высота заголовка вкладки
#define DANGLE_GROUP_TAB_HEIGHT					20

// class CDangleGroup

VOID CDangleGroup::OnCreate(CREATESTRUCT* pCreateStruct)
{
	//создать панель инструментов
	RECT Rect;
	GetClientRect(mhWindow,&Rect);
	mhTabbar=CreateWindowEx(TBSTYLE_EX_MIXEDBUTTONS | WS_EX_WINDOWEDGE,TOOLBARCLASSNAME,TEXT("Quyse Toolbar"),WS_CHILD | TBSTYLE_LIST | TBSTYLE_FLAT | TBSTYLE_WRAPABLE | (mTabbarVisible ? WS_VISIBLE : 0),0,0,Rect.right,mTabbarVisible ? Rect.bottom : 0,mhWindow,NULL,GetModuleHandle(NULL),NULL);
	//отправить сообщение для инициализации панели
	SendMessage(mhTabbar,TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
}

VOID CDangleGroup::OnSize(UINT Width,UINT Height)
{
	///изменить размер панели вкладок
	MoveWindow(mhTabbar,0,0,Width,Height,TRUE);
	//получить размеры панели вкладок
	RECT Rect;
	GetWindowRect(mhTabbar,&Rect);
	UINT TabbarHeight=Rect.bottom-Rect.top;
	//изменить размеры всех вкладок
	for(DGWLI i=mWindowsList.begin();i!=mWindowsList.end();i++)
		MoveWindow((*i)->GetWindowHandle(),0,TabbarHeight,Width,Height-TabbarHeight,TRUE);
}

BOOL CDangleGroup::OnCommand(UINT CommandID,UINT Code,HWND hControl)
{
	//если сообщение поступило от панели вкладок
	if(hControl==mhTabbar)
	{
		//получить указатель на вкладку (идентификатор кнопки - номер окна)
		DGWLI i=mWindowsList.begin();
		std::advance(i,CommandID);
		CDangleWindow* pWindow=*i;
		//если вкладка должна переключиться, т.е. текущее окно не совпадает с выбранным
		if(mSelectedWindowIterator==mWindowsList.end() || pWindow!=*mSelectedWindowIterator)
		{
			//вывести окно выбранной вкладки
			SetWindowPos(pWindow->GetWindowHandle(),HWND_TOP,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

			//установить новое текущее окно
			mSelectedWindowIterator=i;

			//получить информацию о пользовательском интерфейсе вкладки
			DANGLE_WINDOW_UI WindowUI;
			pWindow->GetUI(&WindowUI);
			//отправить уведомление главному окну, чтобы оно обновило интерфейс
			NDG_CHANGEWINDOW_STRUCT Notify;
			Notify.Header.code=NDG_CHANGEWINDOW;
			Notify.Header.hwndFrom=mhWindow;
			Notify.Header.idFrom=0;
			Notify.hMenu=WindowUI.hMenu;
			Notify.hAccelerators=WindowUI.hAccelerators;
			::SendMessage(GetParent(mhWindow),WM_NOTIFY,0,(LPARAM)&Notify);
		}

		//сообщение обработано
		return TRUE;
	}
	//иначе сообщение не обработано
	return FALSE;
}

VOID CDangleGroup::OnNotify(NMHDR* pNotify)
{
	//выбор по типу уведомления
	switch(pNotify->code)
	{
	case NDG_WINDOWUPDATED:													//обновилась вкладка
		{
			//получить расширенную структуру
			NDG_WINDOWUPDATED_STRUCT* pStruct=(NDG_WINDOWUPDATED_STRUCT*)pNotify;
			//получить окно вкладки
			HWND hDangleWindow=pStruct->hWindow;

			//если окно не выделено, то закончить (группа автоматически обновится при переключении на это окно)
			if(!mWindowsList.size() || (*mSelectedWindowIterator)->GetWindowHandle()!=hDangleWindow) return;

			//иначе окно выделено

			//получить номер окна
			UINT j=0;
			for(DGWLI i=mWindowsList.begin();i!=mWindowsList.end();i++,j++)
				if(i==mSelectedWindowIterator)
					break;
			//установить текущее окно на неправильный итератор, чтобы было выполнено переключение
			mSelectedWindowIterator=mWindowsList.end();
			//отправить самому себе сообщение о нажатой кнопке
			SendMessage(mhWindow,WM_COMMAND,MAKELONG(j,0),(LPARAM)mhTabbar);
		}
		break;
	}
}

//оконная процедура
LRESULT CDangleGroup::WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_CREATE:
		return OnCreate((CREATESTRUCT*)lParam),0;
	case WM_SIZE:
		return OnSize(LOWORD(lParam),HIWORD(lParam)),0;
	case WM_COMMAND:
		if(OnCommand(LOWORD(wParam),HIWORD(wParam),(HWND)lParam))
			return 0;
		break;
	case WM_NOTIFY:
		return OnNotify((NMHDR*)lParam),0;
	}

	//теперь сообщения, которые не были обработаны, передать выделенной вкладке
	if(mWindowsList.size())
		switch(Msg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_COMMAND:
			(*mSelectedWindowIterator)->PostMessage(Msg,wParam,lParam);
		}
	//выполнить стандартную обработку
	return DefWindowProc(mhWindow,Msg,wParam,lParam);
}

CDangleGroup::CDangleGroup(HWND hWndParent,RECT Rect,BOOL TabbarVisible)
{
	//запомнить флаг видимости панели вкладок
	mTabbarVisible=TabbarVisible;
	//создать окно
	mhWindow=CreateWindow(gApplication.mszDangleGroupWindowClass,NULL,WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE,Rect.left,Rect.top,Rect.right-Rect.left,Rect.bottom-Rect.top,hWndParent,NULL,GetModuleHandle(NULL),this);
}

CDangleGroup::~CDangleGroup()
{
	//уничтожить все вкладки
	for(DGWLI i=mWindowsList.begin();i!=mWindowsList.end();i++)
	{
		(*i)->Delete();
		(*i)->Release();
	}
	mWindowsList.clear();
	//уничтожить окно
	DestroyWindow(mhWindow);
}

VOID CDangleGroup::AddWindow(CDangleWindow* pWindow)
{
	//создать окно
	RECT Rect,RectTabbar;
	GetClientRect(mhWindow,&Rect);
	GetWindowRect(mhTabbar,&RectTabbar);
	Rect.top+=RectTabbar.bottom-RectTabbar.top;
	pWindow->Create(mhWindow,Rect);
	//добавить окно в группу
	pWindow->AddRef();
	mWindowsList.push_back(pWindow);
	//установить текущее окно на неправильный итератор
	//это выполняется, чтобы в обработчике WM_COMMAND было сделано переключение окон
	//установка на правильный итератор будет выполнена в этом же обработчике
	mSelectedWindowIterator=mWindowsList.end();

	//добавить кнопку в панель вкладок
	TBBUTTON Button;
	Button.iBitmap=I_IMAGENONE;
	//идентификатор кнопки используется, как номер нужного окна
	Button.idCommand=mWindowsList.size()-1;
	Button.fsState=TBSTATE_ENABLED;
	Button.fsStyle=BTNS_SHOWTEXT | BTNS_AUTOSIZE | BTNS_CHECKGROUP;
	Button.dwData=0;
	Button.iString=(INT_PTR)pWindow->GetName();
	SendMessage(mhTabbar,TB_ADDBUTTONS,1,(LPARAM)&Button);

	//нажать кнопку
	SendMessage(mhTabbar,TB_CHECKBUTTON,Button.idCommand,(LPARAM)MAKELONG(TRUE,0));
	//отправить самому себе сообщение о нажатой кнопке
	SendMessage(mhWindow,WM_COMMAND,MAKELONG(Button.idCommand,0),(LPARAM)mhTabbar);
}

CDangleGroup::DGWL* CDangleGroup::GetWindows()
{
	//вернуть указатель на список окон
	return &mWindowsList;
}

VOID CDangleGroup::CloseSelectedWindow()
{
	//если окна есть
	if(mWindowsList.size())
	{
		//получить номер нужного окна (для того, чтобы можно было удалить кнопку по номеру)
		UINT j=0;
		for(DGWLI i=mWindowsList.begin();i!=mWindowsList.end();i++,j++)
			if(i==mSelectedWindowIterator)
				break;

		//получить указатель на окно
		CDangleWindow* pWindow=*mSelectedWindowIterator;
		//удалить и освободить окно
		pWindow->Delete();
		pWindow->Release();
		//удалить окно из списка
		mWindowsList.erase(mSelectedWindowIterator);
		//удалить соответствующую кнопку
		SendMessage(mhTabbar,TB_DELETEBUTTON,(WPARAM)j,0);

		//установить итератор текущего окна на неправильный
		mSelectedWindowIterator=mWindowsList.end();

		//уменьшить всем кнопкам, которые расположены после удалённой, идентификатор команды
		for(;j<mWindowsList.size();j++)
			SendMessage(mhTabbar,TB_SETCMDID,j,j);

		//если окна ешё есть
		if(mWindowsList.size())
		{
			//если это окно - не первое
			if(j)
				//выделить предыдущее окно
				j--;
			//иначе это окно было первым
			else;
				//выделить следующее окно (делать j++ не надо, так как он и так уже содержит номер теперь уже следующего окна)
			//нажать кнопку
			SendMessage(mhTabbar,TB_CHECKBUTTON,j,(LPARAM)MAKELONG(TRUE,0));
			//отправить самому себе сообщение о нажатой кнопке
			SendMessage(mhWindow,WM_COMMAND,MAKELONG(j,0),(LPARAM)mhTabbar);
		}
		//иначе окон больше нет
		else
		{
			//отправить уведомление главному окну, чтобы оно обновило интерфейс
			NDG_CHANGEWINDOW_STRUCT Notify;
			Notify.Header.code=NDG_CHANGEWINDOW;
			Notify.Header.hwndFrom=mhWindow;
			Notify.Header.idFrom=0;
			Notify.hMenu=NULL;
			Notify.hAccelerators=NULL;
			::SendMessage(GetParent(mhWindow),WM_NOTIFY,0,(LPARAM)&Notify);
		}
	}
}

CDangleWindow* CDangleGroup::GetCurrentWindow()
{
	//если окна есть
	if(mWindowsList.size())
		//вернуть окно
		return *mSelectedWindowIterator;
	//иначе окон нет
	return NULL;
}
