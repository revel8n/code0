#ifndef QS_LISTING_HTML_H
#define QS_LISTING_HTML_H

/*
‘айл содержит определение объекта-листинга, выполн€ющего
печать листинга в файл формата HTML.
*/

#include "listing.h"

class CListingHTML : public CListing
{
protected:
	//файл дл€ вывода
	HANDLE mhFile;

public:
	VOID SetFile(HANDLE hFile);										//указать файл дл€ вывода
	VOID BeginFile();															//начать вывод листинга
	VOID EndFile();																//закончить вывод листинга

	virtual VOID EndLine();												//закончить строку
	virtual UINT PrintOffset(UINT Address);				//добавить текстовое представление адреса
};

#endif
