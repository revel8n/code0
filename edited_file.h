#ifndef QS_EDIT_FILE_H
#define QS_EDIT_FILE_H

/*
‘айл содержит определение класса, управл€ющего загрузкой и редактированием файлов.
*/

#include "object.h"

class CEditedFile : public CObject
{
protected:
	//им€ файла
	TCHAR mszFileName[MAX_PATH];
	//указатель на пам€ть проекции файла
	PVOID mpFileMemory;
	//размер файла
	UINT mFileSize;

	//конструктор
	CEditedFile();

public:
	//открыть файл
	static CEditedFile* Open(LPCTSTR szFile);

	//деструктор
	virtual ~CEditedFile();

	//получить им€ файла
	LPCTSTR GetFileName();

	//получить указатель на пам€ть файла
	PVOID GetMemory();
	//получить размер файла
	UINT GetSize();
};

#endif
