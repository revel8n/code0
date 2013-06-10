#include "label.h"

// class CLabel

CLabel::CLabel(LPCTSTR szName)
{
	mszName=new TCHAR[_tcslen(szName)+1];
	_tcscpy(mszName,szName);
}

CLabel::~CLabel()
{
	delete [] mszName;
}

LPCTSTR CLabel::GetName()
{
	return mszName;
}

DWORD CLabel::GetElementType()
{
	return ADET_LABEL;
}

VOID CLabel::Print(CListing* pListing)
{
#ifdef NIKITIN
	return;
#endif
	//начать строку
	pListing->BeginLine();

	//вывести метку
	pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_LABEL),mszName);
	pListing->Print(TEXT(":"));

	//закончить строку
	pListing->EndLine();
}
