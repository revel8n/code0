#include "dialog_instructionassembler.h"
#include "usercontrol_list.h"
#include "application.h"
#include "listing_string.h"
#include "resource.h"

// class CDialogInstructionAssembler::CListElement

CDialogInstructionAssembler::CListElement::CListElement()
{
	//инициализация
	mpCode=NULL;
	mpInstruction=NULL;
}

CDialogInstructionAssembler::CListElement::~CListElement()
{
	//освободить ресурсы
	SafeDeleteMassive(mpCode);
	SafeRelease(mpInstruction);
}

VOID CDialogInstructionAssembler::CListElement::Draw(HDC hdc,RECT* pRect)
{
}

UINT CDialogInstructionAssembler::CListElement::GetHeight()
{
	//вернуть высоту шрифта
	return gApplication.mFontCodeHeight;
}

// class CDialogInstructionAssembler

BOOL CDialogInstructionAssembler::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//создать список
		mpList=new CUserControlList(IDC_LIST_XREFERENCES,GetDlgItem(mhWindow,IDC_LIST_XREFERENCES));
		return TRUE;
	case WM_DESTROY:
		//удалить список
		SafeDelete(mpList);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:										//кнопка Cancel
			EndDialog(mhWindow,FALSE);
			return TRUE;
/*		case IDC_LIST_XREFERENCES:				//список вариантов
			//если уведомление о том, что выделение в списке изменилось
			if(HIWORD(wParam)==LBN_SELCHANGE && mpList->IsExistSelection())
				//что-то сделать
				;
			return TRUE;*/
		}
		return FALSE;
	}
	//если мы не обрабатываем сообщение, то, может, его обработает список
	if(mpList) return mpList->ProcessMessages(Msg,wParam,lParam);
	//иначе ничего не делать
	return FALSE;
}

BOOL CDialogInstructionAssembler::Show(HWND hWndParent)
{
	//инициализировать указатель на список
	mpList=NULL;

	//вызвать диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_INSTRUCTIONASSEMBLER),hWndParent);
}
