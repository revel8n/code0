#ifndef QS_LABEL_H
#define QS_LABEL_H

/*
‘айл содержит объ€вление класса метки.
*/

#include "address_data_element.h"

class CLabel : public CAddressDataElement
{
protected:
	LPTSTR mszName;																		//указатель на им€ метки

public:
	CLabel(LPCTSTR szName);
	virtual ~CLabel();

	LPCTSTR GetName();																//получить указатель на им€ метки (действителен, только пока метка существует)

	virtual DWORD GetElementType();										//получить тип элемента данных адреса
	virtual VOID Print(CListing*);										//напечатать текстовое представление элемента данных
};

#endif
