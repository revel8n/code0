#ifndef QS_DATABASE_H
#define QS_DATABASE_H

/*
‘айл содержит определени€ типов, используемых в файлах баз данных дизассемблера, а
также класс, управл€ющий сохранением и загрузкой файлов.
*/

#include "object.h"
#include <map>

/*
‘ормат базы данных дизассемблера:

+----------------------+--------------+
| FILE_DATABASE_HEADER | некие данные |
+----------------------+--------------+
*/

//сигнатура файла
#define FILE_DATABASE_MAGIC1							'edoC'
#define FILE_DATABASE_MAGIC2							'\0bd0'

//заголовок файла
struct FILE_DATABASE_HEADER
{
	//сигнатура файла
	DWORD Magic[2];
};

class CDatabase
{
protected:
	//описатель файла базы данных
	HANDLE mhFile;
	//карта объектов (ключ - идентификатор в файле, содержимое - адрес объекта)
	std::map<DWORD,CObject*> mObjectsMap;

	//конструктор
	CDatabase();

public:
	//деструктор
	~CDatabase();

	//создать базу данных
	static CDatabase* Create(LPCTSTR szFile);
	//открыть базу данных
	static CDatabase* Open(LPCTSTR szFile);

	//считать данные
	BOOL ReadData(PVOID pData,DWORD Size);
	BOOL WriteData(const PVOID pData,DWORD Size);
	BOOL ReadValue(PDWORD Value);
	BOOL ReadValue(PVOID* Value);
	BOOL WriteValue(DWORD Value);
	BOOL WriteValue(PVOID Value);

	//считать объект
	BOOL ReadObject(CObject** pObject);
	//записать объект
	BOOL WriteObject(const CObject* pObject);
	//обновить указатели у объектов
	BOOL UpdatePointers();
	//получить указатель на объект по его идентификатору
	CObject* RetrieveObject(DWORD ID);
};

#endif
