#ifndef QS_SUBROUTINE_H
#define QS_SUBROUTINE_H

/*
Файл содержит определение функции - элемента карты данных адреса,
который содержит информацию о процедуре кода.
*/

#include "address_data_element.h"

class CSubroutine : public CAddressDataElement
{
protected:
	//пока без параметров

public:
	//конструктор
	CSubroutine();

	//перегруженные функции
	virtual DWORD GetElementType();										//получить тип элемента данных адреса
	virtual VOID Print(CListing*);										//напечатать текстовое представление элемента данных
};

#endif
