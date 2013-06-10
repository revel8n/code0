#ifndef QS_ADDRESS_DATA_ELEMENT_H
#define QS_ADDRESS_DATA_ELEMENT_H

/*
Файл содержит определение абстрактного класса элемента данных адреса.
*/

#include "object.h"
#include "listing.h"

// типы элементов данных адреса
// порядок перечисления типов элементов определяет порядок их вывода в листингах
#define ADET_COMMENT											0
#define ADET_XREFERENCE										1
#define ADET_SUBROUTINE										2
#define ADET_LABEL												3
#define ADET_INTERPRETATION								4

//элемент данных адреса
class CAddressDataElement : public CObject
{
public:
	virtual ~CAddressDataElement();										//деструктор

	virtual DWORD GetElementType() PURE;							//получить тип элемента данных адреса
	virtual VOID Print(CListing*) PURE;								//напечатать текстовое представление элемента данных
};

#endif
