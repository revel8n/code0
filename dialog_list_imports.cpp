#include "dialog_list_imports.h"
#include "image_extend_object.h"
#include "dialog_gotoxreference.h"
#include "application.h"
#include "strings.h"
#include "resource.h"

// subclass CDialogListImports::CListElement

CDialogListImports::CListElement::CListElement(CImageListImports::CImportLibrary* pLibrary)
{
	//установить параметры
	mpLibrary=pLibrary;
	mpLibrary->AddRef();
	mIsLibrary=TRUE;
}

CDialogListImports::CListElement::CListElement(CImageListImports::CImportFunction* pFunction)
{
	//установить параметры
	mpFunction=pFunction;
	mpFunction->AddRef();
	mIsLibrary=FALSE;
}

CDialogListImports::CListElement::~CListElement()
{
	//освободить нужный объект
	if(mIsLibrary)
		mpLibrary->Release();
	else
		mpFunction->Release();
}

VOID CDialogListImports::CListElement::Draw(HDC hdc,RECT* pRect)
{
	//напечатать имя библиотеки/функции
	if(mIsLibrary)
	{
		SelectFont(hdc,gApplication.mhFontCodeBold);
		//получить имя
		LPCTSTR szName=mpLibrary->GetName();
		TextOut(hdc,pRect->left+16,pRect->top,szName,_tcslen(szName));
	}
	else
	{
		SelectFont(hdc,gApplication.mhFontCode);
		//получить и напечатать имя
		LPCTSTR szName=mpFunction->GetName();
		TextOut(hdc,pRect->left+32,pRect->top,szName,_tcslen(szName));
	}
}

UINT CDialogListImports::CListElement::GetHeight()
{
	//вернуть высоту шрифта
	return gApplication.mFontCodeHeight;
}

// class CDialogListImports

BOOL CDialogListImports::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//создать список импорта
			mpList=new CUserControlList(IDC_LIST_IMPORTS,GetDlgItem(mhWindow,IDC_LIST_IMPORTS));
			//заполнить список
			FillList(TEXT(""));
		}
		return TRUE;
	case WM_DESTROY:
		//удалить список
		SafeDelete(mpList);
		return TRUE;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDC_LIST_IMPORTS:
				//если уведомление о том, что выделение в списке изменилось
				if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
				{
					//указать имя кнопке "Go to import" в зависимости от типа выбранного элемента
					HWND hButton=GetDlgItem(mhWindow,IDC_BUTTON_GOTO_IMPORT);
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
					SetWindowText(hButton,pElement->mIsLibrary ? TEXT("Go to IAT of library") : TEXT("Go to function address in IAT"));
					//включить кнопку "Go to import"
					EnableWindow(hButton,TRUE);
					//включить или выключить кнопку "X references"
					EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_XREFERENCES),!pElement->mIsLibrary);
				}
				return TRUE;
			case IDC_EDIT_FILTER:							// поле фильтра
				//если уведомление о том, что текст изменился
				if(HIWORD(wParam)==EN_CHANGE)
				{
					//получить текст фильтра
					HWND hEdit=GetDlgItem(mhWindow,IDC_EDIT_FILTER);
					INT Length=GetWindowTextLength(hEdit)+1;
					LPTSTR szFilter=new TCHAR[Length];
					GetWindowText(hEdit,szFilter,Length);
					//заполнить список
					FillList(szFilter);
					//освободить память
					SafeDeleteMassive(szFilter);
				}
				return TRUE;
			case IDC_BUTTON_GOTO_IMPORT:			// кнопка "Go to import"
				{
					//получить адрес выбранного элемента
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
					if(pElement->mIsLibrary)
						*mpAddress=pElement->mpLibrary->GetAddressIAT();
					else
						*mpAddress=pElement->mpFunction->GetAddressInIAT();
					//завершить диалог
					EndDialog(mhWindow,TRUE);
				}
				return TRUE;
			case IDC_BUTTON_XREFERENCES:			// кнопка "X references"
				{
					//получить адрес выбранного элемента (он должен быть функцией)
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();

					//получить данные адреса
					CAddressData* pAddressData=mpExploredImage->GetAddressData(pElement->mpFunction->GetAddressInIAT());
					if(pAddressData)
					{
						//проверить, что хотя бы одна ссылка есть
						std::multimap<DWORD,CAddressDataElement*>* pAddressElements=pAddressData->GetElements();
						pAddressData->Release();
						if(pAddressElements->find(ADET_XREFERENCE)!=pAddressElements->end())
						{
							//вывести диалог перекрестных ссылок
							CDialogGoToXReference Dialog;
							CAddressMap* pAddressMap=mpExploredImage->GetAddressMap();
							if(Dialog.Show(mhWindow,pAddressData,pAddressMap,mpAddress))
								//завершить диалог
								EndDialog(mhWindow,TRUE);
							pAddressMap->Release();
							//завершить обработку
							return TRUE;
						}
					}
					//вывести сообщение о том, что у адреса нет перекрестных ссылок
					MessageBox(mhWindow,TEXT("Can't find any x-references for this import function."),TEXT("Find x-references"),MB_ICONSTOP);
				}
				return TRUE;
			case IDCANCEL:										// кнопка "Close"
				EndDialog(mhWindow,FALSE);
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}
	//если мы не обрабатываем сообщение, то, может, его обработает список
	if(mpList) return mpList->ProcessMessages(Msg,wParam,lParam);
	//иначе ничего не делать
	return FALSE;
}

