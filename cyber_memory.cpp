#include "cyber_memory.h"

// class CCyberMemoryPageFaultException

CCyberMemoryPageFaultException::CCyberMemoryPageFaultException(CYBER_ADDRESS Address)
{
	mAddress=Address;
}

// class CCyberMemory realization

CCyberMemory::CCyberMemory()
{
	//выделить память для каталога страниц
	mpCatalogue=(PVOID*)VirtualAlloc(NULL,MEMORY_PAGES_COUNT*sizeof(PVOID),MEM_RESERVE | MEM_COMMIT,PAGE_READWRITE);
}

CCyberMemory::~CCyberMemory()
{
	//освободить память каталога страниц
	VirtualFree(mpCatalogue,0,MEM_RELEASE);
}

PVOID CCyberMemory::Translate(CYBER_ADDRESS Address)
{
	//получить реальный адрес страницы
	PVOID PageRealAddress=mpCatalogue[MEMORY_ADDRESS_GET_PAGE(Address)];
	//если страница существует
	if(PageRealAddress)
		//вернуть реальный адрес
		return ((PBYTE)PageRealAddress+MEMORY_ADDRESS_GET_OFFSET(Address));
	//иначе страница не выделена, возбудить исключение
	throw CCyberMemoryPageFaultException(Address);
}

//получить байт из киберпамяти
BYTE CCyberMemory::Byte(CYBER_ADDRESS Address)
{
	//получить байт
	return *(PBYTE)Translate(Address);
}

//получить слово из киберпамяти
WORD CCyberMemory::Word(CYBER_ADDRESS Address)
{
	//получить слово как 2 байта
	return (*(PBYTE)Translate(Address)) | ((*(PBYTE)Translate(Address+1))<<8);
}

//получить двойное слово из киберпамяти
DWORD CCyberMemory::Dword(CYBER_ADDRESS Address)
{
	//получить двойное слово как 4 байта
	return (*(PBYTE)Translate(Address)) | ((*(PBYTE)Translate(Address+1))<<8) | ((*(PBYTE)Translate(Address+2))<<16) | ((*(PBYTE)Translate(Address+3))<<24);
}

//считать произвольное количество данных
VOID CCyberMemory::Data(PVOID pBuffer,CYBER_ADDRESS Address,DWORD Size)
{
	//цикл по количеству данных
	for(DWORD i=0;i<Size;++i)
		//считать байт данных
		((PBYTE)pBuffer)[i]=Byte(Address+i);
}

//считать ASCIIZ-строку
LPSTR CCyberMemory::ReadASCIIZ(CYBER_ADDRESS Address)
{
	//запомнить начало строки
	CYBER_ADDRESS BeginAddress=Address;
	//вычислить длину строки
	for(;*(PCHAR)Translate(Address);Address++);
	//выделить память
	UINT Length=Address-BeginAddress+1;
	LPSTR szString=new CHAR[Length];
	//считать строку
	Data(szString,BeginAddress,Length);
	//вернуть строку
	return szString;
}

// class CCyberMemoryAccessor

CCyberMemoryAccessor::CCyberMemoryAccessor(CCyberMemory* pMemory)
{
	//запомнить указатель на киберпространство
	mpMemory=pMemory;
	mpMemory->AddRef();
	//инициализировать текущий адрес
	mAddress=0;
}

CCyberMemoryAccessor::~CCyberMemoryAccessor()
{
	//освободить киберпамять
	mpMemory->Release();
}

VOID CCyberMemoryAccessor::SetPointer(CYBER_ADDRESS Address)
{
	//установить текущий адрес
	mAddress=Address;
}

CYBER_ADDRESS CCyberMemoryAccessor::GetPointer()
{
	//вернуть текущий адрес
	return mAddress;
}

CCyberMemory* CCyberMemoryAccessor::Memory()
{
	mpMemory->AddRef();
	return mpMemory;
}

//получить текущий байт из киберпамяти
BYTE CCyberMemoryAccessor::CurrentByte()
{
	//считать текущий байт
	BYTE Data=mpMemory->Byte(mAddress);
	//увеличить счетчик текущего адреса
	mAddress++;
	//вернуть байт
	return Data;
}

