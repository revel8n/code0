#include "dialog_instruction_visualizer.h"
#include "application.h"
#include "consts.h"
#include "resource.h"
#include <list>

// class CDialogInstructionVisualizer

#define SHADOW_WIDTH					3

HFONT CDialogInstructionVisualizer::mhFontBig=NULL;

BOOL CDialogInstructionVisualizer::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		//обновить изображение
		Update();
		return TRUE;
	case WM_DRAWITEM:							//перерисовка изображения
		//вывести метафайл изображения
		PlayEnhMetaFile(((DRAWITEMSTRUCT*)lParam)->hDC,mhImageMetaFile,&((DRAWITEMSTRUCT*)lParam)->rcItem);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			//закрыть диалог
			EndDialog(mhWindow,0);
			return TRUE;
		}
		return FALSE;
	case WM_DESTROY:
		//удалить метафайл
		if(mhImageMetaFile)
			DeleteEnhMetaFile(mhImageMetaFile);
		//освободить память
		SafeDeleteMassive(mpCode);
		return TRUE;
	}
	return FALSE;
}

//выполнить построение изображения
VOID CDialogInstructionVisualizer::Update()
{
	//получить прямоугольник элемента управления - картинки
	RECT rect;
	GetClientRect(GetDlgItem(mhWindow,IDC_PICTURE_VISUALIZATION),&rect);
	INT Width=rect.right-rect.left;
	INT Height=rect.bottom-rect.top;

	//удалить предыдущий метафайл
	if(mhImageMetaFile)
		DeleteEnhMetaFile(mhImageMetaFile);

	//создать метафайл
	HDC hdc=CreateEnhMetaFile(NULL,NULL,NULL,NULL);

	//нарисовать тень
	SelectPen(hdc,GetStockPen(BLACK_PEN));
	SelectBrush(hdc,GetStockBrush(BLACK_BRUSH));
	Rectangle(hdc,SHADOW_WIDTH,SHADOW_WIDTH,Width,Height);
	//нарисовать рамку
	SelectBrush(hdc,GetStockBrush(WHITE_BRUSH));
	Rectangle(hdc,0,0,Width-SHADOW_WIDTH,Height-SHADOW_WIDTH);

	//если код задан
	if(mCodeLength)
		//сформировать изображение
		MakeImage(hdc);
	//иначе код не задан
	else
	{
		//вывести сообщение
		SelectFont(hdc,gApplication.mhFontCode);
		DrawText(hdc,TEXT("Enter text into \"code\" edit box."),-1,&rect,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	//закрыть метафайл
	mhImageMetaFile=CloseEnhMetaFile(hdc);
}

//сформировать изображение кода инструкции
VOID CDialogInstructionVisualizer::MakeImage(HDC hdc)
{
	//список "кусков"
	std::list<CHUNK> ChunksList;

	//текущая граница рисования
	INT CurrentMargin=SHADOW_WIDTH;

	//найти префиксы
	for(UINT i=0;i<mCodeLength;++i)
	{
		//получить байт
		BYTE Byte=mpCode[i];
		//если этот байт - префикс
		if(mpInstructionSet->mInstructionBytes[i].bFlags & (IBF_OK | IBF_PREFIX))
		{
			//получить флаг и номер префикса
			DWORD PrefixFlag=mpInstructionSet->mInstructionBytes[i].dwPrefixFlag;
			for(BYTE Prefix=0;Prefix<PREFIXES_COUNT;++Prefix)
				if(PrefixFlag==(1<<Prefix))
					break;
			//добавить кусок
			CHUNK Chunk;
			Chunk.FirstByte=i;
			Chunk.BytesCount=1;
			Chunk.szCode=new TCHAR[3];
			_stprintf(Chunk.szCode,TEXT("%02x"),Byte);
			Chunk.szMessage=new TCHAR[26+_tcslen(gcszPrefixNames[Prefix])];
			_stprintf(Chunk.szMessage,TEXT("prefix byte for \'%s\' prefix"),gcszPrefixNames[Prefix]);

			ChunksList.push_back(Chunk);
		}
	}

	//обработать код инструкции
}




























CDialogInstructionVisualizer::CDialogInstructionVisualizer()
{
	//инициализировать переменные
	mhImageMetaFile=NULL;

	//создать шрифт, если он еще не создан
	if(!mhFontBig)
	{
		LOGFONT lf;
		GetObject(gApplication.mhFontCodeBold,sizeof(lf),&lf);
		lf.lfHeight*=3;
		mhFontBig=CreateFontIndirect(&lf);
	}
}

VOID CDialogInstructionVisualizer::Show(HWND hWndParent)
{
	//инициализировать код
	mpCode=NULL;
	mCodeLength=0;
	//вывести диалог
	Ask(MAKEINTRESOURCE(IDD_DIALOG_INSTRUCTION_VISUALIZER),hWndParent);
}

VOID CDialogInstructionVisualizer::Show(CCyberMemory* pMemory,CYBER_ADDRESS Address,UINT Size,HWND hWndParent)
{
	//считать код инструкции
	mpCode=new BYTE[Size];
	try
	{
		pMemory->Data(mpCode,Address,Size);
	}
	catch(CCyberMemoryPageFaultException)
	{
		_ASSERTE(("Неожиданное исключение page fault",TRUE));
	}
	//запомнить длину
	mCodeLength=0;
	//вывести диалог
	Ask(MAKEINTRESOURCE(IDD_DIALOG_INSTRUCTION_VISUALIZER),hWndParent);
}
