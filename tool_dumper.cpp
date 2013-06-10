#include "windows.h"

#ifdef CODE0_COMPILE_DUMPING_TOOL

#include "tool_dumper.h"
#include "fileformat_dump.h"
#include "application.h"
#include "resource.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <list>

//класс диалога выбора процесса
class CToolDumper::CSelectProcessDialog : public CDialog
{
protected:
	//класс элемента списка
	class CListElement;
protected:
	//список процессов
	CUserControlList* mpList;
	//идентификатор выбранного процесса
	DWORD mProcessID;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//обновить список
	VOID UpdateList();

public:
	//показать диалог
	BOOL Show(HWND hWndParent,DWORD* pProcessID);
};

//класс элемента списка
class CToolDumper::CSelectProcessDialog::CListElement : public CUserControlListElement
{
public:
	//информация о процессе
	DWORD mProcessID;																	//идентификатор процесса
	DWORD mParentProcessID;														//идентификатор родительского процесса
	TCHAR mszBaseName[MAX_PATH];											//имя файла процесса
	TCHAR mszFullName[MAX_PATH];											//полное имя файла процесса

	TCHAR mszAccountDomainName[MAX_PATH];							//имя домена учётной записи
	TCHAR mszAccountName[MAX_PATH];										//имя учётной записи пользователя

	PROCESS_MEMORY_COUNTERS mMemoryInfo;							//информация об использовании памяти
	BOOL mMemoryInfoExist;														//присутствует ли информация о памяти

public:
	//конструктор
	CListElement(PROCESSENTRY32* pProcess);
	//нарисовать элемент
	virtual VOID Draw(HDC hdc,RECT* pRect);
	//получить высоту элемента
	virtual UINT GetHeight();
	//вывести информацию о процессе в окно
	VOID ViewInformation(HWND hWnd);
};

//диалоговая процедура
BOOL CToolDumper::CSelectProcessDialog::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//создать список процессов
		mpList=new CUserControlList(IDC_LIST_PROCESSES,GetDlgItem(mhWindow,IDC_LIST_PROCESSES));
		//обновить список
		UpdateList();
		return TRUE;
	case WM_CLOSE:
		//завершить диалог
		EndDialog(mhWindow,FALSE);
		return TRUE;
	case WM_DESTROY:
		//удалить список процессов
		SafeDelete(mpList);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_DUMP_PROCESS_MEMORY:	//кнопка "Dump"
			//запомнить идентификатор выбранного процесса
			mProcessID=((CListElement*)mpList->GetSelectedElement())->mProcessID;
			//завершить диалог
			EndDialog(mhWindow,TRUE);
			return TRUE;
		case IDCANCEL:								//кнопка "Cancel"
			//завершить диалог
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_BUTTON_UPDATE:				//кнопка "Update"
			//обновить список процессов
			UpdateList();
			return TRUE;
		case IDC_LIST_PROCESSES:			//список процессов
			//если уведомление о том, что выделение в списке изменилось
			if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
			{
				//получить указатель на элемент списка
				CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
				//вывести дополнительную информацию
				HWND hLabelInformation=GetDlgItem(mhWindow,IDC_STATIC_PROCESS_INFORMATION);
				pElement->ViewInformation(hLabelInformation);
				//включить метку
				EnableWindow(hLabelInformation,TRUE);
				//включить кнопку "Dump"
				EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_DUMP_PROCESS_MEMORY),TRUE);
				return TRUE;
			}
			return FALSE;
		}
		return FALSE;
	}
	//может, сообщение относится к списку
	if(mpList) return mpList->ProcessMessages(Msg,wParam,lParam);
	//иначе ничего не делать
	return FALSE;
}

//обновить список процессов
VOID CToolDumper::CSelectProcessDialog::UpdateList()
{
	//очистить список
	mpList->Clear();
	//указать текст для метки информации о процессе
	HWND hLabelInformation=GetDlgItem(mhWindow,IDC_STATIC_PROCESS_INFORMATION);
	SetWindowText(hLabelInformation,TEXT("(no process is being selected)"));
	//выключить метку
	EnableWindow(hLabelInformation,FALSE);
	//выключить кнопку "Dump"
	EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_DUMP_PROCESS_MEMORY),FALSE);

	//получить объект для перечисления процессов
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hSnapshot==INVALID_HANDLE_VALUE) return;

	//структура для перечисления процессов
	PROCESSENTRY32 Process;
	Process.dwSize=sizeof(PROCESSENTRY32);

	//получить первый процесс
	if(Process32First(hSnapshot,&Process))
		do
		{
			//добавить информацию о процессе в список
			mpList->AddElement(new CListElement(&Process));
		}
		while(Process32Next(hSnapshot,&Process));

	//закончить перечисление
	CloseHandle(hSnapshot);
}

