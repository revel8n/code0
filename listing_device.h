#ifndef QS_LISTING_DEVICE_H
#define QS_LISTING_DEVICE_H

/*
Файл содержит определение объекта-листинга, осуществляющего
вывод на контекст DC.

Листинг поддерживает флаг контрольной группы, которая учитывает
все адреса, выводимые в ней, и выделяет соответствующие строки.
Флаг контрольной группы автоматически сбрасывается после вызова
метода GroupLines().
*/

#include "listing.h"
#include <set>

//структура информации контекста печати
struct LISTING_DEVICE_CONTEXT
{
	HDC hdc;																			//контекст для рисования
	RECT Rect;																		//ограничивающий прямоугольник
};

class CListingDevice : public CListing
{
protected:
	LISTING_DEVICE_CONTEXT mContext;							//параметры контекста печати
	SIZE mCharSize;																//размеры одного символа

	INT mPrintedCount;														//количество уже напечатанных строк в этом контексте
	INT mGroupCount;															//количество строк в группе
	BOOL mSelect;																	//флаг выделения строк
	BOOL mControlGroup;														//флаг контрольной группы

	std::set<UINT> mVisesSet;											//набор выделяемых адресов

	static HPEN mhDashPen;												//пунктирное перо для выделения ссылочных адресов

public:
	//конструктор
	CListingDevice();

	//указать флаг выделения
	VOID SetSelecting(BOOL Select);
	//указать текущий контекст
	VOID SetContext(LISTING_DEVICE_CONTEXT Context);

	//указать флаг контрольной группы
	VOID SetControlGrouping();

	//закончить строку
	virtual VOID EndLine();
	//сгруппировать последние несколько строк, начиная с последнего вызова SetAddress
	VOID GroupLines();

	//получить, есть ли переполнение области вывода по количеству строк
	BOOL IsOverflow();

	//добавить текстовое представление адреса
	//метод перегружен из CListing для поддержки контрольных групп
	virtual UINT PrintOffset(UINT Address);
};

#endif
