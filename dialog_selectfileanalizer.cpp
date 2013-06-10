#include "dialog_selectfileanalizer.h"
#include "application.h"
#include "resource.h"
#include <commctrl.h>

// class CListElement

class CDialogSelectFileAnalizer::CListElement : public CUserControlListElement
{
protected:
	//итератор на анализатор в списке анализаторов
	std::list<CFileParser*>::iterator mFileParserIterator;
	//анализатор
	CFileParser* mpFileParser;
	//структура информации об анализаторе
	FILE_PARSER_ABOUT mFileParserAbout;
	//необходимый объём памяти для анализатора
	DWORD mBufferSize;

public:
	//конструктор
	CListElement(std::list<CFileParser*>::iterator FileParserIterator)
	{
		//запомнить указатель на анализатор и итератор
		mFileParserIterator=FileParserIterator;
		mpFileParser=*mFileParserIterator;
		//получить информацию об анализаторе
		mpFileParser->About(&mFileParserAbout);
		//получить необходимый объём памяти
		mBufferSize=mpFileParser->GetNeededMemorySize();
	}

	virtual VOID Draw(HDC hdc,RECT* pRect)
	{
		//выбрать жирный шрифт
		SelectFont(hdc,gApplication.mhFontCodeBold);
		//напечатать имя анализатора
		TextOut(hdc,pRect->left+LIST_ELEMENT_MARGIN_HEADER,pRect->top,mFileParserAbout.szName,_tcslen(mFileParserAbout.szName));
		//выбрать обычный шрифт
		SelectFont(hdc,gApplication.mhFontCode);
		//напечатать версию
		TCHAR s[0x80];
		_stprintf(s,TEXT("Version: %d.%d"),HIWORD(mFileParserAbout.Version),LOWORD(mFileParserAbout.Version));
		TextOut(hdc,pRect->left+LIST_ELEMENT_MARGIN_TEXT,pRect->top+gApplication.mFontCodeHeight,s,_tcslen(s));
		//напечатать необходимый объём памяти
		_stprintf(s,TEXT("File memory size needed: %d.%02d Kb"),mBufferSize/1024,mBufferSize%1024*100/1024);
		TextOut(hdc,pRect->left+LIST_ELEMENT_MARGIN_TEXT,pRect->top+gApplication.mFontCodeHeight*2,s,_tcslen(s));
	}

	virtual UINT GetHeight()
	{
		//высота строки - как три обычные строки
		return gApplication.mFontCodeHeight*3;
	}

	BOOL IsAskSettingsAvailable()
	{
		//вернуть ответ
		return mpFileParser->IsAskSettingsAvaliable();
	}

	BOOL AskSettings(HWND hWndParent)
	{
		//вызвать диалог доплонительных настроек
		return mpFileParser->AskSettings(hWndParent);
	}

	VOID SetDescriptionText(HWND hWnd)
	{
		//указать текст - информацию об анализаторе
		SetWindowText(hWnd,mFileParserAbout.szDescription);
	}

	std::list<CFileParser*>::iterator GetIterator()
	{
		return mFileParserIterator;
	}
};

// class CDialogSelectFileAnalizer

