#include "image_list_exports.h"

// class CImageListExports

// subclass CExportFunction

CImageListExports::CExportFunction::CExportFunction(LPCTSTR szName,UINT Ordinal,CYBER_ADDRESS Address)
{
	if(szName)
	{
		mszName=new TCHAR[_tcslen(szName)+1];
		_tcscpy(mszName,szName);
	}
	else mszName=NULL;
	mOrdinal=Ordinal;
	mAddress=Address;
	mIsLocal=TRUE;
}

CImageListExports::CExportFunction::CExportFunction(LPCTSTR szName,UINT Ordinal,LPCTSTR szForwardName)
{
	if(szName)
	{
		mszName=new TCHAR[_tcslen(szName)+1];
		_tcscpy(mszName,szName);
	}
	else mszName=NULL;
	mOrdinal=Ordinal;
	mszForwardName=new TCHAR[_tcslen(szForwardName)+1];
	_tcscpy(mszForwardName,szForwardName);
	mIsLocal=FALSE;
}

CImageListExports::CExportFunction::~CExportFunction()
{
	SafeDeleteMassive(mszName);
	if(!mIsLocal)
		SafeDeleteMassive(mszForwardName);
}

LPCTSTR CImageListExports::CExportFunction::GetName()
{
	return mszName;
}

UINT CImageListExports::CExportFunction::GetOrdinal()
{
	return mOrdinal;
}

BOOL CImageListExports::CExportFunction::IsLocal()
{
	return mIsLocal;
}

CYBER_ADDRESS CImageListExports::CExportFunction::GetAddress()
{
	return mAddress;
}

LPCTSTR CImageListExports::CExportFunction::GetForwardName()
{
	return mszForwardName;
}

// class CImageListExports

CImageListExports::CImageListExports()
{
}

CImageListExports::~CImageListExports()
{
	//освободить список функций
	for(std::list<CExportFunction*>::iterator i=mFunctionsList.begin();i!=mFunctionsList.end();i++)
		(*i)->Release();
	mFunctionsList.clear();
}

std::list<CImageListExports::CExportFunction*>* CImageListExports::GetList()
{
	return &mFunctionsList;
}
