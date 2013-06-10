#ifndef QS_DUMPFILE_PARSER_H
#define QS_DUMPFILE_PARSER_H

/*
‘айл содержит определение класса CDumpFileParser, выполн€ющего
загрузку и анализ файла дампа процесса в формате Code0.
*/

#include "file_parser.h"
#include "fileformat_dump.h"

class CDumpFileParser : public CFileParser
{
protected:
	//указатели на массивы заголовков
	FILE_DUMP_HEADER* mpHeader;
	FILE_DUMP_REGION* mpRegions;
	FILE_DUMP_THREAD* mpThreads;
	FILE_DUMP_MODULE* mpModules;
	//размеры массивов заголовков
	UINT mRegionsCount;
	UINT mThreadsCount;
	UINT mModulesCount;

public:
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
