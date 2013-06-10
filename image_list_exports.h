#ifndef QS_IMAGE_LIST_EXPORTS_H
#define QS_IMAGE_LIST_EXPORTS_H

#include "explored_image.h"
#include <list>

//класс списка экспорта
class CImageListExports : public CObject
{
public:
	//класс экспортируемой функции
	class CExportFunction : public CObject
	{
	protected:
		//имя функции
		LPTSTR mszName;
		//ординал функции
		UINT mOrdinal;
		//флаг локальности функции (т.е. не форвард)
		BOOL mIsLocal;
		union
		{
			//адрес кода функции
			CYBER_ADDRESS mAddress;
			//имя функции-форварда
			LPTSTR mszForwardName;
		};

	public:
		//конструкторы и деструктор
		CExportFunction(LPCTSTR szName,UINT Ordinal,CYBER_ADDRESS Address);
		CExportFunction(LPCTSTR szName,UINT Ordinal,LPCTSTR szForwardName);
		virtual ~CExportFunction();

		//получить имя
		LPCTSTR GetName();
		//получить ординал
		UINT GetOrdinal();
		//получить флаг локальности
		BOOL IsLocal();
		//получить адрес Функции
		CYBER_ADDRESS GetAddress();
		//получить имя функции-форварда
		LPCTSTR GetForwardName();
	};

protected:
	//список экспортируемых функций
	std::list<CExportFunction*> mFunctionsList;

public:
	//конструктор и деструктор
	CImageListExports();
	virtual ~CImageListExports();

	//получить список функций
	std::list<CExportFunction*>* GetList();
};

#endif
