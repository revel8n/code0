#ifndef QS_DIALOG_INSTRUCTIONASSEMBLER_H
#define QS_DIALOG_INSTRUCTIONASSEMBLER_H

/*
‘айл содержит объ€вление класса CDialogInstructionAssembler,
управл€ющего диалогом ассемблировани€.
*/

#include "dialog.h"
#include "usercontrol_list.h"
#include "address_data.h"
#include "address_map.h"
#include "listing_string.h"

class CDialogInstructionAssembler : public CDialog
{
protected:
	//класс элемента списка вариантов
	class CListElement : CUserControlListElement
	{
	public:
		//код инструкции
		PBYTE mpCode;
		//размер кода
		UINT mCodeSize;
		//инструкци€
		CInstruction* mpInstruction;

	public:
		//конструктор и деструктор
		CListElement();
		virtual ~CListElement();

		//нарисовать элемент
		virtual VOID Draw(HDC hdc,RECT* pRect);
		//получить высоту элемента
		virtual UINT GetHeight();
	};
protected:
	//список вариантов ассемблировани€
	CUserControlList* mpList;
	//листинг дл€ вывода
	CListingString mListing;

protected:
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:

	//вызвать диалог
	BOOL Show(HWND hWndParent);
};

#endif

