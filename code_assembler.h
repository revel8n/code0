#ifndef QS_CODE_ASSEMBLER_H
#define QS_CODE_ASSEMBLER_H

/*
В файле содержится определение класса ассемблера кода.
*/

#include "object.h"
#include "instruction_set.h"
#include "instruction.h"
#include <list>

class CCodeAssembler : public CObject
{
public:
	//класс варианта ассемблированной инструкции
	class CVariant
	{
	public:
		//код и его длина
		PBYTE mpCode;
		UINT mCodeSize;
		//инструкция
		CInstruction* mpInstruction;

	public:
		//конструктор и деструктор
		CVariant();
		~CVariant();
	};

protected:
	//набор инструкций
	CInstructionSet* mpInstructionSet;
	//список вариантов, используемый при ассемблировании
	std::list<CVariant*>* mpList;

public:
	//конструктор и деструктор
	CCodeAssembler(CInstructionSet* pInstructionSet);
	~CCodeAssembler();

	//ассемблировать инструкцию и выдать варианты кода
	//возвращаемое значение определяет корректность строки
	BOOL Assemble(LPCTSTR szCode,std::list<CVariant*>* pList);
};

#endif
