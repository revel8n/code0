#include "interpretations.h"

// class CInterpretation

DWORD CInterpretation::GetElementType()
{
	return ADET_INTERPRETATION;
}

BOOL CInterpretation::GetReference(CYBER_ADDRESS* pAddressReference)
{
	//ссылки нет, это заглушка
	return FALSE;
}

// class CInterpretationData

CInterpretationData::CInterpretationData(CCyberMemory* pMemory,CYBER_ADDRESS Address,DWORD Size)
{
	//запомнить параметры
	mpMemory=pMemory;
	mpMemory->AddRef();
	mAddress=Address;
	mSize=Size;
}

CInterpretationData::~CInterpretationData()
{
	//освободить память
	mpMemory->Release();
}

DWORD CInterpretationData::GetInterpretationType()
{
	return ADEIT_DATA;
}

VOID CInterpretationData::Print(CListing* pListing)
{
	//начать строку
	pListing->BeginLine();

	//напечатать заголовок
	pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_NAME),TEXT("db"));

	//напечатать все байты, но не более, чем 0x10
	DWORD Size=mSize;
	if(Size>0x10) Size=0x10;
	DWORD i;
	for(i=0;i<Size;++i)
	{
		try
		{
			//получить значение байта
			BYTE Byte=mpMemory->Byte(mAddress+i);
			//напечатать значение байта
			TCHAR s[4];
			_stprintf(s,TEXT(" %02X"),Byte);
			pListing->Print(s);
		}
		catch(CCyberMemoryPageFaultException)
		{
			//напечатать символы нечитаемой памяти
			pListing->Print(TEXT(" ##"));
		}
	}

	//если напечатаны не все симвлоы
	if(Size<mSize)
		//вывести многоточие
		pListing->Print(TEXT(" ..."));

	//закончить строку
	pListing->EndLine();
}

// class CInterpretationCode

CInterpretationCode::CInterpretationCode(CInstruction* pInstruction)
{
	//запомнить указатель на инструкцию
	mpInstruction=pInstruction;
	if(mpInstruction) mpInstruction->AddRef();
	//обнулить флаги
	mFlags=0;
}

CInterpretationCode::~CInterpretationCode()
{
	//удалить инструкцию
	SafeRelease(mpInstruction);
}

VOID CInterpretationCode::SetFlags(DWORD Flags)
{
	//установить флаги
	mFlags|=Flags;
}

DWORD CInterpretationCode::GetInterpretationType()
{
	return ADEIT_CODE;
}

VOID CInterpretationCode::Print(CListing* pListing)
{
	//если инструкция есть
	if(mpInstruction)
		//напечатать инструкцию
		mpInstruction->Print(pListing);
	else
	{
		//напечатать, что инструкция - неизвестна
		pListing->BeginLine();
		pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_NAME),TEXT("(unknown instruction)"));
		pListing->EndLine();
	}

#ifdef NIKITIN
	if(0)
	{
#endif
	//если адрес - конец функции
	if(mFlags & ICF_SUBROUTINE_END)
	{
		pListing->BeginLine();
		pListing->EndLine();
		for(DWORD i=0;i<2;++i)
		{
			pListing->BeginLine();
			pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL)+10,TEXT("; *******************************************************************"));
			pListing->EndLine();
		}
		pListing->BeginLine();
		pListing->EndLine();
	}
/*		//напечатать пустые строки
		for(DWORD i=0;i<2;++i)
		{
			pListing->BeginLine();
			pListing->EndLine();
		}*/
	//если инструкция - разделитель
	else if(mFlags & ICF_SPACE)
	{
		pListing->BeginLine();
		pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL)+10,TEXT("; -------------------------------------------------------------------"));
		pListing->EndLine();
		pListing->BeginLine();
		pListing->EndLine();
	}
/*		//напечатать пустые строки
		for(DWORD i=0;i<2;++i)
		{
			pListing->BeginLine();
			pListing->EndLine();
		}*/
#ifdef NIKITIN
	}
#endif
}

BOOL CInterpretationCode::GetReference(CYBER_ADDRESS* pAddressReference)
{
	//если инструкция есть
	if(mpInstruction)
		//получить прямую ссылку
		return mpInstruction->GetReference(pAddressReference);
	//иначе ссылки нет
	return FALSE;
}

// class CIntepretationString

CInterpretationString::~CInterpretationString()
{
	//освободить память
	mpMemory->Release();
}

DWORD CInterpretationString::GetInterpretationType()
{
	return ADEIT_STRING;
}

// class CInterpretationStringUnicode

