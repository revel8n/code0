#include "danglewindow_code.h"
#include "application.h"
#include "dialog_gotoaddress.h"
#include "dialog_gotoxreference.h"
#include "dialog_editlabel.h"
#include "dialog_editcomment.h"
#include "dialog_list_imports.h"
#include "dialog_list_exports.h"
//#include "dialog_instruction_visualizer.h"
#include "xreference.h"
#include "resource.h"

// class CDangleWindowCode

CDangleWindowCode::CDangleWindowCode(CExploredImage* pExploredImage,CYBER_ADDRESS Address,LPCTSTR szFileName)
{
	//получить и запомнить указатели на необходимые объекты
	mpExploredImage=pExploredImage;
	mpExploredImage->AddRef();
	mpAddressMap=pExploredImage->GetAddressMap();
	mpMemory=pExploredImage->GetMemory();
	mpCodeParser=pExploredImage->GetCodeParser();
	//создать листинг
	mpListing=new CListingDevice;

	//инициализировать итератор положения в истории просмотра
	mHistoryCurrentIterator=mHistoryList.end();

	//инициализировать конечный адрес
	mEndAddress=Address;
	//перейти к заданному адресу
	GoToAddress(Address);

	//сформировать имя окна
	//получить имя без пути
	TCHAR szFileTitle[MAX_PATH];
	GetFileTitle(szFileName,szFileTitle,MAX_PATH);
	//если оно слишком длинное
	if(_tcslen(szFileTitle)>MAX_DANGLE_NAME_LENGTH)
	{
		//обрезать его
		CopyMemory(mszNameBuffer,szFileTitle,MAX_DANGLE_NAME_LENGTH*sizeof(TCHAR));
		mszNameBuffer[MAX_DANGLE_NAME_LENGTH]=0;
	}
	//иначе взять целиком
	else
		_tcscpy(mszNameBuffer,szFileTitle);

	//установить указатель на имя
	mszName=mszNameBuffer;
	//указать информацию об UI
	mUI.hMenu=LoadMenu(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_MENU_CODE));
	mUI.hAccelerators=LoadAccelerators(GetModuleHandle(NULL),MAKEINTRESOURCE(IDR_ACCELERATOR_CODE));
}

CDangleWindowCode::~CDangleWindowCode()
{
	//удалить листинг
	SafeDelete(mpListing);
	//освободить объекты
	mpExploredImage->Release();
	mpAddressMap->Release();
	mpMemory->Release();
	mpCodeParser->Release();
}

VOID CDangleWindowCode::OffsetView(INT Offset)
{
	//прокрутить вид на указанное расстояние
	if(Offset>0)
		for(INT i=0;i<Offset;++i)
		{
			//сдвинуть вид
			CAddressData* pAddressData=mpAddressMap->GetAddress(mAddress);
			if(pAddressData)
			{
				mAddress+=pAddressData->GetSize();
				pAddressData->Release();
			}
			else mAddress++;
		}
	else if(Offset<0)
		for(INT i=Offset+1;i<=0;++i)
		{
			//сдвинуть вид
			CAddressData* pAddressData=mpAddressMap->GetAddress(mAddress);
			if(pAddressData)
			{
				mAddress-=pAddressData->GetPreviousSize();
				pAddressData->Release();
			}
			else mAddress--;
		}
}

VOID CDangleWindowCode::OffsetSelection(INT Offset)
{
	//прокрутить выделение на указанное расстояние
	if(Offset>0)
		for(INT i=0;i<Offset;++i)
		{
			//сдвинуть выделение
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			if(pAddressData)
			{
				mSelectedAddress+=pAddressData->GetSize();
				pAddressData->Release();
			}
			else mSelectedAddress++;
		}
	else if(Offset<0)
		for(INT i=Offset+1;i<=0;++i)
		{
			//сдвинуть выделение
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			if(pAddressData)
			{
				mSelectedAddress-=pAddressData->GetPreviousSize();
				pAddressData->Release();
			}
			else mSelectedAddress--;
		}
}

