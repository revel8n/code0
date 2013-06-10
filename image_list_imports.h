#ifndef QS_IMAGE_LIST_IMPORTS_H
#define QS_IMAGE_LIST_IMPORTS_H

#include "explored_image.h"
#include <list>

//класс списка импорта
class CImageListImports : public CObject
{
public:
	//класс импортируемой функции
	class CImportFunction : public CObject
	{
	protected:
		//имя функции
		LPTSTR mszName;
		//адрес элемента в IAT, соответствующего функции
		CYBER_ADDRESS mAddressInIAT;

	public:
		//конструктор и деструктор
		CImportFunction(LPCTSTR szName,CYBER_ADDRESS AddressInIAT);
		virtual ~CImportFunction();

		//получить имя
		LPCTSTR GetName();
		//получить адрес в IAT
		CYBER_ADDRESS GetAddressInIAT();
	};

	//класс импортируемой библиотеки
	class CImportLibrary : public CObject
	{
	protected:
		//список импортируемых функций
		std::list<CImportFunction*> mFunctionsList;
		//имя библиотеки
		LPTSTR mszName;
		//адрес начала IAT
		CYBER_ADDRESS mAddressIAT;

	public:
		//конструктор и деструктор
		CImportLibrary(LPCTSTR szName,CYBER_ADDRESS AddressIAT);
		~CImportLibrary();

		//получить имя
		LPCTSTR GetName();
		//получить адрес IAT
		CYBER_ADDRESS GetAddressIAT();
		//получить список функций
		std::list<CImportFunction*>* GetList();
	};

protected:
	//список импортируемых библиотек
	std::list<CImportLibrary*> mLibrariesList;

public:
	//конструктор и деструктор
	CImageListImports();
	virtual ~CImageListImports();

	//получить список библиотек
	std::list<CImportLibrary*>* GetList();
};

#endif
