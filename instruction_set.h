#ifndef QS_INSTRUCTION_SET_H
#define QS_INSTRUCTION_SET_H

/*
—одержит объ€вление класса, хран€щего набор инструкций дл€ анализа.
*/

#include "object.h"
#include "disassembly.h"

//максимальное количество типов инструкций
#define MAX_INSTRUCTIONS_COUNT												0x1000
//максимальное количество байтов инструкций
#define MAX_INSTRUCTION_BYTES_COUNT										0x10000

class CInstructionSet : public CObject
{
public:
	// переменные

	//набор инструкций
	INSTRUCTION_INFORMATION* mInstructions;
	//набор байтов инструкций
	INSTRUCTION_BYTE* mInstructionBytes;
	//количество инструкций
	DWORD mInstructionsCount;
	//выделенное количество байтов инструкций
	DWORD mInstructionBytesCount;

public:
	CInstructionSet();
	virtual ~CInstructionSet();

protected:
	// вспомогательные функции

	//скорректировать инструкцию
	VOID CorrectInstruction(INSTRUCTION_INFORMATION& II);

public:
	// методы дл€ управлени€ набором инструкций

	//в информации об инструкции разрешаетс€ не указывать некоторые пол€; они будут автоматически скорректированы
	//загрузить инструкцию в набор (возвращаетс€ идентификатор инструкции)
	DWORD LoadInstruction_OneByte(INSTRUCTION_INFORMATION II,BYTE Code);
	DWORD LoadInstruction_TwoBytes(INSTRUCTION_INFORMATION II,BYTE Code1,BYTE Code2,BYTE Mask);
	DWORD LoadInstruction_ThreeBytes(INSTRUCTION_INFORMATION II,BYTE Code1,BYTE Code2,BYTE Code3,BYTE Mask1,BYTE Mask2);
	//загрузить инструкцию-префикс
	VOID LoadPrefix(BYTE Code,DWORD Flag);
	//указать последней загруженной инструкции флаг(и)
	VOID SetInstructionFlag(DWORD Flag);

	// вывести таблицу опкодов в HTML
	VOID PrintHTML(LPCTSTR szFile,DWORD GroupByte=0);
};

#endif
