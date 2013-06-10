#include "address_map.h"
#include "xreference.h"
#include "label.h"

// class CAddressMap realization

CAddressMap::~CAddressMap()
{
	//удалить данные всех адресов
	for(AM::iterator i=mAddressDataMap.begin();i!=mAddressDataMap.end();i++)
		(*i).second->Release();
	//очистить карту адресов
	mAddressDataMap.clear();
}

CAddressData* CAddressMap::OpenAddress(CYBER_ADDRESS Address)
{
	//попробовать найти связанные данные адреса
	AM::iterator i=mAddressDataMap.find(Address);
	//если найдено, вернуть указатель
	if(i!=mAddressDataMap.end())
	{
		CAddressData* pAddressData=(*i).second;
		pAddressData->AddRef();
		return pAddressData;
	}
	//если не найдено, создать новый экземпляр связанных данных
	CAddressData* pAddressData=new CAddressData;
	//указать адрес блока по умолчанию
	pAddressData->SetBlockAddress(Address);
	//добавить его в карту
	mAddressDataMap.insert(std::pair<CYBER_ADDRESS,CAddressData*>(Address,pAddressData));
	//вернуть указатель
	pAddressData->AddRef();
	return pAddressData;
}

CAddressData* CAddressMap::GetAddress(CYBER_ADDRESS Address)
{
	//найти связанные данные адреса
	AM::iterator i=mAddressDataMap.find(Address);
	//если найдено, вернуть указатель
	if(i!=mAddressDataMap.end())
	{
		CAddressData* pAddressData=(*i).second;
		pAddressData->AddRef();
		return pAddressData;
	}
	//иначе не найдено
	return NULL;
}

VOID CAddressMap::AddXReference(CYBER_ADDRESS AddressFrom,CYBER_ADDRESS AddressTo,BYTE ReferenceFlags)
{
	//добавить перекрестную ссылку
	CAddressData* pAddressData=OpenAddress(AddressTo);
	CXReference* pXReference=new CXReference(AddressFrom,ReferenceFlags);
	pAddressData->AddElement(pXReference);
	pXReference->Release();
	pAddressData->Release();
}

VOID CAddressMap::SetLabel(CYBER_ADDRESS Address,LPCTSTR szName)
{
	//указать метку, если её ещё нет
	CAddressData* pAddressData=OpenAddress(Address);
	CLabel* pLabel=pAddressData->GetLabel();
	if(pLabel)
		//освободить существующую метку
		pLabel->Release();
	else
	{
		//создать новую метку
		pLabel=new CLabel(szName);
		//указать её карте адресов
		pAddressData->SetLabel(pLabel);
		//освободить метку
		pLabel->Release();
	}
	//освободить данные адреса
	pAddressData->Release();
}

CLabel* CAddressMap::GetLabel(CYBER_ADDRESS Address)
{
	//получить связанные данные
	CAddressData* pData=GetAddress(Address);
	if(pData)
	{
		//получить и вернуть метку
		CLabel* pLabel=pData->GetLabel();
		pData->Release();
		return pLabel;
	}
	return NULL;
}

VOID CAddressMap::SetComment(CYBER_ADDRESS Address,LPCTSTR szName)
{
	//указать комментарий, если его ещё нет
	CAddressData* pAddressData=OpenAddress(Address);
	CComment* pComment=pAddressData->GetComment();
	if(pComment)
		//освободить существующий комментарий
		pComment->Release();
	else
	{
		//создать новый комментарий
		pComment=new CComment(szName);
		//указать её карте адресов
		pAddressData->SetComment(pComment);
		//освободить комментарий
		pComment->Release();
	}
	//освободить данные адреса
	pAddressData->Release();
}

CComment* CAddressMap::GetComment(CYBER_ADDRESS Address)
{
	//получить связанные данные
	CAddressData* pData=GetAddress(Address);
	if(pData)
	{
		//получить и вернуть комментарий
		CComment* pComment=pData->GetComment();
		pData->Release();
		return pComment;
	}
	return NULL;
}

