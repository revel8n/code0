#include "database.h"

// class CDatabase

CDatabase::CDatabase()
{
}

CDatabase::~CDatabase()
{
	//закрыть файл
	if(mhFile)
	{
		CloseHandle(mhFile);
		mhFile=NULL;
	}
}

CDatabase* CDatabase::Create(LPCTSTR szFile)
{
	//создать файл
	HANDLE hFile=CreateFile(szFile,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) return NULL;

	//записать заголовок
	FILE_DATABASE_HEADER Header;
	Header.Magic[0]=FILE_DATABASE_MAGIC1;
	Header.Magic[1]=FILE_DATABASE_MAGIC2;
	DWORD Written;
	if(!WriteFile(hFile,&Header,sizeof(Header),&Written,NULL) || Written<sizeof(Header))
	{
		CloseHandle(hFile);
		return NULL;
	}

	//создать объект класса
	CDatabase* pDatabase=new CDatabase;
	pDatabase->mhFile=hFile;
	return pDatabase;
}

CDatabase* CDatabase::Open(LPCTSTR szFile)
{
	//открыть файл
	HANDLE hFile=CreateFile(szFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) return NULL;

	//создать объект класса
	CDatabase* pDatabase=new CDatabase;
	pDatabase->mhFile=hFile;
	return pDatabase;
}

BOOL CDatabase::ReadData(PVOID pData,DWORD Size)
{
	//считать данные
	DWORD Read;
	return ReadFile(mhFile,pData,Size,&Read,NULL) && Size==Read;
}

BOOL CDatabase::ReadValue(PDWORD Value)
{
	//считать число
	DWORD Read;
	return ReadFile(mhFile,Value,sizeof(DWORD),&Read,NULL) && Read==sizeof(DWORD);
}

BOOL CDatabase::ReadValue(PVOID* Value)
{
	//считать число
	DWORD Read;
	return ReadFile(mhFile,Value,sizeof(PVOID),&Read,NULL) && Read==sizeof(PVOID);
}

BOOL CDatabase::WriteData(const PVOID pData,DWORD Size)
{
	//записать данные
	DWORD Written;
	return WriteFile(mhFile,pData,Size,&Written,NULL) && Size==Written;
}

BOOL CDatabase::WriteValue(DWORD Value)
{
	//записать число
	DWORD Written;
	return WriteFile(mhFile,&Value,sizeof(DWORD),&Written,NULL) && Written==sizeof(DWORD);
}

BOOL CDatabase::WriteValue(PVOID Value)
{
	//записать число
	DWORD Written;
	return WriteFile(mhFile,&Value,sizeof(PVOID),&Written,NULL) && Written==sizeof(PVOID);
}

BOOL CDatabase::ReadObject(CObject** pObject)
{
	//считать идентификатор объекта
	DWORD ID,Read;
	if(!ReadFile(mhFile,&ID,sizeof(ID),&Read,NULL) || Read<sizeof(ID)) return FALSE;

	//если объект ещё не был считан
	std::map<DWORD,CObject*>::iterator i=mObjectsMap.find(ID);
	if(i==mObjectsMap.end())
	{
		//добавить объект в таблицу
		mObjectsMap.insert(std::make_pair(ID,*pObject));
		//загрузить объект
		return (*pObject)->ReadFromDatabase(this);
	}
	//иначе объект уже был считан
	else
	{
		//вернуть объект
		delete *pObject;
		*pObject=(*i).second;
		return TRUE;
	}
}

BOOL CDatabase::WriteObject(const CObject* pObject)
{
	//записать идентификатор объекта (равный его адресу)
	DWORD ID,Written;
	ID=(DWORD)pObject;
	if(!WriteFile(mhFile,&ID,sizeof(ID),&Written,NULL) || Written<sizeof(ID)) return FALSE;

	//если объект ещё не был сохранён
	if(mObjectsMap.find((DWORD)pObject)==mObjectsMap.end())
	{
		//добавить объект в карту
		mObjectsMap.insert(std::make_pair((DWORD)pObject,(CObject*)pObject));
		//записать объект
		return pObject->WriteToDatabase(this);
	}
	//иначе объект уже был сохранён
	else
		//ещё раз не записывать
		return TRUE;
}

BOOL CDatabase::UpdatePointers()
{
	//цикл по объектам
	for(std::map<DWORD,CObject*>::iterator i=mObjectsMap.begin();i!=mObjectsMap.end();i++)
		//обновить указатели у объекта
		if((*i).second->UpdatePointersWithDatabase(this)) return FALSE;
	return TRUE;
}

CObject* CDatabase::RetrieveObject(DWORD ID)
{
	//получить объект
	std::map<DWORD,CObject*>::iterator i=mObjectsMap.find(ID);
	if(i!=mObjectsMap.end())
		return (*i).second;
	return NULL;
}
