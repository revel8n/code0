#ifndef QS_DIALOG_EDITLABEL_H
#define QS_DIALOG_EDITLABEL_H

/*
Файл содержит определение класса диалога редактирования метки.
*/

#include "dialog.h"
#include "label.h"

class CDialogEditLabel : public CDialog
{
protected:
	//указатель на метку
	LPTSTR mszLabel;

protected:
	//оконная процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//вызов диалога
	//возвращается строка (в *pszResultLabel) - имя метки, строка должна освобождаться в вызывающей функции с помощью delete []
	//если возвращается NULL, то метку нужно удалить
	//если результат функции - FALSE, то пользователь нажал "Отмену"
	BOOL Show(LPCTSTR szExistLabel,LPTSTR* pszResultLabel,HWND hWndParent);
};

#endif
