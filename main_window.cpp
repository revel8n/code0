#include "main_window.h"
#include "application.h"
#include "danglewindow_code.h"
#include "danglewindow_hex.h"
#include "danglewindow_danglegroup.h"
#include "dialog_about.h"
#include "dialog_splash.h"
#include "tool_dumper.h"
#include "resource.h"
#include <commctrl.h>

//количество элементов в главном меню с правого края
#define MAIN_MENU_RIGHT_ITEMS_COUNT 3

// class CMainWindow

BOOL CMainWindow::OnCreate(CREATESTRUCT*)
{
	//создать главную группу вкладок
	RECT Rect;
	GetClientRect(mhWindow,&Rect);
	mpDangleGroup=new CDangleGroup(mhWindow,Rect);

	//указать объекту Application главное окно
	gApplication.mhMainWindow=mhWindow;

	return TRUE;
}

VOID CMainWindow::OnClose()
{
	//удалить группу вкладок
	if(mpDangleGroup)
	{
		CDangleGroup* pGroup=mpDangleGroup;
		mpDangleGroup=NULL;
		pGroup->Release();
	}
	//закрыть окно
	DestroyWindow(mhWindow);
}

VOID CMainWindow::OnDestroy()
{
	//отправить сообщение для окончания работы
	PostQuitMessage(0);
}

VOID CMainWindow::OnSize(UINT Width,UINT Height)
{
	//изменить размер окна вкладок
	if(mpDangleGroup)
		MoveWindow(mpDangleGroup->GetWindowHandle(),0,0,Width,Height,TRUE);
}

