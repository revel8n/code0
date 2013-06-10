#include "dialog_editcomment.h"
#include "application.h"
#include "resource.h"

// class CDialogEditComment

BOOL CDialogEditComment::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//указать шрифт для текстового поля
		FORWARD_WM_SETFONT(GetDlgItem(mhWindow,IDC_EDIT_COMMENT),gApplication.mhFontCode,FALSE,SendMessage);
		//если начальный текст задан
		if(mszComment)
		{
			//вывести его в текстовом поле
			SetDlgItemText(mhWindow,IDC_EDIT_COMMENT,mszComment);
			//включить кнопку Delete
			EnableWindow(GetDlgItem(mhWindow,IDC_BUTTON_DELETE_COMMENT),TRUE);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:										//кнопка "OK"
			{
				//получить длину введенного текста
				HWND hEdit=GetDlgItem(mhWindow,IDC_EDIT_COMMENT);
				UINT Length=GetWindowTextLength(hEdit)+1;
				//если текст есть
				if(Length>1)
				{
					//выделить память и получить текст
					mszComment=new TCHAR[Length];
					GetWindowText(hEdit,mszComment,Length);
					//всё
					EndDialog(mhWindow,TRUE);
				}
				else
				{
					//надо удалить комментарий
					mszComment=NULL;
					EndDialog(mhWindow,TRUE);
				}
			}
			return TRUE;
		case IDCANCEL:								//кнопка "Cancel"
			//ничего не делать
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_BUTTON_DELETE_COMMENT:	//кнопка "Delete"
			//надо удалить комментарий
			mszComment=NULL;
			EndDialog(mhWindow,TRUE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

BOOL CDialogEditComment::Show(LPCTSTR szExistComment,LPTSTR* pszResultComment,HWND hWndParent)
{
	//запомнить указатель на строку
	mszComment=(LPTSTR)szExistComment;
	//вывести диалог
	BOOL Result=Ask(MAKEINTRESOURCE(IDD_DIALOG_EDITCOMMENT),hWndParent);
	//вернуть значение указателя строки
	*pszResultComment=mszComment;
	//вернуть ответ
	return Result;
}
