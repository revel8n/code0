#ifndef QS_DIALOG_PEPARSERSETTINGS_H
#define QS_DIALOG_PEPARSERSETTINGS_H

/*
Файл содержит определение класса-диалога настроек
файлового анализатора CPEParser.
*/

#include "dialog.h"
#include "pe_parser.h"

class CDialogPEParserSettings : public CDialog
{
protected:
	//указатель на структуру с настройками
	CPEParser::SETTINGS* mpSettings;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:
	//показать диалог
	BOOL Show(HWND hWndParent,CPEParser::SETTINGS* pSettings);
};

#endif
