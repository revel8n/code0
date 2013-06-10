#include "xreference.h"
#include "disassembly.h"

// class CXReference realization

CXReference::CXReference(CYBER_ADDRESS AddressFrom,BYTE ReferenceFlags)
{
	mAddressFrom=AddressFrom;
	mReferenceFlags=ReferenceFlags;
}

DWORD CXReference::GetElementType()
{
	return ADET_XREFERENCE;
}

VOID CXReference::Print(CListing* pListing)
{
	//сформировать текст

	//начать строку
	pListing->BeginLine();

	//установить положение курсора
	pListing->SetPosition(pListing->GetColumnMargin(LISTING_COLUMN_COMMENTS));
	//вывести символ комментари€
	pListing->Print(TEXT("; "));

	//напечатать флаги метки
	if(mReferenceFlags & OIR_REFERENCE_READ)
		pListing->Print(TEXT("read "));
	if(mReferenceFlags & OIR_REFERENCE_WRITE)
		pListing->Print(TEXT("write "));
	if(mReferenceFlags & OIR_REFERENCE_EXECUTE_CALL)
		pListing->Print(TEXT("call "));
	if(mReferenceFlags & OIR_REFERENCE_EXECUTE)
		pListing->Print(TEXT("execute "));

	//напечатать по€сн€ющую строку
	pListing->Print(TEXT("reference from "));
	//напечатать адрес
	pListing->PrintOffset(mAddressFrom);

	//вывести строку
	pListing->EndLine();
}

CYBER_ADDRESS CXReference::GetAddressFrom()
{
	//вернуть обратный адрес
	return mAddressFrom;
}
