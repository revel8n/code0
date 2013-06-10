#ifndef QS_LISTING_H
#define QS_LISTING_H

/*
Файл содержит определение класса печати, управляющего выводом на печать.
Класс CListing используется только для печати дизассемблерных листингов.

Модель печати следующая:
- документ состоит из строк. Начиначется и заканчивается строка
вызовами BeginLine() и EndLine() соответственно.
- каждая строка имеет адрес, который указывается вызовом SetAddress().
- строка формируется вызовами Print(), каждый вызов добавляет текст к соответствующей
колонке строки.
- если текст не влезает в размер колонки, он обрезается.
- контекст для рисования очищает вызывающая процедура. Текст печатается в режиме
с прозрачным фоном (TRANSPARENT) цветом, который был ранее указан.
Если используется флаг выделения строк, то для каждой строки рисуется фоновый прямоугольник
текущей кистью. Для того, чтобы сгруппировать выделенные строки,
вызывается GroupSelectLines(), и рисуется объединяющий прямоугольник, с нулевой кистью
и текущим пером.
*/

#include "object.h"

//колонки строки для стандартного вывода листинга
#define LISTING_COLUMN_ADDRESS					0
#define LISTING_COLUMN_LABEL						1
#define LISTING_COLUMN_NAME							2
#define LISTING_COLUMN_OPERANDS					3
#define LISTING_COLUMN_COMMENTS					4
//количество колонок
#define LISTING_COLUMNS_COUNT						5
//максимальная длина строки
#define LISTING_LINE_LENGTH							0x80

class CAddressMap;

class CListing : public CObject
{
protected:
	static const UINT mColumnMargins[LISTING_COLUMNS_COUNT];	//массив информации о колонках
	TCHAR mszLineText[LISTING_LINE_LENGTH];				//текст строки
	UINT mLineTextLength;													//длина уже напечатанного текста строки
	UINT mLineCurrentPosition;										//текущее положение курсора
	DWORD mAddress;																//текущий адрес

	CAddressMap* mpAddressMap;										//указатель на карту адресов

public:
	CListing();
	virtual ~CListing();

	VOID SetAddressMap(CAddressMap* pAddressMap);	//указать карту адресов

	VOID SetAddress(DWORD Address);								//указать текущий адрес
	VOID BeginLine();															//начать строку
	virtual VOID EndLine() PURE;									//закончить строку

	//функции печати
	UINT Print(UINT Position,LPCTSTR szText);			//добавить текст, начиная с заданной позиции (возвращается позиция после конца)
	UINT Print(LPCTSTR szText);										//добавить текст, начиная с текущей позиции (возвращается позиция после конца)
	VOID SetPosition(UINT Position);							//установить текущее положение курсора
	UINT GetColumnMargin(DWORD ColumnIndex);			//получить отступ слева нужной колонки
	virtual UINT PrintOffset(UINT Address);				//добавить текстовое представление адреса
};

#endif
