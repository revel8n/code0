#include "listing_html.h"

// class CListingHTML

VOID CListingHTML::SetFile(HANDLE hFile)
{
	//запомнить файл для вывода
	mhFile=hFile;
}

VOID CListingHTML::BeginFile()
{
	//вывести заголовок HTML
	DWORD Written;
	static TCHAR szHeader[]=TEXT("<HTML><HEAD><TITLE>Listing of file</TITLE></HEAD><BODY>");
	WriteFile(mhFile,szHeader,sizeof(szHeader),&Written,NULL);
}

VOID CListingHTML::EndFile()
{
	//закончить листинг
	DWORD Written;
	static TCHAR szEnd[]=TEXT("</BODY></HTML>");
	WriteFile(mhFile,szEnd,sizeof(szEnd),&Written,NULL);
}

VOID CListingHTML::EndLine()
{
	//вывести строку
	DWORD Written;
	WriteFile(mhFile,mszLineText,mLineTextLength*sizeof(TCHAR),&Written,NULL);
	//вывести символ перевода строки
	WriteFile(mhFile,TEXT("\r\n"),2*sizeof(TCHAR),&Written,NULL);
}