VOID CAddressMap::SetAddressSize(CYBER_ADDRESS Address,DWORD Size)
{
	//указать размер для адреса
	CAddressData* pAddressData=OpenAddress(Address);
	pAddressData->SetSize(Size);
	pAddressData->Release();
	//указать размер предыдущего адреса для следующего адреса
	pAddressData=OpenAddress(Address+Size);
	pAddressData->SetPreviousSize(Size);
	pAddressData->Release();
}

LPTSTR CAddressMap::GetAddressString(CYBER_ADDRESS Address)
{
	//получить данные адреса
	CAddressData* pAddressData=GetAddress(Address);
	if(pAddressData)
	{
		//выяснить, есть ли у адреса метка
		CLabel* pLabel=pAddressData->GetLabel();
		if(pLabel)
		{
			pAddressData->Release();
			//вернуть строку с меткой
			LPCTSTR szLabel=pLabel->GetName();
			LPTSTR szAddress=new TCHAR[_tcslen(szLabel)+1];
			_tcscpy(szAddress,szLabel);
			pLabel->Release();
			return szAddress;
		}
		//выяснить, есть ли у адреса блока метка
		CYBER_ADDRESS BlockAddress=pAddressData->GetBlockAddress();
		pLabel=GetLabel(BlockAddress);
		if(pLabel)
		{
			pAddressData->Release();
			//вывести в форме смещения от начала блока
			LPCTSTR szLabel=pLabel->GetName();
			LPTSTR szAddress=new TCHAR[_tcslen(szLabel)+11];
			int offset=Address-BlockAddress;
			_stprintf(szAddress,offset >0 ? TEXT("%s+%Xh") : TEXT("%s-%Xh"),szLabel,offset>0 ? offset : -offset);
			pLabel->Release();
			return szAddress;
		}
		pAddressData->Release();
	}
	//вернуть строку с адресом-числом
	LPTSTR szAddress=new TCHAR[10];
	_stprintf(szAddress,TEXT("0%X"),Address);
	return szAddress;
}

#include "listing_textfile.h"
#include "listing_device.h"
#include "application.h"
#include <math.h>

//TEST
VOID CAddressMap::ExplortListingToFile()
{
/*	//создать метафайл
	HDC hdc=CreateEnhMetaFile(NULL,TEXT("listing.emf"),NULL,NULL);

	//создать листинг
	CListingDevice Listing;
	LISTING_DEVICE_CONTEXT Context;
	Context.hdc=hdc;
//	SetRect(&Context.Rect,0,0,1024,0);
	Listing.SetAddressMap(this);
//	Listing.SetContext(Context);

	//указать шрифт
	SelectFont(hdc,gApplication.mhFontCode);

	//вывести данные всех адресов
	// 1024*xcount=ycount*h
	// xcount*ycount=size

	// xcount=size/ycount;
	// 1024*xcount=size/xcount*h
	// ycount=sqrt(size*1024/h);
	UINT CountY=(UINT)sqrt(mAddressDataMap.size()*1024/gApplication.mFontCodeHeight);
	UINT j=0;
	for(AM::iterator i=mAddressDataMap.begin();i!=mAddressDataMap.end();i++,j++)
	{
		//вывести данные адреса
		if(j%CountY==0)
		{
			Context.Rect.left=(j/CountY)*1024;
			Context.Rect.top=0;
			Context.Rect.right=Context.Rect.left+1024;
			Context.Rect.bottom=gApplication.mFontCodeHeight;
			Listing.SetContext(Context);
		}
		Listing.SetAddress((*i).first);
		(*i).second->Print(&Listing);
	}

	//закрыть метафайл
	DeleteEnhMetaFile(CloseEnhMetaFile(hdc));*/

	//открыть файл
	HANDLE hFile=CreateFile(TEXT("listing.txt"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
	if(hFile==INVALID_HANDLE_VALUE) return;

	//создать листинг
	CListingTextFile Listing;
	Listing.SetFile(hFile);
	Listing.SetAddressMap(this);

	//вывести данные всех адресов
	for(AM::iterator i=mAddressDataMap.begin();i!=mAddressDataMap.end();i++)
	{
		//вывести данные адреса
		Listing.SetAddress((*i).first);
		(*i).second->Print(&Listing);
	}

	//закрыть файл
	CloseHandle(hFile);

	//сообщение
	MessageBox(NULL,TEXT(""),TEXT(""),0);
}
