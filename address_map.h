#ifndef QS_ADDRESS_MAP_H
#define QS_ADDRESS_MAP_H

/*
Файл содержит определение класса, управляющего хранением информации об исследуемом файле.
*/

#include "object.h"
#include "address_data.h"
#include "label.h"
#include "comment.h"
#include "listing.h"
#include <map>

class CAddressMap : public CObject
{
protected:
	typedef std::map<CYBER_ADDRESS,CAddressData*> AM;
	AM mAddressDataMap;																					//карта (map), содержащая информацию об адресах

public:
	//деструктор
	virtual ~CAddressMap();

	CAddressData* OpenAddress(CYBER_ADDRESS Address);						//открывает или создает информацию о заданном адресе
	CAddressData* GetAddress(CYBER_ADDRESS Address);						//открывает (не создает) информацию о заданном адресе

	//создать перекрестную ссылку (просто, без обработки типов ссылки и дополнительного декодирования)
	VOID AddXReference(CYBER_ADDRESS AddressFrom,CYBER_ADDRESS AddressTo,BYTE ReferenceFlags);
	//указать метку для адреса
	VOID SetLabel(CYBER_ADDRESS Address,LPCTSTR szName);
	//получить метку для адреса (если есть)
	CLabel* GetLabel(CYBER_ADDRESS Address);
	//указать комментарий для адреса
	VOID SetComment(CYBER_ADDRESS Address,LPCTSTR szName);
	//получить комментарий для адреса (если есть)
	CComment* GetComment(CYBER_ADDRESS Address);

	//указать размер адреса
	VOID SetAddressSize(CYBER_ADDRESS Address,DWORD Size);

	// вспомогательные функции

	//сформировать строку для адреса (в виде метки или смещения), память удалять вызовом delete []
	LPTSTR GetAddressString(CYBER_ADDRESS Address);

	//TEST
	VOID ExplortListingToFile();
};

#endif
