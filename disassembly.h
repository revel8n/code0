#ifndef QS_DISASSEMBLY_H
#define QS_DISASSEMBLY_H

#include "windows.h"

/* Файл, описывающий структуры для дизассемблирования.
*/

//максимальное количество операндов в инструкции
#define MAX_INSTRUCTION_OPERANDS_COUNT		3
//максимальная длина имени инструкции
#define MAX_INSTRUCTION_NAME_LENGTH				16

//специальный идентификатор ошибочной инструкции
#define INSTRUCTION_ID_ERROR							0xFFFFFFFF

/******************* СТРУКТУРЫ ИНФОРМАЦИИ *******************/

/*** флаги информации об операнде инструкции ***/

//флаги источника операнда
#define OIS_DATA												0					//константа
#define OIS_SIGNED_DATA									1					//константа со знаком (знак имеет значение при размере меньше 32-битного)
#define OIS_REGISTER_BYTE								2					//регистр из регистрового байта
#define OIS_MODRM_RO										3					//регистр из поля RO байта ModR/M
#define OIS_MODRM_RM										4					//регистр или переменная из поля RM байта ModR/M
#define OIS_MEMORY_IM										5					//переменная по константному 32-битному адресу
#define OIS_CONST_REGISTER							6					//постоянный регистр
#define OIS_CONST_DATA									7					//постоянная константа

//флаги типа ссылки
/* Следующая группа флагов - взаимоискючающие флаги, которые не учитываются для ссылки адреса */
#define OIR_ABSOLUTE_POINTER						1					//флаг, указывающий на то, что данные следует трактовать как абсолютный указатель в памяти
#define OIR_RELATIVE_POINTER						2					//флаг, указывающий на то, что данные следует трактовать как относительный указатель в памяти
/* Следующая группа флагов - также взаимоискючающие флаги, которые учитываются, только если установлен одни из флагов первой группы */
#define OIR_REFERENCE_WRITE							4					//флаг записи памяти
#define OIR_REFERENCE_READ							8					//флаг чтения памяти
#define OIR_REFERENCE_READWRITE					(OIR_REFERENCE_READ | OIR_REFERENCE_WRITE)	//флаг чтения/записи памяти
#define OIR_REFERENCE_EXECUTE						16				//флаг выполнения
/* Следующая группа флагов - также взаимоискючающие флаги, которые учитываются, только если установлен флаг OIR_REFERENCE_EXECUTE */
#define OIR_REFERENCE_EXECUTE_CALL			32				//флаг того, что к коду обращаются как к функции

/*
Постоянный регистр: категория регистра берется из bRegisterCategory, а номер - из bConstRegister.
Это позволяет использовать категорию RC_WORDDWORD.
*/

struct OPERAND_INFORMATION												//информация об операнде инструкции
{
	BYTE bSource;																		//источник операнда - откуда считывать операнд
	/* Данное поле должно использоваться только для разбора инструкции. */

	BYTE bSize;																			//размер данных операнда (для переменных в памяти и непосредственных значений)
	BYTE bAddressReference;													//тип ссылки адреса операнда (т.е. та ячейка памяти, на которую указывает адрес)
	BYTE bDataReference;														//тип ссылки данных операнда (т.е. содержимое ячейки памяти)
	BYTE bRegisterCategory;													//категория используемых регистров (если операнд - регистр)
	union
	{
		BYTE bConstRegister;													//постоянный регистр (только если bSource=OIS_CONST_REGISTER)
		BYTE bConstData;															//постоянная константа (только если bSource=OIS_CONST_DATA)
	};
};

/*** флаги информации об инструкции ***/
#define IIF_MODRM									1								//требуется байт Mod/RM
#define IIF_REGBYTE								2								//требуется регистровый байт
#define IIF_ENDPROC								4								//инструкция является окончанием процедуры
#define IIF_SPACE									8								//инструкция является "знаковым событием", и должен быть добавлен разделитель

struct INSTRUCTION_INFORMATION										//информация об инструкции
{
	BYTE bFlags;																		//флаги
	BYTE bCodeSize;																	//размер кода
	BYTE bRegisterByteOffset;												//смещение от начала инструкции регистрового байта
	BYTE bOperandsCount;														//количество операндов
	DWORD dwSense;																	//"смысл" инструкции, её значение при смысловом разборе кода
	OPERAND_INFORMATION Operands[MAX_INSTRUCTION_OPERANDS_COUNT];	//информация об операндах
	TCHAR szName[MAX_INSTRUCTION_NAME_LENGTH];				//имя инструкции
};

/*** флаги байта инструкции ***/
#define IBF_OK										1								//байт есть; при отсутствии флага инструкция считается нераспознанной
#define IBF_END										2								//этот байт - конечный
#define IBF_PREFIX								4								//этот байт - префикс

struct INSTRUCTION_BYTE														//байт инструкции
{
	BYTE bFlags;																		//флаги
	BYTE bMask;																			//маска кода для следующего байта
	union
	{
		DWORD dwInstructionID;												//если флаг IBF_END установлен, то - идентификатор инструкции
		DWORD dwNextByteGroup;												//если флаг IBF_END не установлен, то - первый из возможных следующих байтов
		DWORD dwPrefixFlag;														//если флаг IBF_PREFIX установлен, то - флаг префикса
	};
};

/******************* СТРУКТУРЫ ЭКЗЕМПЛЯРОВ *******************/

//флаги поля RM из ModRM
//первые 4 совпадают с флагами OMF_ из code.h!
#define MODRM_RM_BASE_REGISTER			1							//используется базовый регистр
#define MODRM_RM_INDEX_REGISTER			2							//используется индексный регистр
#define MODRM_RM_MULTIPLIER					4							//используется множитель
#define MODRM_RM_OFFSET							8							//используется смещение
#define MODRM_RM_MEMORY							16						//флаг того, что операнд RM описывает переменную в памяти

struct MODRM																			//информация о дизассемблированном байте ModRM
{
	struct
	{
		BYTE bRORegister;															//регистр из поля RO
	} RO;																						//информация об операнде RO
	struct
	{
		BYTE bFlags;																	//флаги поля RM
		union
		{
			BYTE bRegister;															//регистр (при использовании регистровой адресации)
			BYTE bBaseRegister;													//базовый регистр
		};
		BYTE bIndexRegister;													//индексный регистр
		BYTE bMultiplier;															//множитель (1, 2, 4, 8)
		DWORD dwOffset;																//смещение
	} RM;																						//информация об операнде RM
};

#endif
