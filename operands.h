#ifndef QS_OPERANDS_H
#define QS_OPERANDS_H

#include "object.h"
#include "disassembly.h"
#include "listing.h"

// интерфейсы операндов

// типы операндов
#define OT_DATA												0								//константа
#define OT_REGISTER										1								//регистр
#define OT_MEMORY											2								//память

// классы операндов

class COperand : public CObject												//операнд инструкции
{
public:
	virtual DWORD GetType() PURE;												//получить тип операнда (флаги OT_*)
	virtual VOID Print(CListing*) PURE;									//напечатать операнд
	virtual BOOL GetReference(DWORD* pReference);				//получить прямую ссылку (определена заглушка)
};

class COperandData : public COperand									//операнд - константа
{
public:
	DWORD mData;																				//значение константы
	BYTE mReferenceFlags;																//флаги OIR_*, описывающие тип ссылки данных

public:
	COperandData(DWORD);																//создать константу с заданным значением

	virtual DWORD GetType();														//получить тип операнда
	virtual VOID Print(CListing*);											//напечатать операнд
	virtual BOOL GetReference(DWORD* pReference);				//получить прямую ссылку
};

class COperandRegister : public COperand							//операнд - регистр
{
public:
	BYTE mRegister;																			//регистр

public:
	COperandRegister(BYTE);															//создать регистр с заданным значением

	virtual DWORD GetType();														//получить тип операнда
	virtual VOID Print(CListing*);											//напечатать операнд
};

//флаги операнда-переменной
#define OMF_BASE_REGISTER					1								//используется базовый регистр
#define OMF_INDEX_REGISTER				2								//используется индексный регистр
#define OMF_MULTIPLIER						4								//используется множитель
#define OMF_OFFSET								8								//используется смещение
#define OMF_SEGMENT								16							//используется сегментный регистр
class COperandMemory : public COperand								//операнд - переменная в памяти
{
public:
	BYTE mFlags;																				//флаги
	BYTE mBaseRegister;																	//базовый регистр
	BYTE mIndexRegister;																//индексный регистр
	BYTE mMultiplier;																		//множитель
	DWORD mOffset;																			//смещение
	BYTE mSize;																					//размер операнда
	BYTE mSegment;																			//сегментный регистр

public:
	COperandMemory(MODRM);															//создать операнд-переменную из поля RM байта ModRM
	COperandMemory(DWORD);															//создать операнд-переменную по константному адресу

	VOID SetSegment(BYTE Segment);											//указать сегментный регистр для адреса

	virtual DWORD GetType();														//получить тип операнда
	virtual VOID Print(CListing*);											//напечатать операнд
	virtual BOOL GetReference(DWORD* pReference);				//получить прямую ссылку
};

#endif
