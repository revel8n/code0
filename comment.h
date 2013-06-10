#ifndef QS_COMMENT_H
#define QS_COMMENT_H

/*
‘айл содержит объ€вление класса комментари€.
*/

#include "address_data_element.h"

class CComment : public CAddressDataElement
{
protected:
	LPTSTR mszText;																		//указатель на текст комментари€

public:
	CComment(LPCTSTR szText);
	virtual ~CComment();

	LPCTSTR GetText();																//получить указатель на текст комментари€ (действителен, только пока комментарий существует)

	virtual DWORD GetElementType();										//получить тип элемента данных адреса
	virtual VOID Print(CListing*);										//напечатать текстовое представление элемента данных
};

#endif
