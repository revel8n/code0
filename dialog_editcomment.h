#ifndef QS_DIALOG_EDITCOMMENT_H
#define QS_DIALOG_EDITCOMMENT_H

/*
Файл содержит определение класса диалога редактирования комментария.
*/

#include "dialog.h"
#include "comment.h"

class CDialogEditComment : public CDialog
{
protected:
	//указатель на текст комментария
	LPTSTR mszComment;

protected:
	//оконная процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//вызов диалога
	//возвращается строка (в *pszResultComment) - текст комментария, строка должна освобождаться в вызывающей функции с помощью delete []
	//если возвращается NULL, то метку нужно удалить
	//если результат функции - FALSE, то пользователь нажал "Отмену"
	BOOL Show(LPCTSTR szExistComment,LPTSTR* pszResultComment,HWND hWndParent);
};

#endif
