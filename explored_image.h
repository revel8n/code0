#ifndef QS_EXPLORED_IMAGE_H
#define QS_EXPLORED_IMAGE_H

#include "object.h"
#include "cyber_memory.h"
#include "address_map.h"
#include "code_parser.h"
#include "file_parser.h"
#include "dangle_group.h"

/*
Файл содержит объявление класса CExploredImage, который олицетворяет один загруженный для анализа файл.
Этот класс является основным, и объединяет все объекты, используемые для анализа.
Так сказать, рабочее пространство анализатора. Но CExploredImage является только
"внутренним", "служебным" рабочим пространством, и не содержит упоминаний о пользовательском
интерфейсе или инструментах.

LoadFile выполняет только физическое открытие и проецирование файла, анализа не выполняется.
*/

class CExploredImage : public CObject
{
protected:
	//имя загруженного файла
	TCHAR mszFileName[MAX_PATH];
	//указатель на память загруженного образа
	PVOID mpFileBuffer;
	//указатель на проекцию файла
	PVOID mpFileMapping;
	//размер проекции
	DWORD mFileMappingSize;

	//адресное киберпространство загруженного файла
	CCyberMemory* mpMemory;
	//карта адресов файла
	CAddressMap* mpAddressMap;
	//анализатор кода
	CCodeParser* mpCodeParser;

	//карта дополнительных информационных объектов
	std::map<UINT,CObject*> mExtendMap;

protected:
	//загрузить файл
	BOOL LoadFile(LPCTSTR szFileName);

public:
	CExploredImage(LPCTSTR szFileName);
	virtual ~CExploredImage();

	//получить, открыт файл или нет
	BOOL IsFileOpened();
	//получить имя файла (без пути)
	LPCTSTR GetFileName();

	//выполнить анализ файла и представить его на экран (возвращается код результата файлового анализатора)
	DWORD AnalizeAndPresent(CFileParser* pFileParser,CDangleGroup* pDangleGroup);

	//получить карту адресов
	CAddressMap* GetAddressMap();
	//получить информацию об адресе
	CAddressData* GetAddressData(CYBER_ADDRESS Address);
	//получить указатель на киберпамять
	CCyberMemory* GetMemory();
	//получить указатель на анализатор кода
	CCodeParser* GetCodeParser();
	//получить указатель на карту дополнительных информационных объектов
	std::map<UINT,CObject*>* GetExtendMap();

	//выполнить запрос к пользователю и получить анализатор файла
	CFileParser* AskFileParser(HWND hWndParent);

	//работа с интерпретациями
	VOID SetAddressInterpretationData(CYBER_ADDRESS Address,DWORD Size);
	VOID SetAddressInterpretationCode(CYBER_ADDRESS Address);
	VOID SetAddressInterpretationStringUnicode(CYBER_ADDRESS Address);
	VOID SetAddressInterpretationStringASCII(CYBER_ADDRESS Address);
	VOID DeleteAddressIntepretation(CYBER_ADDRESS Address);
};

#endif