//показать диалог
BOOL CToolDumper::CSelectProcessDialog::Show(HWND hWndParent,DWORD* pProcessID)
{
	//инициализировать указатель на список процессов
	mpList=NULL;
	//вывести диалог
	if(Ask(MAKEINTRESOURCE(IDD_DIALOG_TOOL_DUMPER_SELECT_PROCESS),hWndParent))
	{
		//вернуть идентификатор выбранного процесса
		*pProcessID=mProcessID;
		//диалог завершен успешно
		return TRUE;
	}
	//выбрана отмена
	return FALSE;
}

CToolDumper::CSelectProcessDialog::CListElement::CListElement(PROCESSENTRY32* pProcess)
{
	//запомнить информацию
	mProcessID=pProcess->th32ProcessID;
	mParentProcessID=pProcess->th32ParentProcessID;
	_tcscpy(mszBaseName,pProcess->szExeFile);

	//получить дополнительную информацию
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,mProcessID);
	//флаг успеха получения информации об учётной записи
	BOOL AccountResulted=FALSE;
	if(hProcess)
	{
		//получить полное имя файла процесса
		GetProcessImageFileName(hProcess,mszFullName,(MAX_PATH-1)*sizeof(TCHAR));
		mszFullName[MAX_PATH-1]=0;

		//получить информацию об использовании памяти процессом
		mMemoryInfoExist=GetProcessMemoryInfo(hProcess,&mMemoryInfo,sizeof(mMemoryInfo));

		//получить маркер защиты процесса
		HANDLE hProcessToken;
		if(OpenProcessToken(hProcess,TOKEN_QUERY,&hProcessToken))
		{
			//получить информацию о пользователе
			DWORD NeededSize;
			if(!GetTokenInformation(hProcessToken,TokenUser,NULL,0,&NeededSize))
			{
				TOKEN_USER* pUserInfo=(TOKEN_USER*)new BYTE[NeededSize];
				if(GetTokenInformation(hProcessToken,TokenUser,pUserInfo,NeededSize,&NeededSize))
				{
					//получить имя учётной записи, под которой работает процесс
					DWORD AccountNameLength=MAX_PATH;
					DWORD AccountDomainNameLength=MAX_PATH;
					SID_NAME_USE AccountUse;
					if(LookupAccountSid(NULL,pUserInfo->User.Sid,mszAccountName,&AccountNameLength,mszAccountDomainName,&AccountDomainNameLength,&AccountUse))
						//установить флаг результата
						AccountResulted=TRUE;
				}
				delete [] (BYTE*)pUserInfo;
			}

			CloseHandle(hProcessToken);
		}

		//закрыть описатель процесса
		CloseHandle(hProcess);
	}
	else
	{
		mszFullName[0]=0;
		mMemoryInfoExist=FALSE;
	}
	//если неудача с получением имени учётной записи, очистить строки
	if(!AccountResulted)
	{
		mszAccountName[0]=0;
		mszAccountDomainName[0]=0;
	}
}

//нарисовать элемент списка
VOID CToolDumper::CSelectProcessDialog::CListElement::Draw(HDC hdc,RECT* pRect)
{
	//напечатать жирным шрифтом идентификатор и имя процесса
	SelectFont(hdc,gApplication.mhFontCodeBold);
	static TCHAR s[MAX_PATH*2+0x30];
	_stprintf(s,TEXT("%s (%X)"),mszBaseName,mProcessID);
	TextOut(hdc,pRect->left+LIST_ELEMENT_MARGIN_HEADER,pRect->top,s,_tcslen(s));
	//далее печатать обычным шрифтом
	SelectFont(hdc,gApplication.mhFontCode);
	//напечатать полное имя файла процесса
	if(mszFullName[0])
		_stprintf(s,TEXT("File: \"%s\""),mszFullName);
	else
		_tcscpy(s,TEXT("File: (unknown)"));
	TextOut(hdc,pRect->left,pRect->top+gApplication.mFontCodeHeight,s,_tcslen(s));
	//напечатать информацию об учётной записи, если она есть
	if(mszAccountName[0])
		_stprintf(s,TEXT("User account: %s\\%s"),mszAccountDomainName,mszAccountName);
	else
		_tcscpy(s,TEXT("User account information: (unknown)"));
	TextOut(hdc,pRect->left,pRect->top+gApplication.mFontCodeHeight*2,s,_tcslen(s));
}
//получить высоту элемента
UINT CToolDumper::CSelectProcessDialog::CListElement::GetHeight()
{
	//вернуть высоту трёх строк
	return gApplication.mFontCodeHeight*3;
}
//вывести информацию о процессе в окно
VOID CToolDumper::CSelectProcessDialog::CListElement::ViewInformation(HWND hWnd)
{
	//сформировать информацию
	if(mMemoryInfoExist)
	{
		TCHAR s[0x400];
		_stprintf(s,TEXT("Page fault count:\t\t\t%Xh\nPeak working set size:\t\t\t%Xh\nCurrent working set size:\t\t%Xh\nPeak paged pool usage:\t\t\t%Xh\nCurrent paged pool usage:\t\t%Xh\nPeak nonpaged pool usage:\t\t%Xh\nCurrent nonpaged pool usage:\t\t%Xh\nPage file usage:\t\t\t\t%Xh\nPeak page file usage:\t\t\t%Xh"),
			mMemoryInfo.PageFaultCount,mMemoryInfo.PeakWorkingSetSize,mMemoryInfo.WorkingSetSize,mMemoryInfo.QuotaPeakPagedPoolUsage,mMemoryInfo.QuotaPagedPoolUsage,mMemoryInfo.QuotaPeakNonPagedPoolUsage,mMemoryInfo.QuotaNonPagedPoolUsage,mMemoryInfo.PagefileUsage,mMemoryInfo.PeakPagefileUsage
			);
		//указать информацию окну
		SetWindowText(hWnd,s);
	}
	else
		//указать, что информации нет
		SetWindowText(hWnd,TEXT("Information for selected process are not available."));
}

