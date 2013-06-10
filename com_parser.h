#ifndef QS_COM_PARSER_H
#define QS_COM_PARSER_H

/*
Файл содержит определение класса разбора исполняемого файла COM.
*/

#include "file_parser.h"

//флаги настроек анализатора
#define COMPSF_32BIT														1					//файл является 32-битным
#define COMPSF_INTERRUPTTABLE										2					//в начале файла расположена таблица прерываний

class CCOMParser : public CFileParser
{
public:
	//структура настроек COM-анализатора
	struct SETTINGS
	{
		//адрес загрузки
		CYBER_ADDRESS BaseAddress;
		//адрес точки входа
		CYBER_ADDRESS EntryPointAddress;
		//флаги
		DWORD Flags;
	};

protected:
	//настройки анализатора
	SETTINGS mSettings;

public:
	//получить, можно ли выбирать дополнительные параметры
	virtual BOOL IsAskSettingsAvaliable();
	//получить дополнительные параметры у пользователя
	virtual BOOL AskSettings(HWND hWndParent);

	//проверить, может ли данный анализатор обработать данный файл
	virtual BOOL PrepareAnalize();
	//выполнить анализ
	virtual DWORD Analize();

	//получить начальный адрес для просмотра
	virtual CYBER_ADDRESS GetBeginAddress();

	//получить информацию об анализаторе
	virtual VOID About(FILE_PARSER_ABOUT* pAbout);
};

#endif
