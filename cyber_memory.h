#ifndef QS_CYBER_MEMORY_H
#define QS_CYBER_MEMORY_H

// Файл содержит объявление класса CCyberMemory, управляющего адресным киберпространством.

#include "object.h"

// МАКРОСЫ

//размер собственной страницы киберпамяти
#define MEMORY_PAGE_SIZE														0x1000

//количество страниц киберпамяти
#define MEMORY_PAGES_COUNT													0x100000

//макрос для получения номера страницы из адреса
#define MEMORY_ADDRESS_GET_PAGE(Address)						( (Address) / MEMORY_PAGE_SIZE )
//макрос для получения смещения внутри страницы
#define MEMORY_ADDRESS_GET_OFFSET(Address)					( (Address) % MEMORY_PAGE_SIZE )

//макросы округления
#define ALIGN_UP(Address,Align) (((Address)+(Align)-1)/(Align)*(Align))
#define ALIGN_DOWN(Address,Align) ((Address)/(Align)*(Align))

// ТИПЫ

typedef DWORD CYBER_ADDRESS;

// КЛАССЫ

class CCyberMemoryPageFaultException
{
public:
	CYBER_ADDRESS mAddress;

	CCyberMemoryPageFaultException(CYBER_ADDRESS Address);
};

class CCyberMemory : public CObject
{
protected:
	//указатель на каталог страниц
	PVOID* mpCatalogue;

public:
	CCyberMemory();
	virtual ~CCyberMemory();

	//получить реальный адрес из киберадреса
	PVOID Translate(CYBER_ADDRESS Address);

	//получить байт
	BYTE Byte(CYBER_ADDRESS Address);
	//получить слово
	WORD Word(CYBER_ADDRESS Address);
	//получить двойное слово
	DWORD Dword(CYBER_ADDRESS Address);
	//считать произвольное количество данных
	VOID Data(PVOID pBuffer,CYBER_ADDRESS Address,DWORD Size);
	//считать ASCIIZ-строку (память должна быть освобождена вызывающей стороной с помощью delete [])
	LPSTR ReadASCIIZ(CYBER_ADDRESS Address);

	//спроецировать реальную память на киберпамять
	BOOL Map(CYBER_ADDRESS Address,PVOID pBuffer,DWORD Size);
	//отключить проецирование памяти
	BOOL Unmap(CYBER_ADDRESS Address,DWORD Size);
	//очистить киберпамять, сбросив все проекции
	VOID Clear();
};

class CCyberMemoryAccessor : public CObject
{
protected:
	//указатель на киберпространство
	CCyberMemory* mpMemory;
	//текущий адрес
	CYBER_ADDRESS mAddress;

public:
	CCyberMemoryAccessor(CCyberMemory* pMemory);
	virtual ~CCyberMemoryAccessor();

	//управление указателем текущего места
	VOID SetPointer(CYBER_ADDRESS Address);
	CYBER_ADDRESS GetPointer();

	//получение прямого указателя на киберпространство
	CCyberMemory* Memory();

	//получить текущий байт
	BYTE CurrentByte();
	//получить текущее слово
	WORD CurrentWord();
	//получить текущее двойное слово
	DWORD CurrentDword();
	//получить текущие данные
	DWORD CurrentData(BYTE Size);
	//получить текущие данные с учетом знака
	DWORD CurrentSignedData(BYTE Size);
};

#endif
