#ifndef QS_CODE_PARSER_H
#define QS_CODE_PARSER_H

/*
‘айл содержит определение класса CCodeParser, занимающегос€ дизассемблированием кода.
*/

#include "object.h"
#include "cyber_memory.h"
#include "address_map.h"
#include "instruction_set.h"
#include "instruction.h"
#include <set>

#define MODE_16BIT					0							//16-битный режим
#define MODE_32BIT					1							//32-битный режим

//структура, используема€ дл€ хранени€ параметров режимов
struct MODE_STRUCT
{
	CInstructionSet* pInstructionSet;				//набор инструкций
	BYTE NormalAddressOperandSize;					//размер "большого" операнда при нормальных услови€х
	BYTE RedefinedAddressOperandSize;				//размер "большого" операнда при наличии префикса переопределени€ размера адреса
};

class CCodeParser : public CObject										//класс, занимающийс€ дизасемблированием кода
{
protected:
	CCyberMemoryAccessor* mpMemory;											//адресное киберпространство пам€ти
	CAddressMap* mpAddressMap;													//карта информации адресов
	MODE_STRUCT mModes[2];															//параметры режимов

	DWORD mMode;																				//текущий режим (т.е. режим процессора, не завис€щий от префиксов переопределени€ разр€дности)
	BYTE mAddressOperandSize;														//размер "большого" операнда в данном режиме (2 или 4 байта)
	MODE_STRUCT* mpCurrentMode;													//параметры текущего режима (а это режим текущей инструкции)
	DWORD mPrefixFlags;																	//флаги префиксов текущей декодируемой инструкции
	CYBER_ADDRESS mInstructionAddress;									//адрес начала текущей инструкции
	CYBER_ADDRESS mSubroutineAddress;										//адрес начала функции
	std::set<std::pair<CYBER_ADDRESS,CYBER_ADDRESS> > mParsingQueue;	//очередь дизассемблировани€ (первый адрес - адрес, второй адрес - начало функции)

public:
	CCodeParser(CCyberMemory* pMemory,CAddressMap* pAddressMap,CInstructionSet* pSet16,CInstructionSet* pSet32);	//конструктор
	virtual ~CCodeParser();

	VOID SetMode(DWORD Mode);														//установить текущий режим
	VOID ParseEntryPoint(CYBER_ADDRESS EntryPoint);			//начать дизассемблирование с указанной точки входа

protected:
	//обновить текущий режим
	VOID UpdateMode();
	//декодировать байт ModRM
	VOID ParseModRM(MODRM* pModRM);
	//рекурсивно декодировать очередной байт инструкции
	DWORD ParseInstructionByte(DWORD Group,BYTE Mask);
	//декодировать инструкцию
	CInstruction* ParseInstruction(CYBER_ADDRESS Address);
	//добавить перекрестную ссылку
	VOID AddXReference(CYBER_ADDRESS AddressFrom,CYBER_ADDRESS AddressTo,BYTE ReferenceFlags,BYTE Size=0);
};

#endif
