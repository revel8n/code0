#include "comment.h"

// class CComment

CComment::CComment(LPCTSTR szText)
{
	//запомнить текст
	mszText=new TCHAR[_tcslen(szText)+1];
	_tcscpy(mszText,szText);
}

CComment::~CComment()
{
	//удалить текст
	delete [] mszText;
}

LPCTSTR CComment::GetText()
{
	return mszText;
}

DWORD CComment::GetElementType()
{
	return ADET_COMMENT;
}

VOID CComment::Print(CListing* pListing)
{
	//указатель на текущую часть строки
	LPTSTR szText=mszText;
	//цикл вывода строк комментария
	while(*szText)
	{
		//начать строку
		pListing->BeginLine();

		//найти символ перевода строки
		LPTSTR szEnd;
		for(szEnd=szText;*szEnd;++szEnd)
			if(*szEnd==TEXT('\r') || *szEnd==TEXT('\n'))
				break;
		//запомнить старый символ
		TCHAR OldChar=*szEnd;
		//указать конец строки
		*szEnd=0;
		//вывести строку
		pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_COMMENTS),TEXT("; "));
		pListing->Print(szText);
		//вернуть старый символ
		*szEnd=OldChar;
		//закончить строку
		pListing->EndLine();

		//если это не конец строки
		if(OldChar)
			//проверить следующий символ - может это парный к \r
			if(szEnd[0]==TEXT('\r') && szEnd[1]==TEXT('\n'))
				//перейти к следующей строке
				szText=szEnd+2;
			else
				szText=szEnd+1;
		//иначе закончить
		else
			break;
	}
}
