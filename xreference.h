#ifndef QS_XREFERENCE_H
#define QS_XREFERENCE_H

/*
Файл содержит определение класса элемента данных адреса - перекрестной ссылки.
*/

#include "address_data_element.h"
#include "cyber_memory.h"

class CXReference : public CAddressDataElement
{
protected:
	CYBER_ADDRESS mAddressFrom;
	BYTE mReferenceFlags;

public:
	CXReference(CYBER_ADDRESS AddressFrom,BYTE ReferenceFlags);

	virtual DWORD GetElementType();										//получить тип элемента данных адреса
	virtual VOID Print(CListing*);										//напечатать текстовое представление элемента данных

	//специальные функции перекрестной ссылки
	CYBER_ADDRESS GetAddressFrom();										//получить обратный адрес перекрестной ссылки
};

#endif
