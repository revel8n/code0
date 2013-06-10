#include "instruction.h"
#include "consts.h"

// class CInstruction

CInstruction::CInstruction(CInstructionSet* pInstructionSet,DWORD InstructionID,DWORD PrefixFlags)
{
	//запомнить набор инструкций, идентификатор инструкции и флаги префиксов
	mpInstructionSet=pInstructionSet;
	mpInstructionSet->AddRef();
	mInstructionID=InstructionID;
	mPrefixFlags=PrefixFlags;
}

CInstruction::~CInstruction()
{
	//очистить список операндов
	for(IOLI i=mOperandsList.begin();i!=mOperandsList.end();i++)
		(*i)->Release();
	mOperandsList.clear();

	//освободить набор инструкций
	mpInstructionSet->Release();
}

VOID CInstruction::AddOperand(COperand* pOperand)
{
	//добавить операнд в список операндов
	pOperand->AddRef();
	mOperandsList.push_back(pOperand);
}

VOID CInstruction::Print(CListing* pListing)
{
	//указатель строки
	UINT Position=pListing->GetColumnMargin(LISTING_COLUMN_NAME);

	//начать строку
	pListing->BeginLine();

	//напечатать префиксы
	for(DWORD i=0;i<PREFIXES_COUNT;++i)
		//если префикс есть, и он выводимый
		if(mPrefixFlags & (1<<i) && PREFIXES_VISIBLE & (1<<i))
		{
			//напечатать имя префикса и двоеточие
			pListing->Print(Position,gcszPrefixNames[i]);
			Position=pListing->Print(TEXT(":"))+1;
		}

	//напечатать имя функции
	LPCTSTR szName=mpInstructionSet->mInstructions[mInstructionID].szName;
	Position=pListing->Print(Position,szName)+1;

	//сдвинуть указатель на столбец операндов
	pListing->SetPosition(pListing->GetColumnMargin(LISTING_COLUMN_OPERANDS));

	//напечатать операнды
	for(IOLI i=mOperandsList.begin();i!=mOperandsList.end();i++)
	{
		//если этот операнд - не начальный
		if(i!=mOperandsList.begin())
			//напечатать запятую
			pListing->Print(TEXT(", "));

		//напечатать операнд
		(*i)->Print(pListing);
	}

#ifdef NIKITIN
	{
		//если конец, напечатать
		if(mpInstructionSet->mInstructions[mInstructionID].bFlags & IIF_ENDPROC)
			pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL), TEXT("S"));
		//если есть переход, напечатать
		DWORD reference;
		if(GetReference(&reference))
		{
			TCHAR s[100];
			_stprintf(s, TEXT("J%08X"), reference);
			pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL) + 1, s);
		}
	}
#endif

	//закончить строку
	pListing->EndLine();
}

BOOL CInstruction::GetReference(DWORD* pReference)
{
	//цикл по операндам
	for(IOLI i=mOperandsList.begin();i!=mOperandsList.end();i++)
		//если операнд возвращает ссылку
		if((*i)->GetReference(pReference))
			//вернуть её
			return TRUE;
	//иначе ссылки нет
	return FALSE;
}
