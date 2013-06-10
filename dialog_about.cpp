#include "dialog_about.h"
#include "resource.h"

// class CDialogAbout

BOOL CDialogAbout::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//загрузить картинку из ресурсов
			HDC hdc=GetDC(NULL);
			mhdcImage=CreateCompatibleDC(hdc);
			mbmpImage=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP_KNOT),IMAGE_BITMAP,0,0,0);
			SelectBitmap(mhdcImage,mbmpImage);
			ReleaseDC(NULL,hdc);
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(mhWindow,0);
		return TRUE;
	case WM_DESTROY:
		//выгрузить картинку
		DeleteDC(mhdcImage);
		DeleteBitmap(mbmpImage);
		return TRUE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc=BeginPaint(mhWindow,&ps);
			BitBlt(hdc,0,16,128,128,mhdcImage,0,0,SRCCOPY);
			EndPaint(mhWindow,&ps);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(mhWindow,0);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

VOID CDialogAbout::Show(HWND hWndParent)
{
	//показать диалог
	Ask(MAKEINTRESOURCE(IDD_DIALOG_ABOUT),hWndParent);
}