BOOL CMainWindow::OnCommand(INT ControlID)
{
	//выбор по идентификатору
	switch(ControlID)
	{
		//** меню "File"
	case MID_MAIN_FILE_OPENIMAGE:
		{
			//вывести окно выбора файла
			TCHAR szFile[MAX_PATH]={0};
			OPENFILENAME ofn={sizeof(OPENFILENAME)};
			ofn.hwndOwner=mhWindow;
			ofn.lpstrFile=szFile;
			ofn.nMaxFile=MAX_PATH;
			ofn.lpstrFilter=TEXT("All supported files\0*.exe;*.dll;*.com;*.dump\0Executable images (*.exe)\0*.exe\0Dynamic-link libraries (*.dll)\0*.dll\0MS-DOS .COM files (*.com)\0*.com\0Code0 dump files (*.dump)\0*.dump\0All files (*.*)\0*.*\0");
			ofn.lpstrTitle=TEXT("Select File to Disassembly");
			ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			if(!GetOpenFileName(&ofn)) break;

			//загрузить файл
			CExploredImage* pImage=new CExploredImage(szFile);
			if(pImage->IsFileOpened())
			{
				//выбрать и настроить анализатор файла
				CFileParser* pFileParser=pImage->AskFileParser(mhWindow);
				if(pFileParser)
				{
					//установить сплэш-экран
					CSplashWindow SplashWindow;
					SplashWindow.Show(mhWindow,TEXT("Analizing your file..."),TEXT("Opening"));
					//выполнить анализ и вывести сообщение о неудаче анализа, если нужно
					switch(pImage->AnalizeAndPresent(pFileParser,mpDangleGroup))
					{
					case FILE_PARSER_ERROR_SUCCESS:
						//всё в порядке
						break;
					case FILE_PARSER_ERROR_LOADING:
						MessageBox(mhWindow,TEXT("File analizer reported about permanent errors.\nFile can't be loaded with this analizer."),TEXT("Analizing error"),MB_ICONSTOP);
						break;
					case FILE_PARSER_ERROR_ANALIZING:
						MessageBox(mhWindow,TEXT("File analizer reported about availability some errors.\nIt may be mean auto-analizing has failed."),TEXT("Analizing warning"),MB_ICONWARNING);
						break;
					default:
						_ASSERTE(("Unknown file parser error code.",TRUE));
						break;
					}
					//освободить анализатор
					pFileParser->Release();
					//убрать сплэш-экран
					SplashWindow.Hide();
				}
			}
			else
				//вывести сообщение
				MessageBox(NULL,TEXT("Coudn't open this file.\nThis is maybe not enough memory, not enough address space or network problem."),TEXT("Opening executable image"),MB_ICONSTOP);
			//освободить файл
			pImage->Release();
		}
		break;
	case MID_MAIN_FILE_OPENHEX:
		{
			//вывести окно выбора файла
			TCHAR szFile[MAX_PATH]={0};
			OPENFILENAME ofn={sizeof(OPENFILENAME)};
			ofn.hwndOwner=mhWindow;
			ofn.lpstrFile=szFile;
			ofn.nMaxFile=MAX_PATH;
			ofn.lpstrFilter=TEXT("All files\0*.*\0");
			ofn.lpstrTitle=TEXT("Select File to HEX edit");
			ofn.Flags=OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			if(!GetOpenFileName(&ofn)) break;

			//загрузить файл
			CEditedFile* pFile=CEditedFile::Open(szFile);
			if(pFile)
			{
				//вывести файл на экран
				CDangleWindowHex* pWindow=new CDangleWindowHex(pFile);
				pFile->Release();
				mpDangleGroup->AddWindow(pWindow);
				pWindow->Release();
			}
			else
				MessageBox(mhWindow,TEXT("File could not be opened."),TEXT("Opening file in HEX"),MB_ICONSTOP);
		}
		break;
	case MID_MAIN_FILE_QUIT:
		//отправить сообщение о закрытии окна, чтобы корректно завершиться
		PostMessage(WM_CLOSE,0,0);
		break;
		//** меню "Tools"
	case MID_MAIN_TOOLS_DUMPPROCESSMEMORY:
		{
#ifdef CODE0_COMPILE_DUMPING_TOOL
			//запустить инструмент
			CToolDumper Tool;
			Tool.Start();
#else
			//вывести сообщение
			MessageBox(mhWindow,TEXT("Dumping tool in this version is not supported."),TEXT("Dumping tool"),MB_ICONSTOP);
#endif
		}
		break;
		//** меню "Window"
	case MID_MAIN_WINDOW_CLOSE:
		{
			//установить сплэш-экран, так как при закрытии больших файлов окна закрываются долго
			CSplashWindow SplashWindow;
			SplashWindow.Show(mhWindow,TEXT("Releasing resources..."),TEXT("Shutdown"));
			//закрыть выделенное окно
			mpDangleGroup->CloseSelectedWindow();
			//убрать сплэш-экран
			SplashWindow.Hide();
		}
		break;
		//** меню "Help"
	case MID_MAIN_HELP_ABOUT:								//о программе
		{
			//вывести диалог
			CDialogAbout Dialog;
			Dialog.Show(mhWindow);
		}
		break;
	default:			//все неизвестные сообщения не обрабатывать
		return FALSE;
	}

	//сообщение обработано
	return TRUE;
}

