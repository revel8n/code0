#include "object.h"

// class CObject

CObject::CObject()
{
	//установить начальное количество ссылок
	mObjectReferencesCount=1;
}

CObject::~CObject()
{
	//ничего не делать, это определено
	//для поддержки виртуальных деструкторов
}

VOID CObject::AddRef()
{
	//увеличить количество ссылок
	mObjectReferencesCount++;
}

VOID CObject::Release()
{
	_ASSERTE(mObjectReferencesCount!=0);
	//уменьшить количество ссылок
	mObjectReferencesCount--;
	//если количество ссылок равно 0
	if(!mObjectReferencesCount)
		//удалить объект
		delete this;
}
