#include "listing_string.h"

// class CListingString

LPCTSTR CListingString::GetString()
{
	//вернуть указатель на строку
	return mszLineText;
}

VOID CListingString::EndLine()
{
	//установить завершающий символ
	mszLineText[mLineTextLength]=0;
}
