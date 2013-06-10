#ifndef QS_POINTER_OBJECT
#define QS_POINTER_OBJECT

/*
Файл содержит определение вспомогательного класса-указателя, использующегося для хранения указателей на различные объекты.
*/

#include "object.h"

class CPointerObject
{
protected:
	CObject* mpObject;

public:
	CPointerObject(CObject* pObject);
	~CPointerObject();
	T& operator*()
	{
		return *mpObject;
	}
};

#endif
