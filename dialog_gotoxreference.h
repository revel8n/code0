#ifndef QS_DIALOG_GOTOXREFERENCE_H
#define QS_DIALOG_GOTOXREFERENCE_H

/*
‘айл содержит объ€вление класса CDialogGoToXReference,
поддерживающего диалог со списком перекрестных ссылок.
*/

#include "dialog.h"
#include "usercontrol_list.h"
#include "address_data.h"
#include "address_map.h"
#include "listing_string.h"

class CDialogGoToXReferenceListElement;

class CDialogGoToXReference : public CDialog
{
	friend class CDialogGoToXReferenceListElement;
protected:
	//данные адреса
	CAddressData* mpAddressData;
	//указатель на переменную дл€ записи результата
	CYBER_ADDRESS* mpAddress;
	//список перекрестных ссылок
	CUserControlList* mpList;
	//листинг дл€ вывода
	CListingString mListing;

protected:
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

public:

	//вызвать диалог
	BOOL Show(HWND hWndParent,CAddressData* pAddressData,CAddressMap* pAddressMap,CYBER_ADDRESS* pAddress);
};

#endif