VOID CDangleWindowCode::GoToAddress(CYBER_ADDRESS Address)
{
	//закрепить текущий адрес
	KeepHistoryCurrent();
	//удалить в истории просмотра все элементы, начиная с mHistoryCurrentIterator
	mHistoryList.erase(mHistoryCurrentIterator,mHistoryList.end());

	//если адрес виден в окне
	if(Address>=mAddress && Address<mEndAddress)
		//просто выделить адрес
		mSelectedAddress=Address;
	//иначе
	else
	{
		//перейти к указанному адресу
		mAddress=Address;
		mSelectedAddress=Address;
	}
	//добавить указанный адрес в список
	mHistoryList.push_back(Address);
	//указать новый текущий итератор
	mHistoryCurrentIterator=mHistoryList.end();
	//обновить окно
	InvalidateRect(mhWindow,NULL,FALSE);
}

VOID CDangleWindowCode::GoHistoryBack()
{
	//получить предыдущий итератор истории
	std::list<CYBER_ADDRESS>::iterator i=mHistoryCurrentIterator;
	--i;
	//если этот итератор не первый
	if(i!=mHistoryList.begin())
	{
		//закрепить текущий адрес
		KeepHistoryCurrent();
		//передвинуть текущий итератор назад
		--mHistoryCurrentIterator;
		//получить новый адрес и указать его
		mAddress=*(--i);
		mSelectedAddress=mAddress;
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
	}
}

VOID CDangleWindowCode::GoHistoryForward()
{
	//если текущий итератор не последний
	if(mHistoryCurrentIterator!=mHistoryList.end())
	{
		//закрепить текущий адрес
		KeepHistoryCurrent();
		//получить новый адрес и указать его
		mAddress=*(mHistoryCurrentIterator++);
		mSelectedAddress=mAddress;
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
	}
}

VOID CDangleWindowCode::KeepHistoryCurrent()
{
	//получить итератор текущей записи
	std::list<CYBER_ADDRESS>::iterator i=mHistoryCurrentIterator;
	if(i!=mHistoryList.begin()) *(--i)=mSelectedAddress;
	else
	{
		mHistoryList.push_back(mSelectedAddress);
		mHistoryCurrentIterator=mHistoryList.end();
	}
}

VOID CDangleWindowCode::Paint(HDC hdc)
{
	//получить прямоугольник окна
	RECT Rect;
	GetClientRect(mhWindow,&Rect);

	//указать контекст для листинга
	LISTING_DEVICE_CONTEXT Context;
	Context.hdc=hdc;
	Context.Rect=Rect;
	mpListing->SetContext(Context);
	//указать карту адресов
	mpListing->SetAddressMap(mpAddressMap);

	//очистить окно
	SelectBrush(hdc,GetStockBrush(WHITE_BRUSH));
	PatBlt(hdc,Rect.left,Rect.top,Rect.right,Rect.bottom,PATCOPY);
	//выделить нужные перо и кисть
	SelectPen(hdc,gApplication.mhPenSelect);
	SelectBrush(hdc,gApplication.mhBrushSelect);
	//выделить шрифт
	SelectFont(hdc,gApplication.mhFontCode);

	//напечатать контрольный адрес
	{
		//установить режим контрольного адреса
		mpListing->SetControlGrouping();
		CAddressData* pData=mpAddressMap->GetAddress(mSelectedAddress);
		if(pData)
		{
			//указать текущий адрес
			mpListing->SetAddress(mSelectedAddress);
			//напечатать данные адреса
			pData->Print(mpListing);
			//освободить данные
			pData->Release();
		}
		//сгруппировать строки (это также отключит контрольный адрес)
		mpListing->GroupLines();
	}

	//цикл печати
	CYBER_ADDRESS Address=mAddress;
	for(;;)
	{
		//получить данные адреса
		CAddressData* pData=mpAddressMap->GetAddress(Address);

		//установить флаг выделенности, если нужно
		mpListing->SetSelecting(Address==mSelectedAddress);

		//указать текущий адрес
		mpListing->SetAddress(Address);

		if(pData)
		{
			//напечатать данные адреса
			pData->Print(mpListing);
			//сдвинуть адрес на размер адреса
			Address+=pData->GetSize();
			//освободить данные адреса
			pData->Release();
		}
		else
		{
			//напечатать, что интерпретации нет
			mpListing->BeginLine();
			mpListing->Print(mpListing->GetColumnMargin(LISTING_COLUMN_NAME),TEXT("(no interpretation)"));
			mpListing->EndLine();
			//сдвинуть адрес на 1
			Address++;
		}

		//сгруппировать напечатанные строки
		mpListing->GroupLines();

		//если печать вышла за границы прямоугольника, закончить
		if(mpListing->IsOverflow()) break;
	}
	//вычислить конечный адрес
	mEndAddress=Address-2;
}

