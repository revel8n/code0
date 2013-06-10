#ifndef QS_DIALOG_ABOUT_H
#define QS_DIALOG_ABOUT_H

/*
Файл содержит определение класса диалога "About Code0".
*/

#include "dialog.h"

class CDialogAbout : public CDialog
{
protected:
	//контекст и картинка изображения
	HDC mhdcImage;
	HBITMAP mbmpImage;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);
public:
	//показать диалог
	VOID Show(HWND hWndParent);
};

#endif
