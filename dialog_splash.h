#ifndef QS_DIALOG_SPLASH_H
#define QS_DIALOG_SPLASH_H

/*
Файл содержит описание класса для использования окна сплэщ-экрана.
*/

#include "dialog.h"

class CSplashWindow : CDialog
{
protected:
	//текст и заголовок
	LPCTSTR mszText;
	LPCTSTR mszCaption;

	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//конструктор и деструктор
	CSplashWindow();
	~CSplashWindow();

	//вызвать окно
	VOID Show(HWND hWndParent,LPCTSTR szText,LPCTSTR szCaption);
	//закрыть окно
	VOID Hide();
};

#endif
