#ifndef QS_APPLICATION_H
#define QS_APPLICATION_H

/*
‘айл содержит определение класса CApplication, который
управл€ет структурами, общими дл€ всего дизассемблера.
CApplication выполн€ет различную инициализацию и подготовку
общих объектов.

 ласс CApplication объ€вл€етс€ только один раз, и в глобальной
пам€ти, поэтому в нЄм можно объ€вл€ть большие массивы как
внутренние переменные.
*/

#include "instruction_set.h"
#include "file_parser.h"
#include "explored_image.h"
#include <list>

class CApplication
{
public:				//общедоступные объекты

	//наборы инструкций
	CInstructionSet mInstructionSet16;
	CInstructionSet mInstructionSet32;

	//имена классов окон
	LPCTSTR mszMainWindowClass;
	LPCTSTR mszDangleGroupWindowClass;
	LPCTSTR mszDangleWindowClass;

	//объекты и константы GDI
	static const COLORREF mBrushSelectColor;
	static const COLORREF mBrushSlackSelectColor;
	HBRUSH mhBrushSelect;
	HBRUSH mhBrushSlackSelect;
	HPEN mhPenSelect;
	HFONT mhFontCode;
	HFONT mhFontCodeBold;
	//высота строки со шрифтом mhFontCode
	INT mFontCodeHeight;

	//главное окно
	HWND mhMainWindow;

	//вторичный буфер дл€ рисовани€ без мерцани€
	HDC mhdcBackBuffer;
	HBITMAP mbmpBackBuffer;

public:				//объекты программы

	//список загруженных файлов
	std::list<CExploredImage*> mExploredImagesList;

protected:
	//загрузить инструкции (реализаци€ - в load_instructions.cpp)
	VOID LoadInstructions();

public:
	VOID Initialize();
	VOID Uninitialize();

	//получить список анализаторов файлов (удаление анализаторов выполн€етс€ вызывающей функцией с помощью Release)
	VOID GetFileParsers(std::list<CFileParser*>* pList);

	//добавить загруженный файл в список
	VOID AddExploredImage(CExploredImage* pImage);
	//получить список загруженных файлов
	std::list<CExploredImage*>* GetExploredImagesList();
};

//объ€вление глобального объекта Application
extern CApplication gApplication;

#endif
