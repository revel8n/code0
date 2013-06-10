#include "listing.h"
#include "address_map.h"

// class CListing

const UINT CListing::mColumnMargins[LISTING_COLUMNS_COUNT]={ 0, 10, 32, 48, 40 };

CListing::CListing()
{
	//инциализировать указатель на карту адресов
	mpAddressMap=NULL;
}

CListing::~CListing()
{
	//освободить карту адресов
	SafeRelease(mpAddressMap);
}

VOID CListing::SetAddress(DWORD Address)
{
	//запомнить текущий адрес
	mAddress=Address;
}

VOID CListing::SetAddressMap(CAddressMap* pAddressMap)
{
	//освободить прошлую карту адресов
	SafeRelease(mpAddressMap);
	//запомнить указатель на новую карту адресов
	mpAddressMap=pAddressMap;
	mpAddressMap->AddRef();
}

VOID CListing::BeginLine()
{
	//очистить строку
	mLineTextLength=0;
	//сбросить указатель
	mLineCurrentPosition=0;
	//напечатать адрес в начале строки
	TCHAR s[0x10];
	_stprintf(s,TEXT("%08X:"),mAddress);
	Print(0,s);
}

UINT CListing::Print(UINT Position,LPCTSTR szText)
{
	//получить длину добавляемого текста
	UINT TextLength=_tcslen(szText);

	//если положение выходит за рамки максимальное длины строки, то закончить
	if(Position>=LISTING_LINE_LENGTH) return LISTING_LINE_LENGTH;
	//если длина выходит за рамки максимальной длины строки, обрезать строку
	if(Position+TextLength>=LISTING_LINE_LENGTH) TextLength=LISTING_LINE_LENGTH-Position;
	//если строка добавляется правее её нынешнего правого края
	if(Position>mLineTextLength)
		//заполнить пробелами это пространство
		for(UINT i=mLineTextLength;i<Position;++i)
			mszLineText[i]=TEXT(' ');

	//добавить строку
	CopyMemory(mszLineText+Position,szText,TextLength*sizeof(TCHAR));
	//вычислить новое текущее положение курсора
	mLineCurrentPosition=Position+TextLength;
	if(mLineCurrentPosition>mLineTextLength) mLineTextLength=mLineCurrentPosition;
	//вернуть новую позицию
	return mLineCurrentPosition;
}

UINT CListing::Print(LPCTSTR szText)
{
	//напечатать текст, начиная с текущей позиции, и вернуть новую текущую позицию
	return Print(mLineCurrentPosition,szText);
}

VOID CListing::SetPosition(UINT Position)
{
	//если положение выходит за рамки максимальное длины строки, то закончить
	if(Position>=LISTING_LINE_LENGTH) return;
	//установить текущее положение курсора
	mLineCurrentPosition=Position;
}

UINT CListing::GetColumnMargin(DWORD ColumnIndex)
{
	//если номер колонки неверен, закончить
	if(ColumnIndex>=LISTING_COLUMNS_COUNT) return 0;

	//вывести ответ
	return mColumnMargins[ColumnIndex];
}

UINT CListing::PrintOffset(UINT Address)
{
#ifdef NIKITIN
	if(0)
#endif
	//если указана карта адресов
	if(mpAddressMap)
	{
		//получить строку для адреса и вывести его
		LPTSTR szAddress=mpAddressMap->GetAddressString(Address);
		UINT Result=Print(szAddress);
		delete [] szAddress;
		return Result;
	}

	//иначе просто вывести адрес
	TCHAR szAddress[0x10];
	if(Address & 0x80000000)
		_stprintf(szAddress,TEXT("-0%X"),-(int)Address);
	else
		_stprintf(szAddress,TEXT("0%X"),Address);
//	_stprintf(szAddress,TEXT("0%+Xh"),Address);
	return Print(szAddress);
}
