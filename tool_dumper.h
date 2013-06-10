#ifndef QS_TOOL_DUMPER_H
#define QS_TOOL_DUMPER_H

#ifdef CODE0_COMPILE_DUMPING_TOOL

/*
Файл содержит определение инструмента - дампера памяти процессов.
*/

#include "dialog.h"
#include "usercontrol_list.h"

class CToolDumper														//класс - инструмент снятия дампов памяти
{
protected:
	//класс диалога выбора процесса
	class CSelectProcessDialog;
protected:
	//идентификатор обрабатываемого процесса
	DWORD mProcessID;

public:
	//вызвать инструмент
	VOID Start();
};

#endif

#endif
