#include "dialog_messagebox.h"
#include "application.h"
#include "resource.h"

// class CDialogMessageBox

BOOL CDialogMessageBox::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//получить размеры текста
			HDC hdc=GetDC(NULL);
			SelectFont(hdc,gApplication.mhFontCode);
			SIZE Size;
			Size.cx=0;
			Size.cy=0;
			{
				//посчитать размеры построчно
				UINT Length=_tcslen(mszText);
				UINT i,j;
				for(i=0;i<Length;++i)
				{
					for(j=i+1;j<Length;++j)
						if(mszText[j]=='\n')
							break;
					SIZE LineSize;
					GetTextExtentPoint32(hdc,mszText+i,j-i,&LineSize);
					if(Size.cx<LineSize.cx) Size.cx=LineSize.cx;
					Size.cy+=LineSize.cy;
					i=j-1;
				}
			}
			ReleaseDC(NULL,hdc);

			//получить прямоугольник диалога
			RECT DialogRect;
			GetWindowRect(mhWindow,&DialogRect);
			//получить прямоугольник надписи
			HWND hStaticText=GetDlgItem(mhWindow,IDC_STATIC_TEXT);
			RECT StaticTextRect;
			GetWindowRect(hStaticText,&StaticTextRect);
			//получить прямоугольник области для кнопок
			HWND hButtonsArea=GetDlgItem(mhWindow,IDC_BUTTON_AREA);
			RECT ButtonsRect;
			GetWindowRect(hButtonsArea,&ButtonsRect);
			//удалить кнопку-область
			DestroyWindow(hButtonsArea);
			//получить сдвиг прямоугольника относительно надписи от верхнего края
			INT ButtonsOffsetY=ButtonsRect.top-StaticTextRect.bottom;

			//вычислить новый размер диалога
			INT DialogWidth=DialogRect.right-DialogRect.left-(StaticTextRect.right-StaticTextRect.left)+Size.cx;
			INT DialogHeight=DialogRect.bottom-DialogRect.top-(StaticTextRect.bottom-StaticTextRect.top)+Size.cy;
			//получить прямоугольник родительского окна
			RECT ParentRect;
			GetWindowRect(mhWndParent,&ParentRect);
			//указать размер и положение диалога
			MoveWindow(mhWindow,ParentRect.left+(ParentRect.right-ParentRect.left-DialogWidth)/2,ParentRect.top+(ParentRect.bottom-ParentRect.top-DialogHeight)/2,DialogWidth,DialogHeight,FALSE);

			//указать размер надписи
			SetWindowPos(hStaticText,NULL,0,0,Size.cx,Size.cy,SWP_NOMOVE | SWP_NOZORDER);
			//указать шрифт для надписи
			FORWARD_WM_SETFONT(hStaticText,gApplication.mhFontCode,FALSE,SendMessage);
			
			//указать надпись
			SetWindowText(hStaticText,mszText);
			//указать заголовок окна
			if(mszCaption) SetWindowText(mhWindow,mszCaption);

			//получить новое положение надписи в диалоге
			GetWindowRect(hStaticText,&StaticTextRect);
			MapWindowPoints(NULL,mhWindow,(LPPOINT)&StaticTextRect,2);
			//вычислить верхнюю координату для кнопок
			INT ButtonsTop=StaticTextRect.bottom+ButtonsOffsetY;
			//ширина кнопки - задана в конструкторе
			INT ButtonWidth=ButtonsRect.right-ButtonsRect.left;
			//вычислить расстояние между кнопками
			INT ButtonSpace=ButtonWidth+ButtonWidth/7;
			//вычислить отступ слева
			INT ButtonsLeft=StaticTextRect.left+(StaticTextRect.right-StaticTextRect.left-ButtonSpace*mButtonsCount+ButtonSpace-ButtonWidth)/2;
			//получить высоту кнопки
			INT ButtonHeight=ButtonsRect.bottom-ButtonsRect.top;
			//создать кнопки
			for(UINT i=0;i<mButtonsCount;++i)
			{
				HWND hButton=CreateWindowEx(WS_EX_NOPARENTNOTIFY,TEXT("Button"),mszButtonNames[i],WS_VISIBLE | WS_CHILDWINDOW | WS_TABSTOP | BS_TEXT /*| (i==mDefaultButton ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON)*/ | BS_PUSHBUTTON | (!i ? WS_GROUP : 0),
					ButtonsLeft+i*ButtonSpace,ButtonsTop,ButtonWidth,ButtonHeight,mhWindow,NULL,GetModuleHandle(NULL),NULL);
				SetWindowLong(hButton,GWL_USERDATA,i);
				//указать шрифт
				FORWARD_WM_SETFONT(hButton,gApplication.mhFontCode,FALSE,SendMessage);
			}
			//выполнить TAB
			FORWARD_WM_NEXTDLGCTL(mhWindow,0,TRUE,PostMessage);

			//указать иконку
			if(mhIcon)
				SendDlgItemMessage(mhWindow,IDC_STATIC_ICON,STM_SETIMAGE,(WPARAM)IMAGE_BITMAP,(LPARAM)mhIcon);
		}
		return TRUE;
	case WM_COMMAND:
		if(lParam)
			//получить номер нажатой кнопки и закрыть диалог
			EndDialog(mhWindow,GetWindowLong((HWND)lParam,GWL_USERDATA));
		return TRUE;
	}
	return FALSE;
}

UINT CDialogMessageBox::Show(HWND hWndParent,LPCTSTR szText,LPCTSTR szCaption,UINT ButtonsCount,LPCTSTR* szButtonNames,HANDLE hIcon,UINT DefaultButton)
{
	//запомнить параметры
	mhWndParent=hWndParent;
	mszText=szText;
	mszCaption=szCaption;
	mButtonsCount=ButtonsCount;
	mszButtonNames=szButtonNames;
	mhIcon=hIcon;
	mDefaultButton=DefaultButton;

	//вывести диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_MESSAGEBOX),hWndParent);
}
