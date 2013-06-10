#include "address_data.h"
#include "subroutine.h"

// class CAddressData realization

CAddressData::CAddressData()
{
	//инициализировать размер адреса (по умолчанию 1)
	mSize=1;
	mPreviousSize=1;
}

CAddressData::~CAddressData()
{
	//очистить список
	for(ADELI i=mElementsMap.begin();i!=mElementsMap.end();i++)
		(*i).second->Release();
	mElementsMap.clear();
}

VOID CAddressData::SetIdentityElement(CAddressDataElement* pElement,DWORD ElementType)
{
	//удалить элемент, если такой уже есть
	ADELI i=mElementsMap.find(ElementType);
	if(i!=mElementsMap.end())
	{
		(*i).second->Release();
		mElementsMap.erase(i);
	}

	//если новый элемент указан, добавить его в карту данных
	if(pElement)
		AddElement(pElement);
}

BOOL CAddressData::IsEmpty()
{
	//считать данные пустыми, если в список элементов пуст
	return !mElementsMap.size();
}

VOID CAddressData::SetSize(DWORD Size)
{
	//запомнить размер адреса
	mSize=Size;
}

DWORD CAddressData::GetSize()
{
	//вернуть размер адреса
	return mSize;
}

VOID CAddressData::SetPreviousSize(DWORD PreviousSize)
{
	//запомнить размер предыдущего адреса
	mPreviousSize=PreviousSize;
}

DWORD CAddressData::GetPreviousSize()
{
	//вернуть размер предыдущего адреса
	return mPreviousSize;
}

VOID CAddressData::SetBlockAddress(CYBER_ADDRESS Address)
{
	//запомнить адрес блока
	mBlockAddress=Address;
}

CYBER_ADDRESS CAddressData::GetBlockAddress()
{
	//вернуть адрес блока
	return mBlockAddress;
}

VOID CAddressData::AddElement(CAddressDataElement* pElement)
{
	//добавить элемент в карту
	pElement->AddRef();
	mElementsMap.insert(std::pair<DWORD,CAddressDataElement*>(pElement->GetElementType(),pElement));
}

CAddressData::ADEL* CAddressData::GetElements()
{
	//вернуть указатель на карту
	return &mElementsMap;
}

VOID CAddressData::Print(CListing* pListing)
{
#ifdef NIKITIN
	if(0)
#endif
	//если в карте данных есть метка
	if(mElementsMap.find(ADET_LABEL)!=mElementsMap.end())
	{
		//вывести пустую строку
		pListing->BeginLine();
		pListing->EndLine();
	}

	//цикл по элементам в карте данных
	BOOL TooManyElements=mElementsMap.size()>10;
	DWORD LastElementType=0xFFFFFFFF;
	BOOL LastElementMessage=FALSE;
	for(ADELI i=mElementsMap.begin();i!=mElementsMap.end();i++)
	{
		//получить элемент
		CAddressDataElement* pElement=(*i).second;
		if(TooManyElements)
		{
			DWORD ElementType=(*i).first;
			if(ElementType==LastElementType)
			{
				if(LastElementMessage)
				{
					pListing->BeginLine();
					pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_COMMENTS),TEXT("; ... ... ..."));
					pListing->EndLine();
					LastElementMessage=FALSE;
				}
				continue;
			}
			LastElementType=ElementType;
			LastElementMessage=TRUE;
		}
		//напечатать элемент
		pElement->Print(pListing);
	}
	//если интерпретации нет
	if(mElementsMap.find(ADET_INTERPRETATION)==mElementsMap.end())
	{
		//напечатать, что интерпретации нет
		pListing->BeginLine();
		pListing->Print(pListing->GetColumnMargin(LISTING_COLUMN_NAME),TEXT("(no interpretation)"));
		pListing->EndLine();
	}
}

CInterpretation* CAddressData::GetInterpretation()
{
	//вернуть интерпретацию, если она есть
	ADELI i=mElementsMap.find(ADET_INTERPRETATION);
	if(i!=mElementsMap.end())
	{
		CInterpretation* pInterpretation=(CInterpretation*)(*i).second;
		pInterpretation->AddRef();
		return pInterpretation;
	}
	return NULL;
}

VOID CAddressData::SetInterpretation(CInterpretation* pInterpretation)
{
	SetIdentityElement(pInterpretation,ADET_INTERPRETATION);
}

CLabel* CAddressData::GetLabel()
{
	//вернуть метку, если она есть
	ADELI i=mElementsMap.find(ADET_LABEL);
	if(i!=mElementsMap.end())
	{
		CLabel* pLabel=(CLabel*)(*i).second;
		pLabel->AddRef();
		return pLabel;
	}
	return NULL;
}

VOID CAddressData::SetLabel(CLabel* pLabel)
{
	SetIdentityElement(pLabel,ADET_LABEL);
}

CComment* CAddressData::GetComment()
{
	//вернуть комментарий, если он есть
	ADELI i=mElementsMap.find(ADET_COMMENT);
	if(i!=mElementsMap.end())
	{
		CComment* pComment=(CComment*)(*i).second;
		pComment->AddRef();
		return pComment;
	}
	return NULL;
}

VOID CAddressData::SetComment(CComment* pComment)
{
	SetIdentityElement(pComment,ADET_COMMENT);
}

VOID CAddressData::CreateSubroutine()
{
	//если функции нет, создать её
	if(mElementsMap.find(ADET_SUBROUTINE)==mElementsMap.end())
	{
		CSubroutine* pSubroutine=new CSubroutine();
		AddElement(pSubroutine);
		pSubroutine->Release();
	}
}
