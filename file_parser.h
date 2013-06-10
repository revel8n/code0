#ifndef QS_FILE_PARSER_H
#define QS_FILE_PARSER_H

/*
Файл содержит определение абстрактного класса анализатора структуры файла.

Последовательность действий при анализе:
1. Вызовом SetFile указывается файл для обработки. При этом класс CFileParser ничего ещё не выполняет.
			Анализатор заполняет переменные mpFileMapping и mFileSize.
2. Вызовом PrepareAnalize проверяется, может ли анализатор обработать этот файл.
			Анализатор проводит первичный анализ формата файла, заполняет переменную mFileBufferSize
			размером необходимой памяти.
3. С вызовом AskSettings анализатор может получить дополнительные параметры анализа.
			Анализатор может переопределить переменную mFileBufferSize.
			Также надо учитывать, что AskSettings - необязательный вызов, и должны быть предусмотрены
			настройки по умолчанию.
4. Вызовом GetNeededMemorySize получается размер памяти, необходимой анализатору для анализа файла.
5. Выделить память для анализатора.
6. Вызовом SetMemoryBuffer указать выделенную память анализатору.
7. Вызовом SetEnvironment необходимо указать необходимые для анализа объекты.
			Анализатор заполняет переменные mpMemory, mpAddressMap и mpExtendMap.
8. Вызовом Analize - проанализировать файл.
			Анализатор создаёт анализатор кода в переменной CCodeParser.
9. Вызовом Present - открыть различные окна просмотра для исследования проанализированного файла.
10. Теперь можно удалить объект анализатора.
*/

#include "object.h"
#include "cyber_memory.h"
#include "address_map.h"
#include "code_parser.h"

//** коды результата анализа
#define FILE_PARSER_ERROR_SUCCESS					0					//автоматический анализ завершен успешно
#define FILE_PARSER_ERROR_LOADING					1					//ошибка, файл даже не загружен
#define FILE_PARSER_ERROR_ANALIZING				2					//ошибка в анализе, но файл загружен и может анализироваться вручную

//структура ознакомительной информации об анализаторе
struct FILE_PARSER_ABOUT
{
	TCHAR szName[0x40];												//имя анализатора
	TCHAR szDescription[0x100];								//описание анализатора
	DWORD Version;														//версия анализатора
};

class CFileParser : public CObject
{
protected:
	//параметры анализатора
	PVOID mpFileMapping;														//проекция файла на адресное пространство
	DWORD mFileSize;																//размер файла
	CCyberMemory* mpMemory;													//киберпамять
	CAddressMap* mpAddressMap;											//карта адресов
	CCodeParser* mpCodeParser;											//анализатор кода
	std::map<UINT,CObject*>* mpExtendMap;						//карта дополнительных информационных объектов
	PVOID mpFileBuffer;															//буфер файла
	DWORD mFileBufferSize;													//размер буфера файла

public:
	//конструктор и деструктор
	CFileParser();
	virtual ~CFileParser();

	//указать файл для обработки
	VOID SetFile(PVOID pFileMapping,DWORD Size);

	//проверить, может ли данный анализатор обработать данный файл
	//проверка не обязательно абсолютно точная; просто надо отсечь заведомо неподходящие файлы,
	//чтобы пользователю было проще выбирать
	virtual BOOL PrepareAnalize() PURE;

	//получить количество необходимой памяти
	DWORD GetNeededMemorySize();

	//указать память, выделенную для анализа
	VOID SetFileBuffer(PVOID pBuffer);

	//получить, можно ли выбирать дополнительные параметры (определена заглушка, возвращающая FALSE)
	virtual BOOL IsAskSettingsAvaliable();
	//получить дополнительные параметры у пользователя (определена заглушка)
	virtual BOOL AskSettings(HWND hWndParent);

	//указать окружение анализатора
	VOID SetEnvironment(CCyberMemory* pMemory,CAddressMap* pAddressMap,std::map<UINT,CObject*>* pExtendMap);
	//выполнить анализ (возвращается код ошибки FILE_PARSER_ERROR_)
	virtual DWORD Analize() PURE;
	//получить анализатор кода
	CCodeParser* GetCodeParser();

	//получить начальный адрес для просмотра
	virtual CYBER_ADDRESS GetBeginAddress() PURE;

	//получить информацию об анализаторе (просто ознакомительные сведения для пользователя)
	virtual VOID About(FILE_PARSER_ABOUT* pAbout) PURE;
};

#endif