//выполнить заполнение списка
VOID CDialogListImports::FillList(LPCTSTR szFilter)
{
	//начать обновление
	mpList->BeginUpdate();
	//очистить список
	mpList->Clear();
	//выключить кнопки
	EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_GOTO_IMPORT),FALSE);
	EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_XREFERENCES),FALSE);
	//заполнить список функций и библиотек
	std::list<CImageListImports::CImportLibrary*>* pLibrariesList=mpImportsList->GetList();
	for(std::list<CImageListImports::CImportLibrary*>::iterator i=pLibrariesList->begin();i!=pLibrariesList->end();i++)
	{
		//флаг добавленности библиотеки
		BOOL LibraryAddedFlag=FALSE;
		//получить библиотеку
		CImageListImports::CImportLibrary* pLibrary=*i;
		//проверить саму библиотеку
		if(IsAgreeFilterI(pLibrary->GetName(),szFilter))
		{
			//добавить библиотеку в список
			mpList->AddElement(new CListElement(pLibrary));
			//установить флаг добавленности
			LibraryAddedFlag=TRUE;
		}

		//получить список функций
		std::list<CImageListImports::CImportFunction*>* pFunctionsList=(*i)->GetList();
		//цикл по функциям из библиотеки
		for(std::list<CImageListImports::CImportFunction*>::iterator j=pFunctionsList->begin();j!=pFunctionsList->end();j++)
		{
			//получить функцию
			CImageListImports::CImportFunction* pFunction=*j;
			//проверить имя
			if(IsAgreeFilter(pFunction->GetName(),szFilter))
			{
				//если библиотека не добавлена
				if(!LibraryAddedFlag)
				{
					//добавить библиотеку в список
					mpList->AddElement(new CListElement(pLibrary));
					//установить флаг добавленности
					LibraryAddedFlag=TRUE;
				}
				//добавить функцию
				mpList->AddElement(new CListElement(pFunction));
			}
		}
	}
	//закончить обновление
	mpList->EndUpdate();
}

//получить список импорта и вывести диалог
BOOL CDialogListImports::Show(CExploredImage* pExploredImage,CYBER_ADDRESS* pAddress,HWND hWndParent)
{
	//получить список импорта
	std::map<UINT,CObject*>* pExtendMap=pExploredImage->GetExtendMap();
	std::map<UINT,CObject*>::iterator i=pExtendMap->find(IMAGE_EXTEND_LIST_IMPORTS);
	if(i==pExtendMap->end())
	{
		MessageBox(NULL,TEXT("Imports list for this image is not supported."),TEXT("Imports List"),MB_ICONSTOP);
		return FALSE;
	}
	mpImportsList=(CImageListImports*)(*i).second;

	//запомнить адрес образа
	mpExploredImage=pExploredImage;
	//запомнить адрес переменной-результата
	mpAddress=pAddress;
	//инициализировать указатель на список
	mpList=NULL;
	//вывести диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_LIST_IMPORTS),hWndParent);
}
