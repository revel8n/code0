#include "code_assembler.h"
#include "strings.h"

// class CCodeAssembler

CCodeAssembler::CCodeAssembler(CInstructionSet* pInstructionSet)
{
	//запомнить набор инструкций
	mpInstructionSet=pInstructionSet;
	mpInstructionSet->AddRef();
}

CCodeAssembler::~CCodeAssembler()
{
	SafeRelease(mpInstructionSet);
}

BOOL CCodeAssembler::Assemble(LPCTSTR szCode,std::list<CCodeAssembler::CVariant*>* pList)
{
	//запомнить список
	mpList=pList;

	//скопировать строку кода
	UINT Length=_tcslen(szCode);
	LPTSTR szLine=new TCHAR[Length];
	_tcscpy(szLine,szCode);
	//получить имя инструкции (вернее, фильтр имени)
	UINT i;
	for(i=0;i<Length;++i)
		if(!_istspace(szLine[i]))
			break;
	LPCTSTR szFilter=szLine+i;
	for(i++;i<Length;++i)
		if(_istspace(szLine[i]))
			break;
	szLine[i]=0;

	//найти инструкции с подходящим именем
	UINT InstructionsCount=mpInstructionSet->mInstructionsCount;
	for(i=0;i<InstructionsCount;++i)
	{
		//получить инструкцию
		INSTRUCTION_INFORMATION II=mpInstructionSet->mInstructions[i];
		//если имя подходит
		if(IsAgreeFilterI(II.szName,szFilter))
		{
		}
	}

	delete [] szLine;

	return TRUE;
}
