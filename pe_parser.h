#ifndef QS_PE_PARSER_H
#define QS_PE_PARSER_H

/*
‘айл содержит определение класса CPEParser, выполн€ющего анализ
структуры исполн€емого файла формата PE Executable (EXE).
*/

#include "file_parser.h"

//флаги настроек анализатора
#define PEPSF_USE_PREFERRED_BASE_ADDRESS						1						//использовать предпочтительный базовый адрес
#define PEPSF_ANALIZE_DOS_STUB											2						//дизассемблировать DOS-заглушку
#define PEPSF_ANALIZE_EXPORTS_CODE									4						//дизассемблировать экспортируемые функции

class CPEParser : public CFileParser
{
public:
	struct SETTINGS
	{
		//предпочтительный адрес загрузки
		CYBER_ADDRESS PreferredBaseAddress;
		//выбранный базовый адрес загрузки
		CYBER_ADDRESS BaseAddress;

		//флаги
		DWORD Flags;
	};

protected:

	//заголовки файла (реальные адреса)
	PIMAGE_DOS_HEADER mpImageDosHeader;
	PIMAGE_FILE_HEADER mpImageFileHeader;
	PIMAGE_OPTIONAL_HEADER mpImageOptionalHeader;
	PIMAGE_SECTION_HEADER mpImageSections;
	//дополнительные настройки
	SETTINGS mSettings;

protected:

	//методы, на которые разбит процесс анализа файла

	//получить заголовки файла и выполнить первичные проверки
	BOOL RetrieveHeaders();
	//спроецировать файл в киберпам€ть
	BOOL MapToCyberMemory();
	//проанализировать общие структуры файла
	BOOL AnalizeGeneral();
	//проанализировать раздел импорта
	BOOL AnalizeImport();
	//проанализировать раздел экспорта
	BOOL AnalizeExport();
	//дизассемблировать код
	BOOL ParseCode();

public:
	//получить, можно ли выбирать дополнительные параметры
	virtual BOOL IsAskSettingsAvaliable();
	//получить дополнительные параметры у пользовател€
	virtual BOOL AskSettings(HWND hWndParent);

	//проверить, может ли данный анализатор обработать данный файл
	virtual BOOL PrepareAnalize();
	//выполнить анализ
	virtual DWORD Analize();

	//получить начальный адрес дл€ просмотра
	virtual CYBER_ADDRESS GetBeginAddress();

	//получить информацию об анализаторе
	virtual VOID About(FILE_PARSER_ABOUT* pAbout);
};

#endif
