#ifndef QS_CODE_EXPLORER_WINDOW_H
#define QS_CODE_EXPLORER_WINDOW_H

/*
Файл содержит определение класса окна просмотра.
Каждый загруженный файл может иметь несколько окон просмотра.
*/

#include "dangle_window.h"
#include "address_map.h"
#include "cyber_memory.h"
#include "code_parser.h"
#include "explored_image.h"
#include "listing_device.h"
#include <list>

//класс поддержки окна просмотра
class CDangleWindowCode : public CDangleWindow
{
protected:
	//указатель на загруженный файл
	CExploredImage* mpExploredImage;
	//указатель на карту адресов
	CAddressMap* mpAddressMap;
	//указатель на киберпамять
	CCyberMemory* mpMemory;
	//указатель на анализатор кода
	CCodeParser* mpCodeParser;

	//указатель на листинг, использующийся для печати
	CListingDevice* mpListing;
	//текущий адрес просмотра
	CYBER_ADDRESS mAddress;
	//выделенный адрес
	CYBER_ADDRESS mSelectedAddress;
	//конечный адрес просмотра (последний адрес, который вмещается в окно)
	CYBER_ADDRESS mEndAddress;
	//имя окна
	TCHAR mszNameBuffer[MAX_DANGLE_NAME_LENGTH+1];

	//список истории просмотра
	std::list<CYBER_ADDRESS> mHistoryList;
	//итератор нового положения в списке
	std::list<CYBER_ADDRESS>::iterator mHistoryCurrentIterator;

protected:
	//процедуры обработки сообщений
	VOID OnKey(UINT Key);
	VOID OnCommand(UINT CommandID,UINT Code,HWND hControl);
	VOID OnMouseWheel(INT Delta,UINT Keys);

	//вспомогательные процедуры

	//сдвинуть вид
	VOID OffsetView(INT Offset);
	//сдвинуть выделение
	VOID OffsetSelection(INT Offset);
	//перейти к заданному адресу
	VOID GoToAddress(CYBER_ADDRESS Address);
	//перейти назад в истории просмотра
	VOID GoHistoryBack();
	//перейти вперед в истории просмотра
	VOID GoHistoryForward();
	//закрепить текуций адрес в текущей записи истории просмотра
	VOID KeepHistoryCurrent();

public:
	//конструктор и деструктор
	CDangleWindowCode(CExploredImage* pExploredImage,CYBER_ADDRESS Address,LPCTSTR szFileName);
	virtual ~CDangleWindowCode();

	//оконная процедура
	virtual LRESULT DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//рисование
	virtual VOID Paint(HDC hdc);
};

#endif
