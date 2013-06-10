#include "dialog_editlabel.h"
#include "application.h"
#include "resource.h"

// class CDialogEditLabel

BOOL CDialogEditLabel::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//указать шрифт для текстового поля
		FORWARD_WM_SETFONT(GetDlgItem(mhWindow,IDC_EDIT_LABEL),gApplication.mhFontCode,FALSE,SendMessage);
		//если начальная метка задана
		if(mszLabel)
		{
			//вывести её в текстовом поле
			SetDlgItemText(mhWindow,IDC_EDIT_LABEL,mszLabel);
			//включить кнопку Delete
			EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_DELETE_LABEL),TRUE);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:										//кнопка "OK"
			{
				//получить длину введенной метки
				HWND hEdit=GetDlgItem(mhWindow,IDC_EDIT_LABEL);
				UINT Length=GetWindowTextLength(hEdit)+1;
				//если текст есть
				if(Length>1)
				{
					//выделить память и получить текст
					mszLabel=new TCHAR[Length];
					GetWindowText(hEdit,mszLabel,Length);
					//всё
					EndDialog(mhWindow,TRUE);
				}
				else
				{
					//надо удалить метку
					mszLabel=NULL;
					EndDialog(mhWindow,TRUE);
				}
			}
			return TRUE;
		case IDCANCEL:								//кнопка "Cancel"
			//ничего не делать
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_BUTTON_DELETE_LABEL:	//кнопка "Delete"
			//надо удалить метку
			mszLabel=NULL;
			EndDialog(mhWindow,TRUE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CDialogEditLabel::Show(LPCTSTR szExistLabel,LPTSTR* pszResultLabel,HWND hWndParent)
{
	//запомнить указатель на строку
	mszLabel=(LPTSTR)szExistLabel;
	//вывести диалог
	BOOL Result=Ask(MAKEINTRESOURCE(IDD_DIALOG_EDITLABEL),hWndParent);
	//вернуть значение указателя строки
	*pszResultLabel=mszLabel;
	//вернуть ответ
	return Result;
}
