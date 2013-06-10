#ifndef QS_LISTING_TEXTFILE_H
#define QS_LISTING_TEXTFILE_H

/*
‘айл содержит определение объекта-листинга, выполн€ющего
печать листинга в файл.
*/

#include "listing.h"

class CListingTextFile : public CListing
{
protected:
	//файл дл€ вывода
	HANDLE mhFile;

public:
	VOID SetFile(HANDLE hFile);										//указать файл дл€ вывода

	virtual VOID EndLine();												//закончить строку
};

#endif
