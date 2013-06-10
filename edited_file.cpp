#include "edited_file.h"

// class CEditedFile

CEditedFile::CEditedFile()
{
	//инициализация
	mpFileMemory=NULL;
}

CEditedFile::~CEditedFile()
{
	SafeUnmapViewOfFile(mpFileMemory);
}

CEditedFile* CEditedFile::Open(LPCTSTR szFile)
{
	//открыть и спроецировать файл
	HANDLE hFile=CreateFile(szFile,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) return NULL;
	UINT Size=GetFileSize(hFile,NULL);
	HANDLE hMapping=CreateFileMapping(hFile,NULL,PAGE_READWRITE,0,0,NULL);
	CloseHandle(hFile);
	PVOID pMemory=MapViewOfFile(hMapping,FILE_MAP_WRITE,0,0,0);
	CloseHandle(hMapping);
	if(!pMemory) return NULL;

	//создать и вернуть объект
	CEditedFile* pFile=new CEditedFile;
	pFile->mpFileMemory=pMemory;
	pFile->mFileSize=Size;
	_tcscpy(pFile->mszFileName,szFile);
	return pFile;
}

LPCTSTR CEditedFile::GetFileName()
{
	return mszFileName;
}

PVOID CEditedFile::GetMemory()
{
	return mpFileMemory;
}

UINT CEditedFile::GetSize()
{
	return mFileSize;
}
