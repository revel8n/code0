#include "usercontrol_list.h"
#include "application.h"

// class CUserControlListElement

CUserControlListElement::~CUserControlListElement()
{
	//ничего не делать, это заглушка
}

// class CUserControlList

CUserControlList::CUserControlList(UINT ControlID,HWND hWindow)
{
	//запомнить идентификатор элемента управлени€, его окно и выоту элементов управлени€
	mControlID=ControlID;
	mhWindow=hWindow;
}

CUserControlList::~CUserControlList()
{
	//очистить список
	Clear();
}

VOID CUserControlList::AddElement(CUserControlListElement* pElement)
{
	//добавить элемент в список, указав класс элемента в качестве данных элемента
	ListBox_AddString(mhWindow,pElement);
}

CUserControlListElement* CUserControlList::GetSelectedElement()
{
	//получить индекс выделенного элемента
	UINT ElementIndex=ListBox_GetCurSel(mhWindow);
	if(ElementIndex==LB_ERR) return NULL;
	return (CUserControlListElement*)ListBox_GetItemData(mhWindow,ElementIndex);
}

VOID CUserControlList::DeleteSelectedElement()
{
	//получить выделенный элемент
	UINT ElementIndex=ListBox_GetCurSel(mhWindow);
	if(ElementIndex!=LB_ERR)
	{
		//удалить элемент
		delete (CUserControlListElement*)ListBox_GetItemData(mhWindow,ElementIndex);
		ListBox_DeleteString(mhWindow,ElementIndex);
	}
}

VOID CUserControlList::Clear()
{
	//удалить все элементы в списке
	while(ListBox_GetCount(mhWindow))
	{
		//удалить элемент
		delete (CUserControlListElement*)ListBox_GetItemData(mhWindow,0);
		ListBox_DeleteString(mhWindow,0);
	}
}

BOOL CUserControlList::IsExistSelection()
{
	//вернуть ответ
	return ListBox_GetCurSel(mhWindow)!=LB_ERR;
}

BOOL CUserControlList::IsEmpty()
{
	//вернуть ответ
	return ListBox_GetCount(mhWindow)==0;
}

void CUserControlList::BeginUpdate()
{
	SendMessage(mhWindow, WM_SETREDRAW, FALSE, 0);
}

void CUserControlList::EndUpdate()
{
	SendMessage(mhWindow, WM_SETREDRAW, TRUE, 0);
}

BOOL CUserControlList::ProcessMessages(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//проверить, что сообщение относитс€ именно к этому списку
	if(wParam!=mControlID) return FALSE;

	//выбор по сообщению
	switch(Msg)
	{
	case WM_DRAWITEM:				//рисование
		{
			//получить структуру с информацией о рисовании
			DRAWITEMSTRUCT* pDrawItem=(DRAWITEMSTRUCT*)lParam;
			//если элементов нет, закончить
			if(pDrawItem->itemID==LB_ERR) return TRUE;

			//кэшировать некоторые параметры
			HDC hdc=pDrawItem->hDC;
			RECT Rect=pDrawItem->rcItem;

			//указать шрифт
			HFONT hLastFont=SelectFont(hdc,gApplication.mhFontCode);
			//установить режим прозрачности
			SetBkMode(hdc,TRANSPARENT);
			//выделить перо и кисть в контекст
			HPEN hLastPen=SelectPen(hdc,gApplication.mhPenSelect);
			HBRUSH hLastBrush=SelectBrush(hdc,gApplication.mhBrushSelect);

			//если элемент выделен
			if(pDrawItem->itemState & ODS_SELECTED)
				//нарисовать выдел€ющий пр€моугольник
				Rectangle(hdc,Rect.left,Rect.top,Rect.right,Rect.bottom);
			//иначе очистить пр€моугольник
			else
				FillRect(hdc,&Rect,GetStockBrush(WHITE_BRUSH));

			//нарисовать элемент
			((CUserControlListElement*)pDrawItem->itemData)->Draw(hdc,&Rect);

			//вернуть прежние перо и кисть
			SelectPen(hdc,hLastPen);
			SelectBrush(hdc,hLastBrush);
			//вернуть шрифт
			SelectFont(hdc,hLastFont);
		}
		return TRUE;
	case WM_MEASUREITEM:		//создание элементов
		{
			//получить структуру
			MEASUREITEMSTRUCT* pMeasureItem=(MEASUREITEMSTRUCT*)lParam;
			//указать высоту элемента
			pMeasureItem->itemHeight=((CUserControlListElement*)pMeasureItem->itemData)->GetHeight();
		}
		return TRUE;
	}
	//сообщение не обработано
	return FALSE;
}
