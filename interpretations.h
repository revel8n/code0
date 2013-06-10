#ifndef QS_INTERPRETATIONS_H
#define QS_INTERPRETATIONS_H

/*
‘айл содержит объ€влени€ классов интерпретаций.
*/

#include "address_data_element.h"
#include "cyber_memory.h"
#include "instruction.h"

// типы интерпретаций

#define ADEIT_DATA									0
#define ADEIT_CODE									1
#define ADEIT_STRING								2

// общий класс интерпретации
class CInterpretation : public CAddressDataElement
{
public:
	virtual DWORD GetElementType();									//получить тип элемента данных адреса
	virtual DWORD GetInterpretationType() PURE;			//получить тип интерпретации
	virtual BOOL GetReference(CYBER_ADDRESS* pAddressReference);	//получить пр€мую ссылку (опеределена заглушка)
};

// класс интерпретации - данных

class CInterpretationData : public CInterpretation
{
protected:
	CCyberMemory* mpMemory;													//указатель на киберпам€ть
	CYBER_ADDRESS mAddress;													//адрес начала
	DWORD mSize;																		//размер данных

public:
	CInterpretationData(CCyberMemory* pMemory,CYBER_ADDRESS Address,DWORD Size);
	virtual ~CInterpretationData();

	virtual DWORD GetInterpretationType();					//получить тип интерпретации
	virtual VOID Print(CListing* pListing);					//напечатать текстовое представление элемента данных
};

// класс интерпретации - кода

//флаги кода
#define ICF_SUBROUTINE_END							1							//конец функции
#define ICF_SPACE												2							//разделитель

class CInterpretationCode : public CInterpretation
{
protected:
	CInstruction* mpInstruction;										//указатель на инструкцию
	DWORD mFlags;																		//флаги кода

public:
	CInterpretationCode(CInstruction* pInstruction);
	virtual ~CInterpretationCode();

	VOID SetFlags(DWORD Flags);											//установить флаги
	virtual DWORD GetInterpretationType();					//получить тип интерпретации
	virtual VOID Print(CListing*);									//напечатать текстовое представление элемента данных
	virtual BOOL GetReference(CYBER_ADDRESS* pAddressReference);	//получить пр€мую ссылку
};

// абстрактный класс интерпретации-строки

class CInterpretationString : public CInterpretation
{
protected:
	CCyberMemory* mpMemory;													//указатель на киберпам€ть
	CYBER_ADDRESS mAddress;													//адрес начала

public:
	//деструктор
	virtual ~CInterpretationString();

	virtual DWORD GetInterpretationType();					//получить тип интерпретации
	virtual UINT GetStringSize() PURE;							//получить размер интерпретации-строки в байтах (перегружаетс€ в производных классах)
};

// класс интерпретации - Unicode-строки

class CInterpretationStringUnicode : public CInterpretationString
{
protected:

public:
	//конструктор и деструктор
	CInterpretationStringUnicode(CCyberMemory* pMemory,CYBER_ADDRESS Address);
	virtual ~CInterpretationStringUnicode();

	virtual VOID Print(CListing* pListing);					//напечатать строку
	virtual UINT GetStringSize();										//получить размер интерпретации-строки в байтах
};

// класс интерпретации - ASCII-строки

class CInterpretationStringASCII : public CInterpretationString
{
protected:

public:
	//конструктор и деструктор
	CInterpretationStringASCII(CCyberMemory* pMemory,CYBER_ADDRESS Address);
	virtual ~CInterpretationStringASCII();

	virtual VOID Print(CListing* pListing);					//напечатать строку
	virtual UINT GetStringSize();										//получить размер интерпретации-строки в байтах
};

#endif