VOID CDangleWindowCode::OnKey(UINT Key)
{
	//выбор по нажатой клавише
	switch(Key)
	{
	case VK_DOWN:							//вниз
		//если клавиша Control нажата
		if(GetKeyState(VK_CONTROL)<0)
		{
			//сдвинуть вид
			OffsetView(1);
			//сдвинуть выделение
			OffsetSelection(1);
		}
		else
		{
			//сдвинуть выделение
			OffsetSelection(1);
			//если выделенный адрес не виден на экране
			if(mSelectedAddress>=mEndAddress)
				//сдвинуть и вид
				OffsetView(1);
		}

		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case VK_UP:								//вверх
		//если клавиша Control нажата
		if(GetKeyState(VK_CONTROL)<0)
		{
			//сдвинуть вид
			OffsetView(-1);
			//сдвинуть выделение
			OffsetSelection(-1);
		}
		else
		{
			//сдвинуть выделение
			OffsetSelection(-1);
			//если выделенный адрес не виден на экране
			if(mSelectedAddress<mAddress)
				//сдвинуть и вид
				OffsetView(-1);
		}

		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case VK_NEXT:							// PAGE DOWN
		//сдвинуть вид
		OffsetView(0x10);
		//сдвинуть выделение
		OffsetSelection(0x10);

		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case VK_PRIOR:						// PAGE UP
		//сдвинуть вид
		OffsetView(-0x10);
		//сдвинуть выделение
		OffsetSelection(-0x10);

		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	}
}

