#include "listing_device.h"
#include "application.h"

// class CListingDevice

HPEN CListingDevice::mhDashPen=CreatePen(PS_DASH,1,0);

CListingDevice::CListingDevice()
{
	//инициализировать переменные
	mControlGroup=FALSE;
}

VOID CListingDevice::SetSelecting(BOOL Select)
{
	//запомнить флаг выделени€
	mSelect=Select;
}

VOID CListingDevice::SetContext(LISTING_DEVICE_CONTEXT Context)
{
	//запомнить параметры контекста
	mContext=Context;

	//получить ширину и высоту одного символа
	GetTextExtentPoint32(mContext.hdc,TEXT("W"),1,&mCharSize);

	//установить параметры контекста рисовани€
	SetBkMode(mContext.hdc,TRANSPARENT);
	SetTextColor(mContext.hdc,0);

	//сбросить количество уже напечатанных строк и группировку строк
	mPrintedCount=0;
	mGroupCount=0;
	//сбросить флаг выделени€
	mSelect=FALSE;
}

VOID CListingDevice::SetControlGrouping()
{
	//установить флаг контрольной группы
	mControlGroup=TRUE;
	//очистить набор выдел€емых адресов
	mVisesSet.clear();
}

VOID CListingDevice::EndLine()
{
	//если нет флага контрольной группы
	if(!mControlGroup)
	{
		//кэшировать пр€моугольник
		RECT Rect=mContext.Rect;

		//вычислить координату Y
		INT Y=Rect.top+mPrintedCount*mCharSize.cy;

		//если строка выделена
		if(mSelect)
			//нарисовать выдел€ющий пр€моугольник
			PatBlt(mContext.hdc,Rect.left,Y,Rect.right-Rect.left,mCharSize.cy,PATCOPY);

		//напечатать строку
		TextOut(mContext.hdc,Rect.left,Y,mszLineText,mLineTextLength);

		//увеличить количество напечатанных строк
		mPrintedCount++;
		//увеличить количество строк в группе
		mGroupCount++;
	}
}

VOID CListingDevice::GroupLines()
{
	//если есть флаг контрольной группы
	if(mControlGroup)
	{
		//сбросить флаг
		mControlGroup=FALSE;
	}
	else
		//если последн€€ группа выделена,
		//или адрес группы есть в наборе выдел€емых адресов
		if(mSelect || (mVisesSet.find(mAddress)!=mVisesSet.end()))
		{
			//нарисовать группирующий пр€моугольник
			HBRUSH hLastBrush=SelectBrush(mContext.hdc,GetStockBrush(NULL_BRUSH));
			HPEN hLastPen=SelectPen(mContext.hdc,mSelect ? GetStockPen(BLACK_PEN) : mhDashPen);
			Rectangle(mContext.hdc,
				mContext.Rect.left,mContext.Rect.top+(mPrintedCount-mGroupCount)*mCharSize.cy,
				mContext.Rect.right,mContext.Rect.top+mPrintedCount*mCharSize.cy
/*				mContext.Rect.left+mCharSize.cx*(mColumnMargins[LISTING_COLUMN_LABEL]-1),
				mContext.Rect.top+(mPrintedCount-mGroupCount)*mCharSize.cy,
				mContext.Rect.left+mCharSize.cx*mColumnMargins[LISTING_COLUMN_LABEL],
				mContext.Rect.top+mPrintedCount*mCharSize.cy*/
				);
			SelectBrush(mContext.hdc,hLastBrush);
			SelectPen(mContext.hdc,hLastPen);
		}
	//сбросить группу
	mGroupCount=0;
}

BOOL CListingDevice::IsOverflow()
{
	//вернуть, есть ли переполнение
	return (mPrintedCount*mCharSize.cy)>=(mContext.Rect.bottom-mContext.Rect.top);
}

UINT CListingDevice::PrintOffset(UINT Address)
{
	//если включен флаг контрольной группы
	if(mControlGroup)
		//добавить выводимый адрес в набор выдел€емых адресов
		mVisesSet.insert(Address);
	//вывести адрес
	return CListing::PrintOffset(Address);
}
