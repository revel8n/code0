#include "dialog_gotoxreference.h"
#include "xreference.h"
#include "usercontrol_list.h"
#include "application.h"
#include "listing_string.h"
#include "resource.h"

// class CListElement

class CDialogGoToXReferenceListElement : public CUserControlListElement
{
	friend class CDialogGoToXReference;
protected:
	//указатель на класс диалога
	CDialogGoToXReference* mpDialog;
	//итератор перекрестной ссылки
	CAddressData::ADELI mXReferenceIterator;

public:
	CDialogGoToXReferenceListElement(CDialogGoToXReference* pDialog,CAddressData::ADELI XReferenceIterator)
	{
		//запомнить указатель и итератор
		mpDialog=pDialog;
		mXReferenceIterator=XReferenceIterator;
	}

	virtual VOID Draw(HDC hdc,RECT* pRect)
	{
		//получить перекрестную ссылку
		CXReference* pXReference=(CXReference*)(*mXReferenceIterator).second;

		//скомпоновать текст для перекрестной ссылки
		pXReference->Print(&mpDialog->mListing);
		//напечатать текст
		LPCTSTR szText=mpDialog->mListing.GetString()+mpDialog->mListing.GetColumnMargin(LISTING_COLUMN_COMMENTS)+2;
		TextOut(hdc,pRect->left,pRect->top,szText,_tcslen(szText));
	}

	virtual UINT GetHeight()
	{
		//вернуть высоту шрифта
		return gApplication.mFontCodeHeight;
	}
};

// class CDialogGoToXReference

BOOL CDialogGoToXReference::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//создать список
			mpList=new CUserControlList(IDC_LIST_XREFERENCES,GetDlgItem(mhWindow,IDC_LIST_XREFERENCES));
			//заполнить список
			CAddressData::ADEL* pElements=mpAddressData->GetElements();
			CAddressData::ADELI Begin=pElements->lower_bound(ADET_XREFERENCE),End=pElements->upper_bound(ADET_XREFERENCE);
			for(CAddressData::ADELI i=Begin;i!=End;i++)
				mpList->AddElement(new CDialogGoToXReferenceListElement(this,i));
		}
		return TRUE;
	case WM_DESTROY:
		//удалить список
		SafeDelete(mpList);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_GO:								//кнопка "Go"
			//получить адрес выделенной ссылки
			*mpAddress=((CXReference*)(*((CDialogGoToXReferenceListElement*)mpList->GetSelectedElement())->mXReferenceIterator).second)->GetAddressFrom();
			EndDialog(mhWindow,TRUE);
			return TRUE;
		case IDCANCEL:										//кнопка Cancel
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_LIST_XREFERENCES:				//список ссылок
			//если уведомление о том, что выделение в списке изменилось
			if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
				//включить кнопку "Go"
				EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_GO),TRUE);
			return TRUE;
		}
		return FALSE;
	}
	//если мы не обрабатываем сообщение, то, может, его обработает список
	if(mpList) return mpList->ProcessMessages(Msg,wParam,lParam);
	//иначе ничего не делать
	return FALSE;
}

BOOL CDialogGoToXReference::Show(HWND hWndParent,CAddressData* pAddressData,CAddressMap* pAddressMap,CYBER_ADDRESS* pAddress)
{
	//инициализировать указатель на список
	mpList=NULL;
	//инициализировать листинг
	mListing.SetAddressMap(pAddressMap);
	//запомнить параметры
	mpAddressData=pAddressData;
	mpAddress=pAddress;

	//вызвать диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_GO_TO_XREFERENCE),hWndParent);
}
