#ifndef QS_DIALOG_ENTERSTRING_H
#define QS_DIALOG_ENTERSTRING_H

/*
‘айл содержит определение класса диалога, который
спрашивает у пользовател€ текстовую строку.
*/

#include "dialog.h"

class CDialogEnterString : public CDialog
{
protected:
	//указатель на буфер дл€ текста
	LPTSTR mszTextResult;
	//строки с текстом в диалоге
	LPCTSTR mszDialogTitle;
	LPCTSTR mszDialogCaption;
	LPCTSTR mszDialogInitText;

protected:
	//оконна€ процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	CDialogEnterString();
	~CDialogEnterString();

	//получить текстовую строку; если пользователь нажимает отмену,
	//то возвращаетс€ NULL. »наче возвращаетс€ указатель на буфер со строкой,
	//который актуален, пока существует экземпл€р класса.
	LPTSTR Show(LPCTSTR szDialogTitle,LPCTSTR szDialogCaption,LPCTSTR szDialogInitText,HWND hWndParent);
};

#endif
