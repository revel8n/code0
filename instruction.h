#ifndef QS_INSTRUCTION_H
#define QS_INSTRUCTION_H

/*
‘айл содержит объ€вление класса кодовой инструкции, хран€щего информацию об одной дизассемблированной инструкции.
*/

#include "object.h"
#include "operands.h"
#include "instruction_set.h"
#include "listing.h"
#include <list>

class CInstruction : public CObject
{
protected:
	typedef std::list<COperand*> IOL;							//тип списка операндов
	typedef IOL::iterator IOLI;										//итератор списка операндов

	CInstructionSet* mpInstructionSet;						//набор инструкций
	IOL mOperandsList;														//список операндов
	DWORD mPrefixFlags;														//флаги префиксов
public:
	DWORD mInstructionID;													//идентификатор инструкции

public:
	CInstruction(CInstructionSet* pInstructionSet,DWORD InstructionID,DWORD PrefixFlags);
	virtual ~CInstruction();

	VOID AddOperand(COperand* pOperand);					//добавить операнд в список
	VOID Print(CListing* pListing);								//напечатать инструкцию
	BOOL GetReference(DWORD* pReference);					//получить пр€мую ссылку
};

#endif
