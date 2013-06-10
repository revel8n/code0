#ifndef QS_REFERENCE_H
#define QS_REFERENCE_H

/*
Файл содержит определение класса ссылки.
*/

#include <functional>
#include "cyber_memory.h"

class CReference
{
protected:
	//обратный адрес
	CYBER_ADDRESS AddressFrom;
	//прямой адрес
	CYBER_ADDRESS AddressTo;

public:
	//функция сравнения в прямом направлении
	static class CompareForward
	{
	};
	//функция сравнения в обратном направлении
	static class CompareBackward
	{
	};
};

#endif
