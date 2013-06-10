#include "image_list_imports.h"

// class CImageListImports

// subclass CImportFunction

CImageListImports::CImportFunction::CImportFunction(LPCTSTR szName,CYBER_ADDRESS AddressInIAT)
{
	//запомнить параметры
	mszName=new TCHAR[_tcslen(szName)+1];
	_tcscpy(mszName,szName);
	mAddressInIAT=AddressInIAT;
}

CImageListImports::CImportFunction::~CImportFunction()
{
	//освободить память строки-имени
	SafeDeleteMassive(mszName);
}

LPCTSTR CImageListImports::CImportFunction::GetName()
{
	return mszName;
}

CYBER_ADDRESS CImageListImports::CImportFunction::GetAddressInIAT()
{
	return mAddressInIAT;
}

// subclass CImportLibrary

CImageListImports::CImportLibrary::CImportLibrary(LPCTSTR szName,CYBER_ADDRESS AddressIAT)
{
	//запомнить параметры
	mszName=new TCHAR[_tcslen(szName)+1];
	_tcscpy(mszName,szName);
	mAddressIAT=AddressIAT;
}

CImageListImports::CImportLibrary::~CImportLibrary()
{
	//освободить список функций
	for(std::list<CImageListImports::CImportFunction*>::iterator i=mFunctionsList.begin();i!=mFunctionsList.end();i++)
		(*i)->Release();
	mFunctionsList.clear();
	//освободить память строки-имени
	SafeDeleteMassive(mszName);
}

LPCTSTR CImageListImports::CImportLibrary::GetName()
{
	return mszName;
}

CYBER_ADDRESS CImageListImports::CImportLibrary::GetAddressIAT()
{
	return mAddressIAT;
}

std::list<CImageListImports::CImportFunction*>* CImageListImports::CImportLibrary::GetList()
{
	return &mFunctionsList;
}

// class CImageListImports

CImageListImports::CImageListImports()
{
}

CImageListImports::~CImageListImports()
{
	//освободить список библиотек
	for(std::list<CImportLibrary*>::iterator i=mLibrariesList.begin();i!=mLibrariesList.end();i++)
		(*i)->Release();
	mLibrariesList.clear();
}

std::list<CImageListImports::CImportLibrary*>* CImageListImports::GetList()
{
	return &mLibrariesList;
}