VOID CDangleWindowCode::OnCommand(UINT CommandID,UINT Code,HWND hControl)
{
	//выбор по идентификатору
	switch(CommandID)
	{
		//** меню "Code"
	case MID_CODE_CODE_SAVELISTING:
		//экспортировать листинг в файл (TEST)
		mpAddressMap->ExplortListingToFile();
		break;
		//** меню "Information"
	case MID_CODE_INFORMATION_IMPORTSLIST:
		{
			CDialogListImports Dialog;

			//запустить диалог списка импорта
			CYBER_ADDRESS Address;
			if(Dialog.Show(mpExploredImage,&Address,mhWindow))
				//если пользователь захотел, переместиться в нужное место
				GoToAddress(Address);
		}
		break;
	case MID_CODE_INFORMATION_EXPORTSLIST:
		{
			CDialogListExports Dialog;

			//запустить диалог списка экспорта
			CYBER_ADDRESS Address;
			if(Dialog.Show(mpExploredImage,&Address,mhWindow))
				//если пользователь захотел, переместиться в нужное место
				GoToAddress(Address);
		}
		break;
/*	case MID_CODE_INFORMATION_INSTRUCTIONVISUALIZATION:
		{
			//проверить, что интерпретация в выделенном адресе - кодовая
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			if(pAddressData)
			{
				CInterpretation* pInterpretation=pAddressData->GetInterpretation();
				if(pInterpretation)
				{
					if(pInterpretation->GetInterpretationType()==ADEIT_CODE)
					{
						//вывести диалог
						CDialogInstructionVisualizer Dialog;
						Dialog.Show(mpMemory,mSelectedAddress,pAddressData->GetSize(),mhWindow);
					}
					pInterpretation->Release();
				}
				pAddressData->Release();
			}
		}
		break;*/
		//** меню "Navigate"
	case MID_CODE_NAVIGATE_GOTOADDRESS:
		{
			//вывести диалог
			CDialogGoToAddress Dialog;
			CYBER_ADDRESS Address=mSelectedAddress;
			if(Dialog.Show(mhWindow,&Address))
				//перейти по адресу
				GoToAddress(Address);
		}
		break;
	case MID_CODE_NAVIGATE_HISTORYBACK:
		//перейти по истории просмотра назад
		GoHistoryBack();
		break;
	case MID_CODE_NAVIGATE_HISTORYFORWARD:
		//перейти по истории просмотра вперёд
		GoHistoryForward();
		break;
	case MID_CODE_NAVIGATE_GOTOREFERENCE:
		{
			//получить данные адреса
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			if(pAddressData)
			{
				//получить интерпретацию
				CInterpretation* pInterpretation=pAddressData->GetInterpretation();
				//освободить данные адреса
				pAddressData->Release();
				if(pInterpretation)
				{
					//получить прямую ссылку
					CYBER_ADDRESS Address;
					if(pInterpretation->GetReference(&Address))
						//перейти по ссылке
						GoToAddress(Address);
					//освободить интерпретацию
					pInterpretation->Release();
				}
			}
		}
		break;
	case MID_CODE_NAVIGATE_GOTOXREFERENCE:
		{
			//получить данные адреса
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			if(pAddressData)
			{
				//посчитать количество перекрестных ссылок
				CAddressData::ADEL* pElements=pAddressData->GetElements();
				UINT ElementsCount=pElements->count(ADET_XREFERENCE);
				//если есть 1 перекрестная ссылка
				if(ElementsCount==1)
					//просто перейти по ней
					GoToAddress( ((CXReference*)((*pElements->find(ADET_XREFERENCE)).second))->GetAddressFrom() );
				//иначе если есть больше одной ссылки
				else if(ElementsCount>1)
				{
					//вывести диалог
					CDialogGoToXReference Dialog;
					CYBER_ADDRESS Address;
					if(Dialog.Show(mhWindow,pAddressData,mpAddressMap,&Address))
						//перейти по ссылке
						GoToAddress(Address);
				}
				//освободить данные адреса
				pAddressData->Release();
			}
		}
		break;
		//** меню "Interpretate"
	case MID_CODE_INTERPRETATE_DATA_BYTE:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationData(mSelectedAddress,1);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_DATA_WORD:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationData(mSelectedAddress,2);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_DATA_DWORD:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationData(mSelectedAddress,4);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_CODE:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationCode(mSelectedAddress);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_UNICODESTRING:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationStringUnicode(mSelectedAddress);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_ASCIISTRING:
		//указать интерпретацию
		mpExploredImage->SetAddressInterpretationStringASCII(mSelectedAddress);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_DELETE:
		mpExploredImage->DeleteAddressIntepretation(mSelectedAddress);
		//обновить окно
		InvalidateRect(mhWindow,NULL,FALSE);
		break;
	case MID_CODE_INTERPRETATE_EDITLABEL:
		{
			//получить метку
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			CLabel* pExistLabel;
			LPCTSTR szLabel=NULL;
			if(pAddressData)
			{
				pExistLabel=pAddressData->GetLabel();
				pAddressData->Release();
				if(pExistLabel)
					szLabel=pExistLabel->GetName();
			}
			else
				pExistLabel=NULL;
			//вывести окно редактирования метки
			CDialogEditLabel Dialog;
			LPTSTR szResultLabel;
			if(Dialog.Show(szLabel,&szResultLabel,mhWindow))
			{
				//если метка есть
				if(szResultLabel)
				{
					//создать новую метку
					CLabel* pNewLabel=new CLabel(szResultLabel);
					//освободить память строки
					delete [] szResultLabel;
					//открыть данные адреса
					pAddressData=mpAddressMap->OpenAddress(mSelectedAddress);
					//указать метку
					pAddressData->SetLabel(pNewLabel);
					//освободить метку
					pNewLabel->Release();
					//освободить данные адреса
					pAddressData->Release();
				}
				//иначе нужно удалить метку
				else
					//если метка была
					if(pExistLabel)
					{
						//удалить метку для данных адреса
						pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
						pAddressData->SetLabel(NULL);
						pAddressData->Release();
					}
				//обновить окно
				InvalidateRect(mhWindow,NULL,FALSE);
			}
			//освободить метку
			SafeRelease(pExistLabel);
		}
		break;
	case MID_CODE_INTERPRETATE_EDITCOMMENT:
		{
			//получить комментарий
			CAddressData* pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
			CComment* pExistComment;
			LPCTSTR szComment=NULL;
			if(pAddressData)
			{
				pExistComment=pAddressData->GetComment();
				pAddressData->Release();
				if(pExistComment)
					szComment=pExistComment->GetText();
			}
			else
				pExistComment=NULL;
			//вывести окно редактирования комментария
			CDialogEditComment Dialog;
			LPTSTR szResultComment;
			if(Dialog.Show(szComment,&szResultComment,mhWindow))
			{
				//если комментарий есть
				if(szResultComment)
				{
					//создать новый комментарий
					CComment* pNewComment=new CComment(szResultComment);
					//освободить память строки
					delete [] szResultComment;
					//открыть данные адреса
					pAddressData=mpAddressMap->OpenAddress(mSelectedAddress);
					//указать комментарий
					pAddressData->SetComment(pNewComment);
					//освободить комментарий
					pNewComment->Release();
					//освободить данные адреса
					pAddressData->Release();
				}
				//иначе нужно удалить комментарий
				else
					//если комментарий был
					if(pExistComment)
					{
						//удалить комментарий для данных адреса
						pAddressData=mpAddressMap->GetAddress(mSelectedAddress);
						pAddressData->SetComment(NULL);
						pAddressData->Release();
					}
				//обновить окно
				InvalidateRect(mhWindow,NULL,FALSE);
			}
			//освободить комментарий
			SafeRelease(pExistComment);
		}
		break;
	case MID_CODE_INTERPRETATE_CREATESUBROUTINE:
		{
			//создать процедуру для данного кода
			CAddressData* pAddressData=mpAddressMap->OpenAddress(mSelectedAddress);
			CInterpretation* pInterpretation=pAddressData->GetInterpretation();
			if(pInterpretation)
			{
				if(pInterpretation->GetInterpretationType()==ADEIT_CODE)
				{
					pAddressData->CreateSubroutine();
					//обновить окно
					InvalidateRect(mhWindow,NULL,FALSE);
				}
				pInterpretation->Release();
			}
			pAddressData->Release();
		}
		break;
	}
}

VOID CDangleWindowCode::OnMouseWheel(INT Delta,UINT Keys)
{
	//немного ускорить прокрутку и направить её в правильном направлении
	Delta*=(-3);
	//прокрутить окно на указанное расстояние
	OffsetView(Delta);
	OffsetSelection(Delta);

	//обновить окно
	InvalidateRect(mhWindow,NULL,FALSE);
}

LRESULT CDangleWindowCode::DangleProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_KEYDOWN:
		return OnKey(wParam),0;
	case WM_COMMAND:
		return OnCommand(LOWORD(wParam),HIWORD(wParam),(HWND)lParam),0;
	case WM_MOUSEWHEEL:
		return OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)/WHEEL_DELTA,GET_KEYSTATE_WPARAM(wParam)),0;
	default:
		return DefWindowProc(mhWindow,Msg,wParam,lParam);
	}
}
