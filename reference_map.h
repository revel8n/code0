#ifndef QS_REFERENCE_MAP_H
#define QS_REFERENCE_MAP_H

/*
Файл содержит определение класса карты ссылок.
*/

#include "reference.h"
#include <set>

class CReferenceMap : public CObject
{
public:
	//типы итераторов для элементов карты
	typedef std::set<CReference*,CReference::CompareForward>::iterator REFERENCE_FORWARD_ITERATOR;
	typedef std::set<CReference*,CReference::CompareBackward>::iterator REFERENCE_BACKWARD_ITERATOR;

protected:
	//прямая и обратная наборы ссылок
	std::set<CReference*,CReference::CompareForward> mSetForward;
	std::set<CReference*,CReference::CompareBackward> mSetBackward;

public:
	//получить список ссылок, исходящих из адреса
	std::pair<REFERENCE_FORWARD_ITERATOR,CReferenceMap::REFERENCE_FORWARD_ITERATOR> QueryFromAddress(CYBER_ADDRESS Address);
	//получить список ссылок, входящих в адрес
	std::pair<REFERENCE_BACKWARD_ITERATOR,CReferenceMap::REFERENCE_BACKWARD_ITERATOR> QueryToAddress(CYBER_ADDRESS Address);
};

#endif