//считать текущее слово из киберпамяти
WORD CCyberMemoryAccessor::CurrentWord()
{
	//считать текущее слово
	WORD Data=mpMemory->Word(mAddress);
	//увеличить счетчик текущего адреса
	mAddress+=2;
	//вернуть слово
	return Data;
}

//считать текущее двойное слово из киберпамяти
DWORD CCyberMemoryAccessor::CurrentDword()
{
	//считать текущее двойное слово
	DWORD Data=mpMemory->Dword(mAddress);
	//увеличить счетчик текущего адреса
	mAddress+=4;
	//вернуть двойное слово
	return Data;
}

//считать текущие данные заданного размера
DWORD CCyberMemoryAccessor::CurrentData(BYTE Size)
{
	//выбор по размеру данных
	switch(Size)
	{
	case 1:
		return CCyberMemoryAccessor::CurrentByte();
	case 2:
		return CCyberMemoryAccessor::CurrentWord();
	case 4:
		return CCyberMemoryAccessor::CurrentDword();
	default:
		return 0;
	}
}

//считать текущие данные заданного размера с учетом знака
DWORD CCyberMemoryAccessor::CurrentSignedData(BYTE Size)
{
	//выбор по размеру данных
	switch(Size)
	{
	case 1:
		{
			BYTE Data=CurrentByte();
			//если данные отрицательны
			if(Data & 0x80)
				//корректно преобразовать в Dword
				return 0-(DWORD)(0x100-Data);
			//иначе преобразование не требуется
			return (DWORD)Data;
		}
	case 2:
		{
			WORD Data=CurrentWord();
			//если данные отрицательны
			if(Data & 0x8000)
				//корректно преобразовать в Dword
				return 0-(DWORD)(0x10000-Data);
			//иначе преобразование не требуется
			return (DWORD)Data;
		}
	case 4:
		//просто считать Dword
		return CurrentDword();
	default:
		return 0;
	}
}

//спроецировать реальную память на киберпамять
BOOL CCyberMemory::Map(CYBER_ADDRESS Address,PVOID pBuffer,DWORD Size)
{
	//если адрес или размер не кратны размеру страницы, закончить
	if((Address%MEMORY_PAGE_SIZE) || (Size%MEMORY_PAGE_SIZE)) return FALSE;

	//выразить адрес в страницах
	Address/=MEMORY_PAGE_SIZE;
	//выразить размер в страницах
	Size/=MEMORY_PAGE_SIZE;

	//проверить, не выходит ли выделяемый блок за границы киберпространства
	if(Address+Size>MEMORY_PAGES_COUNT) return FALSE;

	//вычислить конечную границу блока вместо размера
	Size+=Address;

	//проверить, выделена ли эта память
	DWORD i;
	for(i=Address;i<Size;++i)
		//если хоть одна страница выделена, закончить
		if(mpCatalogue[i]) return FALSE;

	//спроецировать память
	for(i=Address;i<Size;++i)
		//спроецировать страницу
		mpCatalogue[i]=(PVOID)((PBYTE)pBuffer+(i-Address)*MEMORY_PAGE_SIZE);

	//всё!
	return TRUE;
}

//отключить проецирование
BOOL CCyberMemory::Unmap(CYBER_ADDRESS Address,DWORD Size)
{
	//если адрес или размер не кратны размеру страницы, закончить
	if((Address%MEMORY_PAGE_SIZE) || (Size%MEMORY_PAGE_SIZE)) return FALSE;

	//выразить адрес в страницах
	Address/=MEMORY_PAGE_SIZE;
	//выразить размер в страницах
	Size/=MEMORY_PAGE_SIZE;

	//вычислить конечную границу блока вместо размера
	Size+=Address;

	//отключить проецирование памяти
	for(DWORD i=Address;i<Size;++i)
		//отключить проецирование страницы
		mpCatalogue[i]=NULL;

	//всё!
	return TRUE;
}

//очистить киберпамять полностью
VOID CCyberMemory::Clear()
{
	//отключить проецирование всех страниц
	for(DWORD i=0;i<MEMORY_PAGES_COUNT;++i)
		mpCatalogue[i]=NULL;
}
