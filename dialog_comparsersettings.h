#ifndef QS_DIALOG_COMPARSERSETTINGS_H
#define QS_DIALOG_COMPARSERSETTINGS_H

/*
Файл содержит определение класса-диалога настроек
файлового анализатора CCOMParser.
*/

#include "dialog.h"
#include "com_parser.h"

class CDialogCOMParserSettings : public CDialog
{
protected:
	//указатель на структуру с настройками
	CCOMParser::SETTINGS* mpSettings;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//показать диалог
	BOOL Show(HWND hWndParent,CCOMParser::SETTINGS* pSettings);
};

#endif
