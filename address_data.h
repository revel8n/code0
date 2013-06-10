#ifndef QS_ADDRESS_DATA_H
#define QS_ADDRESS_DATA_H

#include "object.h"
#include "cyber_memory.h"
#include "address_data_element.h"
#include "interpretations.h"
#include "label.h"
#include "comment.h"
#include "listing.h"
#include <map>

class CAddressData : public CObject									//класс, хранящий информацию об одном адресе
{
public:
	typedef std::multimap<DWORD,CAddressDataElement*> ADEL;
	typedef ADEL::iterator ADELI;
protected:
	ADEL mElementsMap;																//карта элементов данных адреса
	DWORD mSize;																			//размер блока адреса
	DWORD mPreviousSize;															//размер предыдущего блока
	CYBER_ADDRESS mBlockAddress;											//адрес начала процедуры/какого-то блока кода

protected:
	//вспомогательные функции

	//указать элемент, который может быть только один
	VOID SetIdentityElement(CAddressDataElement* pElement,DWORD ElementType);

public:
	CAddressData();																		//конструктор
	virtual ~CAddressData();													//деструктор

	//проверить, является ли данные пустыми, т.е. не содержат информации
	BOOL IsEmpty();

	//установить размер адреса
	VOID SetSize(DWORD Size);
	//получить размер адреса
	DWORD GetSize();
	//установить размер предыдущего адреса
	VOID SetPreviousSize(DWORD PreviousSize);
	//получить размер предыдущего адреса
	DWORD GetPreviousSize();
	//установить адрес начала процедуры
	VOID SetBlockAddress(CYBER_ADDRESS Address);
	//получить адрес начала процедуры
	CYBER_ADDRESS GetBlockAddress();

	//добавить элемент в информацию об адресе
	VOID AddElement(CAddressDataElement* pElement);
	//получить указатель на карту элементов данных адреса
	ADEL* GetElements();
	//напечатать данные адреса
	VOID Print(CListing* pListing);

	//функции управления особыми элементами
	//получить интерпретацию
	CInterpretation* GetInterpretation();
	//установить интерпретацию (NULL - удалить интерпретацию)
	VOID SetInterpretation(CInterpretation* pInterpretation);
	//получить метку
	CLabel* GetLabel();
	//установить метку (NULL - удалить метку)
	VOID SetLabel(CLabel* pLabel);
	//получить комментарий
	CComment* GetComment();
	//установить комментарий (NULL - удалить комментарий)
	VOID SetComment(CComment* pComment);
	//создать функцию
	VOID CreateSubroutine();
};

#endif
