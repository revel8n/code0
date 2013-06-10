#ifndef QS_BINARY_PARSER_H
#define QS_BINARY_PARSER_H

/*
Файл содержит определение анализатора двоичных файлов.
*/

#include "file_parser.h"

class CBinaryParser : public CFileParser
{
public:
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