CInterpretationStringUnicode::CInterpretationStringUnicode(CCyberMemory* pMemory,CYBER_ADDRESS Address)
{
	//запомнить параметры
	mpMemory=pMemory;
	mpMemory->AddRef();
	mAddress=Address;
}

CInterpretationStringUnicode::~CInterpretationStringUnicode()
{
}

VOID CInterpretationStringUnicode::Print(CListing* pListing)
{
	//начать строку
	pListing->BeginLine();
	//получить отступ для строки
	UINT Margin=pListing->GetColumnMargin(LISTING_COLUMN_NAME);
	//получить максимальную длину строки
	UINT MaxLength=LISTING_LINE_LENGTH-Margin-0x10;

	//вывести заголовок
	pListing->Print(Margin,TEXT(".unicode \""));

	//флаг печати завершающей последовательности
	//цикл по символам
	CYBER_ADDRESS Address=mAddress;
	for(UINT i=10;i<=MaxLength;++i)
	{
		try
		{
			//получить символ
			WCHAR Chars[2];
			Chars[0]=mpMemory->Word(Address);
			Chars[1]=0;
			//если символ - конечный, то закончить
			if(!Chars[0])
			{
				pListing->Print(TEXT("\""));
				break;
			}
			//вывести символ, если мы не вышли за пределы строки
			if(i<MaxLength)
				pListing->Print(Chars);
			else
				//вывести многоточие
				pListing->Print(TEXT("\"..."));
		}
		catch(CCyberMemoryPageFaultException)
		{
			//напечатать символы окончания строки
			pListing->Print(TEXT("\"##"));
			//выйти из цикла
			break;
		}
		//перейти к следующему символу
		Address+=sizeof(WCHAR);
	}

	//закончить строку
	pListing->EndLine();
}

UINT CInterpretationStringUnicode::GetStringSize()
{
	//посчитать длину строки
	CYBER_ADDRESS i;
	for(i=mAddress;;i+=sizeof(WCHAR))
		try
		{
			//считать символ
			WCHAR Char=mpMemory->Word(i);
			//если символ нулевой, то это конец строки
			if(!Char) break;
		}
		catch(CCyberMemoryPageFaultException)
		{
			//мы дошли до нечитаемой памяти
			break;
		}
	//вернуть размер строки
	return i-mAddress+sizeof(WCHAR);
}

// class CInterpretationStringASCII

CInterpretationStringASCII::CInterpretationStringASCII(CCyberMemory* pMemory,CYBER_ADDRESS Address)
{
	//запомнить параметры
	mpMemory=pMemory;
	mpMemory->AddRef();
	mAddress=Address;
}

CInterpretationStringASCII::~CInterpretationStringASCII()
{
}

VOID CInterpretationStringASCII::Print(CListing* pListing)
{
	//начать строку
	pListing->BeginLine();
	//получить отступ для строки
	UINT Margin=pListing->GetColumnMargin(LISTING_COLUMN_NAME);
	//получить максимальную длину строки
	UINT MaxLength=LISTING_LINE_LENGTH-Margin-0x10;

	//вывести заголовок
	pListing->Print(Margin,TEXT(".ascii \""));

	//флаг печати завершающей последовательности
	//цикл по символам
	CYBER_ADDRESS Address=mAddress;
	for(UINT i=10;i<=MaxLength;++i)
	{
		try
		{
			//получить символ
			TCHAR Chars[2];
			Chars[0]=(TCHAR)mpMemory->Byte(Address);
			Chars[1]=0;
			//если символ - конечный, то закончить
			if(!Chars[0])
			{
				pListing->Print(TEXT("\""));
				break;
			}
			//вывести символ, если мы не вышли за пределы строки
			if(i<MaxLength)
				pListing->Print(Chars);
			else
				//вывести многоточие
				pListing->Print(TEXT("\"..."));
		}
		catch(CCyberMemoryPageFaultException)
		{
			//напечатать символы окончания строки
			pListing->Print(TEXT("\"##"));
			//выйти из цикла
			break;
		}
		//перейти к следующему символу
		Address+=sizeof(CHAR);
	}

	//закончить строку
	pListing->EndLine();
}

UINT CInterpretationStringASCII::GetStringSize()
{
	//посчитать длину строки
	CYBER_ADDRESS i;
	for(i=mAddress;;i+=sizeof(CHAR))
		try
		{
			//считать символ
			CHAR Char=mpMemory->Byte(i);
			//если символ нулевой, то это конец строки
			if(!Char) break;
		}
		catch(CCyberMemoryPageFaultException)
		{
			//мы дошли до нечитаемой памяти
			break;
		}
	//вернуть размер строки
	return i-mAddress+sizeof(CHAR);
}
