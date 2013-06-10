#ifndef QS_DIALOG_INSTRUCTION_VISUALIZER_H
#define QS_DIALOG_INSTRUCTION_VISUALIZER_H

/*
Файл содержит определение класса диалога - визуализатора инструкций.
*/

#include "dialog.h"
#include "cyber_memory.h"
#include "instruction_set.h"

class CDialogInstructionVisualizer : public CDialog
{
protected:
	//вспомогательная структура "куска кода"
	struct CHUNK
	{
		INT FirstByte;						//первый выделенный байт
		INT BytesCount;						//количество выделенных байтов

		INT Left;									//левая граница рисования
		INT Right;								//правая граница рисования

		LPTSTR szCode;						//код
		LPTSTR szMessage;					//пояснение
	};
protected:
	//машинный код инструкции
	PBYTE mpCode;
	//длина кода
	UINT mCodeLength;
	//метафайл изображения для перерисовки
	HENHMETAFILE mhImageMetaFile;

	//набор инструкций
	CInstructionSet* mpInstructionSet;

	//специальный большой шрифт для рисования
	static HFONT mhFontBig;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//обновить изображение
	VOID Update();
	//сформировать изображение
	VOID MakeImage(HDC hdc);

public:
	//конструктор
	CDialogInstructionVisualizer();

	//вывести диалог
	VOID Show(HWND hWndParent);
	//вывести диалог с заданной инструкцией
	VOID Show(CCyberMemory* pMemory,CYBER_ADDRESS Address,UINT Size,HWND hWndParent);
};

#endif