VOID CToolDumper::Start()
{
	//показать диалог
	CSelectProcessDialog Dialog;
	if(!Dialog.Show(gApplication.mhMainWindow,&mProcessID)) return;

	//открыть процесс
	HANDLE hProcess=OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE,mProcessID);
	if(!hProcess)
	{
		MessageBox(NULL,TEXT("Could not open handle of selected process.\nNo rights to perform the operation."),TEXT("Dumping process"),MB_ICONSTOP);
		return;
	}

	//запросить файл для сохранения дампа
	TCHAR szFile[MAX_PATH]={0};
	{
		OPENFILENAME ofn={sizeof(OPENFILENAME)};
		ofn.hwndOwner=gApplication.mhMainWindow;
		ofn.lpstrFile=szFile;
		ofn.nMaxFile=MAX_PATH;
		ofn.lpstrFilter=TEXT("Code0 Dump Files (*.dump)\0*.dump\0");
		ofn.lpstrTitle=TEXT("Select File to Save Dump File");
		ofn.Flags=OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST |OFN_HIDEREADONLY;
		if(!GetSaveFileName(&ofn))
		{
			//закрыть процесс и закончить
			CloseHandle(hProcess);
			return;
		}
	}

	//создать файл дампа
	HANDLE hFile=CreateFile(szFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		CloseHandle(hProcess);
		MessageBox(NULL,TEXT("Could not create file on specified location."),TEXT("Dumping process"),MB_ICONSTOP);
		return;
	}

	//получить список регионов и размеры памяти регионов
	std::list<FILE_DUMP_REGION> RegionsList;
	DWORD MemorySize=0;
	{
		//цикл по регионам
		PBYTE pRegion=0;
		do
		{
			//получить информацию о регионе
			MEMORY_BASIC_INFORMATION RegionInfo;
			if(!VirtualQueryEx(hProcess,pRegion,&RegionInfo,sizeof(RegionInfo))) break;

			//добавить регион в список
			FILE_DUMP_REGION Region;
			Region.Flags=0;
			Region.BaseAddress=(DWORD)pRegion;
			Region.Size=RegionInfo.RegionSize;
			Region.State=RegionInfo.State;
			Region.Protect=RegionInfo.Protect;
			Region.DataOffset=RegionInfo.State==MEM_COMMIT ? MemorySize : 0;
			RegionsList.push_back(Region);

			//увеличить размеры необходимой памяти, если память региона действительна
			if(RegionInfo.State==MEM_COMMIT)
				MemorySize+=RegionInfo.RegionSize;

			//перейти к следующему региону
			pRegion+=RegionInfo.RegionSize;
		}
		while(pRegion);
	}

	//получить список потоков
	std::list<FILE_DUMP_THREAD> ThreadsList;
	{
		//создать снимок процесса
		HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
		if(hSnapshot==INVALID_HANDLE_VALUE)
			MessageBox(NULL,TEXT("Thread list in dump is not available."),TEXT("Dumping process"),MB_ICONEXCLAMATION);
		else
		{
			//цикл по потокам
			THREADENTRY32 ThreadEntry;
			ThreadEntry.dwSize=sizeof(ThreadEntry);

			if(Thread32First(hSnapshot,&ThreadEntry))
				do
				{
					//если поток принадлежит нужному процессу (так как перечисляются все потоки)
					if(ThreadEntry.th32OwnerProcessID==mProcessID)
					{
						//добавить информацию о потоке в список
						FILE_DUMP_THREAD Thread;
						Thread.Flags=0;
						Thread.ThreadID=ThreadEntry.th32ThreadID;
						Thread.UsageCount=ThreadEntry.cntUsage;
						Thread.PriorityBase=ThreadEntry.tpBasePri;
						Thread.PriorityDelta=ThreadEntry.tpDeltaPri;
						ThreadsList.push_back(Thread);
					}
				}
				while(Thread32Next(hSnapshot,&ThreadEntry));
			//закрыть снимок процесса
			CloseHandle(hSnapshot);
		}
	}

	//получить список модулей
	std::list<FILE_DUMP_MODULE> ModulesList;
	{
		//создать снимок процесса
		HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,mProcessID);
		if(hSnapshot==INVALID_HANDLE_VALUE)
			MessageBox(NULL,TEXT("Modules list in dump is not available."),TEXT("Dumping process"),MB_ICONEXCLAMATION);
		else
		{
			//цикл по модулям
			MODULEENTRY32 ModuleEntry;
			ModuleEntry.dwSize=sizeof(ModuleEntry);

			if(Module32First(hSnapshot,&ModuleEntry))
				do
				{
					//добавить информацию о модуле в список
					FILE_DUMP_MODULE Module;
					ZeroMemory(&Module,sizeof(Module));
					Module.Flags=0;
					Module.BaseAddress=(DWORD)ModuleEntry.modBaseAddr;
					Module.BaseSize=ModuleEntry.modBaseSize;
					Module.UsageGlobalCount=ModuleEntry.GlblcntUsage;
					Module.UsageProcessCount=ModuleEntry.ProccntUsage;
					_tcscpy(Module.ModuleName,ModuleEntry.szModule);
					_tcscpy(Module.ModulePath,ModuleEntry.szExePath);
					ModulesList.push_back(Module);
				}
				while(Module32Next(hSnapshot,&ModuleEntry));
			//закрыть снимок процесса
			CloseHandle(hSnapshot);
		}
	}

	//сформировать и записать заголовок файла дампа
	DWORD Written;
	DWORD HeadersSize;
	{
		FILE_DUMP_HEADER Header;
		Header.Magics[0]=FILE_DUMP_MAGIC0;
		Header.Magics[1]=FILE_DUMP_MAGIC1;
		Header.RegionsCount=RegionsList.size();
		Header.ThreadsCount=ThreadsList.size();
		Header.ModulesCount=ModulesList.size();
		HeadersSize=sizeof(FILE_DUMP_HEADER)+Header.RegionsCount*sizeof(FILE_DUMP_REGION)+Header.ThreadsCount*sizeof(FILE_DUMP_THREAD)+Header.ModulesCount*sizeof(FILE_DUMP_MODULE);
		Header.FileSize=HeadersSize+MemorySize;
		WriteFile(hFile,&Header,sizeof(Header),&Written,NULL);
	}

	//записать список регионов, добавляя каждому к смещению размеры заголовков
	{
		for(std::list<FILE_DUMP_REGION>::iterator i=RegionsList.begin();i!=RegionsList.end();i++)
		{
			(*i).DataOffset+=HeadersSize;
			WriteFile(hFile,&*i,sizeof(FILE_DUMP_REGION),&Written,NULL);
		}
	}
	//записать список потоков
	{
		for(std::list<FILE_DUMP_THREAD>::iterator i=ThreadsList.begin();i!=ThreadsList.end();i++)
			WriteFile(hFile,&*i,sizeof(FILE_DUMP_THREAD),&Written,NULL);
	}
	//записать список модулей
	{
		for(std::list<FILE_DUMP_MODULE>::iterator i=ModulesList.begin();i!=ModulesList.end();i++)
			WriteFile(hFile,&*i,sizeof(FILE_DUMP_MODULE),&Written,NULL);
	}

	//записать память регионов
	{
		//цикл по регионам
		for(std::list<FILE_DUMP_REGION>::iterator i=RegionsList.begin();i!=RegionsList.end();i++)
		{
			//получить регион
			FILE_DUMP_REGION Region=*i;
			//если регион действителен
			if(Region.State==MEM_COMMIT)
			{
				//получить и записать память региона постранично
				PBYTE pEndAddress=(PBYTE)(Region.BaseAddress+Region.Size);
				static BYTE Buffer[0x1000];
				for(PBYTE pAddress=(PBYTE)Region.BaseAddress;pAddress<pEndAddress;pAddress+=0x1000)
				{
					ReadProcessMemory(hProcess,pAddress,Buffer,0x1000,NULL);
					WriteFile(hFile,Buffer,0x1000,&Written,NULL);
				}
			}
		}
	}

	//закрыть файл
	CloseHandle(hFile);

	//закрыть описатель процесса
	CloseHandle(hProcess);

	//сообщить об успешном завершении
	MessageBox(NULL,TEXT("Dumping process has been finished succesfully."),TEXT("Dumping process"),MB_ICONINFORMATION);
}

#endif
