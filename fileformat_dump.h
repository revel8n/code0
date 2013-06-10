#ifndef QS_FILEFORMAT_DUMP_H
#define QS_FILEFORMAT_DUMP_H

/*
Файл содержит определения структур, используемых в
файлах дампа формата Code0.
*/

/*
Формат файла дампа:

FILE_DUMP_HEADER Header;																		//заголовок файла дампа
FILE_DUMP_REGION Regions[Header.RegionsCount];							//список регионов
FILE_DUMP_THREAD Threads[Header.ThreadsCount];							//список потоков
FILE_DUMP_MODULE Modules[Header.ModulesCount];							//список модулей
BYTE Data[?];																								//различные данные; определяется ссылками в структурах из предыдущих частей файла
*/

//**** заголовок файла дампа
//сигнатура файла дампа
#define FILE_DUMP_MAGIC0									'edoC'
#define FILE_DUMP_MAGIC1									'pmd0'
//структура заголовка
struct FILE_DUMP_HEADER
{
	DWORD Magics[2];												//сигнатура файла дампа

	DWORD FileSize;													//размер файла
	DWORD RegionsCount;											//количество регионов
	DWORD ThreadsCount;											//количество потоков
	DWORD ModulesCount;											//количество модулей
};

//**** заголовок региона
struct FILE_DUMP_REGION
{
	DWORD Flags;														//флаги
	DWORD BaseAddress;											//адрес начала региона
	DWORD Size;															//размер региона
	DWORD State;														//состояние региона
	DWORD Protect;													//атрибуты защиты памяти
	DWORD DataOffset;												//смещение от начала файла до данных региона
};

//**** заголовок потока
struct FILE_DUMP_THREAD
{
	DWORD Flags;														//флаги
	DWORD ThreadID;													//идентификатор потока
	DWORD UsageCount;												//количество ссылок на поток
	LONG PriorityBase;											//базовый уровень приоритета
	LONG PriorityDelta;											//изменение уровня приоритета
};

//**** заголовок модуля
struct FILE_DUMP_MODULE
{
	DWORD Flags;														//флаги
	DWORD BaseAddress;											//базовый адрес модуля
	DWORD BaseSize;													//размер модуля
	DWORD UsageGlobalCount;									//количество ссылок на модуль в системе
	DWORD UsageProcessCount;								//количество ссылок на модуль в процессе
	TCHAR ModuleName[256];									//имя модуля (256 = MAX_MODULE_NAME32+1)
	TCHAR ModulePath[MAX_PATH];							//смещение от начала файла до полного имени файла модуля
};

#endif
