#include "dialog_peparsersettings.h"
#include "resource.h"

// class CDialogPEParserSettings

BOOL CDialogPEParserSettings::DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam)
{
	//выбор по сообщению
	switch(Msg)
	{
	case WM_INITDIALOG:
		{
			//указать адрес загрузки
			TCHAR s[0x40];
			_stprintf(s,TEXT("%08X"),mpSettings->BaseAddress);
			SetDlgItemText(mhWindow,IDC_EDIT_BASEADDRESS,s);
			//установить переключатель
			CheckDlgButton(mhWindow,(mpSettings->Flags & PEPSF_USE_PREFERRED_BASE_ADDRESS) ? IDC_RADIO_IMAGEBASEPREFERRED : IDC_RADIO_IMAGEBASESPECIFY,BST_CHECKED);
			//установить включенность поля выбираемого адреса
			EnableWindow(GetDlgItem(mhWindow,IDC_EDIT_BASEADDRESS),!(mpSettings->Flags & PEPSF_USE_PREFERRED_BASE_ADDRESS));
			//установить текст переключателя "Preferred address"
			_stprintf(s,TEXT("Preferred base address (%08Xh)"),mpSettings->PreferredBaseAddress);
			SetDlgItemText(mhWindow,IDC_RADIO_IMAGEBASEPREFERRED,s);
			//установить флажки
			CheckDlgButton(mhWindow,IDC_CHECK_ANALIZE_DOS_STUB,(mpSettings->Flags & PEPSF_ANALIZE_DOS_STUB) ? BST_CHECKED : BST_UNCHECKED);
			CheckDlgButton(mhWindow,IDC_CHECK_ANALIZE_EXPORTS_CODE,(mpSettings->Flags & PEPSF_ANALIZE_EXPORTS_CODE) ? BST_CHECKED : BST_UNCHECKED);
		}
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				//получить указанный адрес
				{
					TCHAR s[0x10];
					GetDlgItemText(mhWindow,IDC_EDIT_BASEADDRESS,s,0x10);
					CYBER_ADDRESS Address;
					_stscanf(s,TEXT("%x"),&Address);
					//проверить, что адрес выровнен на границу страницы
					if(Address % MEMORY_PAGE_SIZE)
					{
						MessageBox(mhWindow,TEXT("Base address must be aligned for 1000h bytes."),TEXT("Error in entered data"),MB_ICONSTOP);
						SetFocus(GetDlgItem(mhWindow,IDC_EDIT_BASEADDRESS));
						return TRUE;
					}
					mpSettings->BaseAddress=Address;
				}
				//установить флаги
				mpSettings->Flags=
					(IsDlgButtonChecked(mhWindow,IDC_RADIO_IMAGEBASEPREFERRED)==BST_CHECKED ? PEPSF_USE_PREFERRED_BASE_ADDRESS : 0) |
					(IsDlgButtonChecked(mhWindow,IDC_CHECK_ANALIZE_DOS_STUB)==BST_CHECKED ? PEPSF_ANALIZE_DOS_STUB : 0) |
					(IsDlgButtonChecked(mhWindow,IDC_CHECK_ANALIZE_EXPORTS_CODE)==BST_CHECKED ? PEPSF_ANALIZE_EXPORTS_CODE : 0)
					;

				EndDialog(mhWindow,TRUE);
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(mhWindow,FALSE);
			return TRUE;
		case IDC_RADIO_IMAGEBASEPREFERRED:
			EnableWindow(GetDlgItem(mhWindow,IDC_EDIT_BASEADDRESS),FALSE);
			return TRUE;
		case IDC_RADIO_IMAGEBASESPECIFY:
			EnableWindow(GetDlgItem(mhWindow,IDC_EDIT_BASEADDRESS),TRUE);
			return TRUE;
		}
		return FALSE;
	case WM_CLOSE:
		EndDialog(mhWindow,FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL CDialogPEParserSettings::Show(HWND hWndParent,CPEParser::SETTINGS* pSettings)
{
	//запомнить указатель на настройки
	mpSettings=pSettings;
	//вызвать диалог
	return Ask(MAKEINTRESOURCE(IDD_DIALOG_PEPARSERSETTINGS),hWndParent);
}
