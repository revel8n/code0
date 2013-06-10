#include "subroutine.h"

// class CSubroutine

CSubroutine::CSubroutine()
{
	//пока ничего не делать
}

DWORD CSubroutine::GetElementType()
{
	return ADET_SUBROUTINE;
}

VOID CSubroutine::Print(CListing* pListing)
{
	//вывести заголовок функции
	pListing->BeginLine();
	pListing->EndLine();
	pListing->BeginLine();
	pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL)+10,TEXT("; *******************************************************************"));
	pListing->EndLine();
	pListing->BeginLine();
	pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL)+10,TEXT("; ***                         SUBROUTINE                          ***"));
	pListing->EndLine();
	pListing->BeginLine();
	pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL)+10,TEXT("; *******************************************************************"));
	pListing->EndLine();
	pListing->BeginLine();
	pListing->EndLine();
}
