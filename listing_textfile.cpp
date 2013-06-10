#include "listing_textfile.h"

// class CListingTextFile

VOID CListingTextFile::SetFile(HANDLE hFile)
{
	//запомнить файл для вывода
	mhFile=hFile;
}

VOID CListingTextFile::EndLine()
{
	//вывести строку
	DWORD Written;
	WriteFile(mhFile,mszLineText,mLineTextLength*sizeof(TCHAR),&Written,NULL);
	//вывести символ перевода строки
	WriteFile(mhFile,TEXT("\r\n"),2*sizeof(TCHAR),&Written,NULL);
}
