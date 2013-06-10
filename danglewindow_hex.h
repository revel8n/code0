#ifndef QS_DANGLEWINDOW_HEX_H
#define QS_DANGLEWINDOW_HEX_H

/*
Файл содержит определение класса окна просмотра файла в HEX-режиме.
*/

#include "dangle_window.h"
#include "edited_file.h"
#include "listing_device.h"

class CDangleWindowHex : public CDangleWindow
{
protected:
	//указатель на редактируемый файл
	CEditedFile* mpEditedFile;
	//сдвиг от начала файла, соответствующий началу окна
	UINT mWindowOffset;
	//адрес выделенного байта или начала выделения
	UINT mSelectionOffset;
	//длина выделения
	UINT mSelectionLength;
	//листинг для вывода
	CListingDevice mListing;
	//количество поместившихся в окне строк
	UINT mWindowLinesCount;

	//имя окна
	TCHAR mszNameBuffer[MAX_DANGLE_NAME_LENGTH+1];

protected:
	//процедуры обработки сообщений
	VOID OnKey(UINT Key);
	VOID OnCommand(UINT CommandID,UINT Code,HWND hControl);
	VOID OnMouseWheel(INT Delta,UINT Keys);

public:
	//конструктор и деструктор
	CDangleWindowHex(CEditedFile* pEditedFile);
	virtual ~CDangleWindowHex();

	//оконная процедура
	virtual LRESULT DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//рисование
	virtual VOID Paint(HDC hdc);
};

#endif
