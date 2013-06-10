#include "dialog_list_exports.h"
#include "image_extend_object.h"
#include "dialog_gotoxreference.h"
#include "application.h"
#include "strings.h"
#include "resource.h"

// subclass CDialogListExports::CListElement

CDialogListExports::CListElement::CListElement(CImageListExports::CExportFunction* pFunction)
{
	//установить параметры
	mpFunction=pFunction;
	mpFunction->AddRef();
}

CDialogListExports::CListElement::~CListElement()
{
	//освободить объект
	mpFunction->Release();
}

VOID CDialogListExports::CListElement::Draw(HDC hdc,RECT* pRect)
{
	//напечатать имя функции
	SelectFont(hdc,gApplication.mhFontCode);
	//получить и напечатать ординал и имя
	static TCHAR szText[LISTING_LINE_LENGTH];
	LPCTSTR szName=mpFunction->GetName();
	if(szName)
		_stprintf(szText,TEXT("% 5u  %s"),mpFunction->GetOrdinal(),szName);
	else
		_stprintf(szText,TEXT("% 5u  (no name)"),mpFunction->GetOrdinal());
	TextOut(hdc,pRect->left,pRect->top,szText,_tcslen(szText));

	//получить отступ до края строки
	SIZE Size;
	GetTextExtentPoint32(hdc,szText,_tcslen(szText),&Size);
	Size.cx+=pRect->left;

	//если функция - локальная
	if(mpFunction->IsLocal())
		//напечатать адрес
		_stprintf(szText,TEXT(" %08X"),mpFunction->GetAddress());
	//иначе функция - форвард
	else
		//напечатать форвард
		_stprintf(szText,TEXT(" - forward to %s"),mpFunction->GetForwardName());
	TextOut(hdc,Size.cx,pRect->top,szText,_tcslen(szText));
}

UINT CDialogListExports::CListElement::GetHeight()
{
	//вернуть высоту шрифта
	return gApplication.mFontCodeHeight;
}

// class CDialogListExports

BOOL CDialogListExports::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//создать список экспорта
			mpList=new CUserControlList(IDC_LIST_EXPORTS,GetDlgItem(mhWindow,IDC_LIST_EXPORTS));
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
			case IDC_LIST_EXPORTS:
				//если уведомление о том, что выделение в списке изменилось
				if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
				{
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
					//получить флаг локальности
					BOOL IsLocal=pElement->mpFunction->IsLocal();
					//включить или выключить кнопку "Go to export"
					EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_GOTO_EXPORT),IsLocal);
					//включить или выключить кнопку "X references"
					EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_XREFERENCES),IsLocal);
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
			case IDC_BUTTON_GOTO_EXPORT:			// кнопка "Go to export"
				{
					//получить адрес выбранного элемента (он должен быть локальной функцией)
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();
					*mpAddress=pElement->mpFunction->GetAddress();
					//завершить диалог
					EndDialog(mhWindow,TRUE);
				}
				return TRUE;
			case IDC_BUTTON_XREFERENCES:			// кнопка "X references"
				{
					//получить адрес выбранного элемента (он должен быть локальной функцией)
					CListElement* pElement=(CListElement*)mpList->GetSelectedElement();

					//получить данные адреса
					CAddressData* pAddressData=mpExploredImage->GetAddressData(pElement->mpFunction->GetAddress());
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
					MessageBox(mhWindow,TEXT("Can't find any x-references for this export function."),TEXT("Find x-references"),MB_ICONSTOP);
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
VOID CDialogListExports::FillList(LPCTSTR szFilter)
{
	//очистить список
	mpList->Clear();
	//выключить кнопки
	EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_GOTO_EXPORT),FALSE);
	EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_XREFERENCES),FALSE);
	//заполнить список функций
	std::list<CImageListExports::CExportFunction*>* pFunctionsList=mpExportsList->GetList();
	for(std::list<CImageListExports::CExportFunction*>::iterator i=pFunctionsList->begin();i!=pFunctionsList->end();i++)
	{
		//получить функцию
		CImageListExports::CExportFunction* pFunction=*i;
		//проверить функцию
		LPCTSTR szName=pFunction->GetName();
		if(!*szFilter || (szName && IsAgreeFilter(szName,szFilter)))
				//добавить функцию в список
				mpList->AddElement(new CListElement(pFunction));
	}
}

//получить список экспорта и вывести диалог
BOOL CDialogListExports::Show(CExploredImage* pExploredImage,CYBER_ADDRESS* pAddress,HWND hWndParent)
{
	//получить список экспорта
	std::map<UINT,CObject*>* pExtendMap=pExploredImage->GetExtendMap();
	std::map<UINT,CObject*>::iterator i=pExtendMap->find(IMAGE_EXTEND_LIST_EXPORTS);
	if(i==pExtendMap->end())
	{
		MessageBox(NULL,TEXT("Exports list for this image is not supported."),TEXT("Exports List"),MB_ICONSTOP);
		return FALSE;
	}
	mpExportsList=(CImageListExports*)(*i).second;

	//запомнить адрес образа
	mpExploredImage=pExploredImage;
	//запомнить адрес переменной-результата
	mpAddress=pAddress;
	//инициализировать указатель на список
	mpList=NULL;
	//вывести диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_LIST_EXPORTS),hWndParent);
}
