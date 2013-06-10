#ifndef QS_LISTING_STRING_H
#define QS_LISTING_STRING_H

/*
‘айл содержит определение объекта-листинга, выполн€ющего
печать листинга в строку.
*/

#include "listing.h"

class CListingString : public CListing
{
public:
	//получить строку с выводом
	LPCTSTR GetString();

	virtual VOID EndLine();												//закончить строку
};

#endif