VOID CMainWindow::OnNotify(NMHDR* pNotify)
{
	//выбор по типу сообщения
	switch(pNotify->code)
	{
	case NDG_CHANGEWINDOW:					//переключена вкладка
		{
			//получить расширенную структуру
			NDG_CHANGEWINDOW_STRUCT* pStruct=(NDG_CHANGEWINDOW_STRUCT*)pNotify;

			//запомнить таблицу акселераторов
			mhCurrentWindowAccelerators=pStruct->hAccelerators;

			//модифицировать меню
			{
				//получить меню окна
				HMENU hMainMenu=GetMenu(mhWindow);
				//получить текущее количество меню
				UINT MainMenuItemsCount=GetMenuItemCount(hMainMenu);
				//получить количество меню, которое нужно удалить
				UINT ItemsToDeleteCount=MainMenuItemsCount-mMainMenuItemsCount;
				//цикл по этим элементам
				for(UINT i=0;i<ItemsToDeleteCount;++i)
					//удалить элемент (без удаления соответствующего подменю, в отличие от DeleteMenu)
					RemoveMenu(hMainMenu,mMainMenuItemsCount-MAIN_MENU_RIGHT_ITEMS_COUNT,MF_BYPOSITION);

				//получить меню для вставки
				HMENU hMenu=pStruct->hMenu;
				//если оно есть
				if(hMenu)
				{
					//получить количество элементов в новом меню
					UINT MenuItemsCount=GetMenuItemCount(hMenu);
					//вставить их в главное меню
					for(UINT i=0;i<MenuItemsCount;++i)
					{
						//получить информацию об элементе меню (максимальная поддерживаемая длина строки меню ограничена)
						MENUITEMINFO ItemInfo;
						ZeroMemory(&ItemInfo,sizeof(ItemInfo));
						ItemInfo.cbSize=sizeof(ItemInfo);
						ItemInfo.fMask=MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
						static TCHAR szMenuItemName[0x100];
						ItemInfo.cch=0x100-1;
						ItemInfo.dwTypeData=szMenuItemName;
						INT k=GetMenuItemInfo(hMenu,i,TRUE,&ItemInfo);
						//вставить элемент
						//здесь указано -1, так как требуется передавать идентификатор элемента,
						//перед которым требуется вставить элемент
						k=InsertMenuItem(hMainMenu,mMainMenuItemsCount-MAIN_MENU_RIGHT_ITEMS_COUNT+i,TRUE,&ItemInfo);
					}
				}

				//перерисовать строку меню
				DrawMenuBar(mhWindow);
			}

			//установить новое меню
		}
		break;
	}
}

LRESULT CMainWindow::WindowProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_CREATE:
		return OnCreate((CREATESTRUCT*)lParam) ? 0 : -1;
	case WM_CLOSE:
		return OnClose(),0;
	case WM_DESTROY:
		return OnDestroy(),0;
	case WM_SIZE:
		return OnSize(LOWORD(lParam),HIWORD(lParam)),0;
	case WM_COMMAND:
		if(OnCommand(LOWORD(wParam)))
			return 0;
		break;
	case WM_NOTIFY:
		return OnNotify((NMHDR*)lParam),0;
	}
	//теперь сообщения, которые не обработаны, перенаправить группе вкладок
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

	//для всех не обработанных сообщений (в том числе перенаправленных) выполнить стандартную обработку
	return DefWindowProc(mhWindow,Msg,wParam,lParam);
}

CMainWindow::CMainWindow()
{
	//инициализировать переменные
	HMENU hMainMenu=LoadMenu(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MENU_MAIN));
	mMainMenuItemsCount=GetMenuItemCount(hMainMenu);
	mhCurrentWindowAccelerators=NULL;

	//создать окно
	mhWindow=CreateWindowEx(WS_EX_APPWINDOW,gApplication.mszMainWindowClass,TEXT("Quyse Code0 Disassembly"),WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,0,0,500,400,NULL,hMainMenu,GetModuleHandle(NULL),this);
	ShowWindow(mhWindow,SW_MAXIMIZE);
}

CMainWindow::~CMainWindow()
{
}

//главная функция программы
VOID CMainWindow::Show()
{
	//загрузить таблицу акселераторов
	HACCEL hAccelerators=LoadAccelerators(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_ACCELERATOR_MAIN));
	//главный цикл
	MSG Msg;
	//получить сообщение
	while(GetMessage(&Msg,NULL,0,0))
	{
		//если есть таблица акселераторов выделенной вкладки
		if(mhCurrentWindowAccelerators)
			//проверить, перехватывает ли она сообщение
			if(TranslateAccelerator(mhWindow,mhCurrentWindowAccelerators,&Msg))
				continue;
		//теперь проверить по глобальной таблице акселераторов
		if(TranslateAccelerator(mhWindow,hAccelerators,&Msg))
			continue;

		//таблицы акселераторов не перехватывают сообщение; передать его на обработку
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}
