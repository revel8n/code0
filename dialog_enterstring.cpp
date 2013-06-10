#include "dialog_enterstring.h"
#include "application.h"
#include "resource.h"

// class CDialogEnterString

BOOL CDialogEnterString::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//указать шрифт для текстового поля
		FORWARD_WM_SETFONT(GetDlgItem(mhWindow,IDC_EDIT_STRING),gApplication.mhFontCode,FALSE,SendMessage);
		//инициализировать строки в диалоге
		SetWindowText(mhWindow,mszDialogCaption);
		SetDlgItemText(mhWindow,IDC_STATIC_TITLE,mszDialogTitle);
		if(mszDialogInitText) SetDlgItemText(mhWindow,IDC_EDIT_STRING,mszDialogInitText);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				//получить поле
				HWND hEdit=GetDlgItem(mhWindow,IDC_EDIT_STRING);
				//получить длину текста в поле
				INT Length=GetWindowTextLength(hEdit);
				//выделить память и скопировать текст
				mszTextResult=new TCHAR[Length+1];
				GetWindowText(hEdit,mszTextResult,Length+1);
				//завершить диалог
				EndDialog(mhWindow,TRUE);
			}
			return TRUE;
		case IDCANCEL:
			//завершить диалог
			EndDialog(mhWindow,FALSE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

CDialogEnterString::CDialogEnterString()
{
	//инициализировать указатель
	mszTextResult=NULL;
}

CDialogEnterString::~CDialogEnterString()
{
	//освободить память буфера результирующей строки
	SafeDeleteMassive(mszTextResult);
}

LPTSTR CDialogEnterString::Show(LPCTSTR szDialogTitle,LPCTSTR szDialogCaption,LPCTSTR szDialogInitText,HWND hWndParent)
{
	//запомнить указатели на строки
	mszDialogTitle=szDialogTitle;
	mszDialogCaption=szDialogCaption;
	mszDialogInitText=szDialogInitText;
	//вызвать диалог; если успешно
	if(Ask(MAKEINTRESOURCE(IDD_DIALOG_ENTERSTRING),hWndParent))
		//вернуть указатель на строку
		return mszTextResult;
	//иначе ничего не возвращать
	return NULL;
}