BOOL CDialogSelectFileAnalizer::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:												//кнопка "Continue"
			//запомнить итератор выделенного анализатора
			mSelectedParserIterator=((CListElement*)mpList->GetSelectedElement())->GetIterator();
			EndDialog(mhWindow,TRUE);
			return TRUE;
		case IDCANCEL:										//кнопка "Cancel"
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_LIST_FILE_ANALIZERS:			//список анализаторов
			//если уведомление о том, что выделение в списке изменилось
			if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
			{
				//получить указатель на элемент списка
				CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
				//включить кнопку "Continue"
				EnableWindow(GetDlgItem(mhWindow,IDOK),TRUE);
				//показать надпись рядом с кнопкой "Continue"
				ShowWindow(GetDlgItem(mhWindow,IDC_STATIC_CONTINUE),SW_SHOW);
				//установить состояние кнопки "Customize"
				BOOL CustomizeAvailable=pElement->IsAskSettingsAvailable();
				EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_CUSTOMIZE),CustomizeAvailable);
				//указать текст для метки рядом с "Customize"
				SetDlgItemText(mhWindow,IDC_STATIC_CUSTOMIZE,CustomizeAvailable ? TEXT("To specify additional options for this analizer, click \"Customize...\" button.") : TEXT("This analizer is no have additional options."));
				//указать текст для метки описания анализатора и включить её
				HWND hDescriptionLabel=GetDlgItem(mhWindow,IDC_STATIC_DESCRIPTION);
				pElement->SetDescriptionText(hDescriptionLabel);
				EnableWindow(hDescriptionLabel,TRUE);
				return TRUE;
			}
			return FALSE;
		case IDC_BUTTON_CUSTOMIZE:				//кнопка "Customize"
			//выполнить диалог настроек
			((CListElement*)mpList->GetSelectedElement())->AskSettings(mhWindow);
			return TRUE;
		case IDC_BUTTON_UPDATEMEMORYINFORMATION:			//кнопка "Update"
			UpdateMemoryInformation();
			return TRUE;
		}
		return FALSE;
	case WM_INITDIALOG:
		{
			//создать список анализаторов
			mpList=new CUserControlList(IDC_LIST_FILE_ANALIZERS,GetDlgItem(mhWindow,IDC_LIST_FILE_ANALIZERS));
			//добавить в него анализаторы
			for(std::list<CFileParser*>::iterator i=mParsersList.begin();i!=mParsersList.end();i++)
				mpList->AddElement(new CListElement(i));

			//обновить информацию о памяти
			UpdateMemoryInformation();
		}
		return TRUE;
	case WM_DESTROY:
		//удалить список анализаторов
		SafeDelete(mpList);
		return TRUE;
	}
	//может, сообщение относится к списку
	if(mpList) return mpList->ProcessMessages(Msg,wParam,lParam);
	//иначе ничего не делать
	return FALSE;
}

VOID CDialogSelectFileAnalizer::UpdateMemoryInformation()
{
	//обновить информацию о памяти
	MEMORYSTATUS MemoryStatus;
	GlobalMemoryStatus(&MemoryStatus);
	//сформировать текст
	TCHAR s[0x200];
	_stprintf(s,TEXT("Total Physical Memory: %d Mb\nFree Physical Memory: %d Mb\nVirtual Address Space: %d Mb"),MemoryStatus.dwTotalPhys>>20,MemoryStatus.dwAvailPhys>>20,MemoryStatus.dwAvailVirtual>>20);
	SetDlgItemText(mhWindow,IDC_STATIC_MEMORYINFORMATION,s);
	//указать положение полосы
	HWND hProgressBar=GetDlgItem(mhWindow,IDC_PROGRESS_USINGMEMORY);
	SendMessage(hProgressBar,PBM_SETRANGE,0,MAKELPARAM(0,100));
	SendMessage(hProgressBar,PBM_SETPOS,MemoryStatus.dwMemoryLoad,0);
	//указать процент использования
	_stprintf(s,TEXT("%d%%"),MemoryStatus.dwMemoryLoad);
	SetDlgItemText(mhWindow,IDC_STATIC_MEMORYUSINGPERCENT,s);
}

CFileParser* CDialogSelectFileAnalizer::Show(HWND hWndParent,LPCTSTR szFileName,PVOID pFileMapping,DWORD FileSize)
{
	//получить список всех анализаторов файлов
	gApplication.GetFileParsers(&mParsersList);

	//удалить все анализаторы, которые не могут обработать файл
	std::list<CFileParser*> newList;
	std::list<CFileParser*>::iterator i;
	for(i=mParsersList.begin();i!=mParsersList.end();++i)
	{
		//получить анализатор
		CFileParser* pFileParser=*i;
		//указать анализатору проекцию загружаемого файла
		pFileParser->SetFile(pFileMapping,FileSize);
		//если анализатор может обработать этот файл
		if(pFileParser->PrepareAnalize())
			//добавить в новый список
			newList.push_back(pFileParser);
		else
			//удалить его
			pFileParser->Release();
	}
	//заменить список
	mParsersList=newList;

	//теперь в списке остались только анализаторы, которые могут обработать файл

	//инициализировать указатель на список анализаторов в диалоге
	mpList=NULL;
	//выбранный анализатор
	CFileParser* pFileParser;
	//показать диалог
	if(Ask(MAKEINTRESOURCE(IDD_DIALOG_SELECTFILEANALIZER),hWndParent))
	{
		//получить выбранный анализатор
		pFileParser=*mSelectedParserIterator;
		//удалить его из списка
		mParsersList.erase(mSelectedParserIterator);
	}
	else
		pFileParser=NULL;
	//удалить все анализаторы из списка
	for(i=mParsersList.begin();i!=mParsersList.end();i++)
		(*i)->Release();
	mParsersList.clear();

	//вернуть анализатор
	return pFileParser;
}
