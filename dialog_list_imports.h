#ifndef QS_DIALOG_LIST_IMPORTS_H
#define QS_DIALOG_LIST_IMPORTS_H

/*
Файл содержит определение класса диалога списка импорта.
*/

#include "dialog.h"
#include "usercontrol_list.h"
#include "explored_image.h"
#include "image_list_imports.h"

class CDialogListImports : public CDialog
{
protected:
	//класс элемента списка
	class CListElement : public CUserControlListElement
	{
	public:
		//флаг - это библиотека?
		BOOL mIsLibrary;
		union
		{
			//указатель на библиотеку
			CImageListImports::CImportLibrary* mpLibrary;
			//указатель на функцию
			CImageListImports::CImportFunction* mpFunction;
		};

		//конструкторы и деструктор
		CListElement(CImageListImports::CImportFunction* pFunction);
		CListElement(CImageListImports::CImportLibrary* pLibrary);
		virtual ~CListElement();

		//нарисовать элемент списка
		virtual VOID Draw(HDC hdc,RECT* pRect);
		//получить высоту элемента
		virtual UINT GetHeight();
	};

protected:
	//указатель на объект списка импорта
	CImageListImports* mpImportsList;
	//список импорта
	CUserControlList* mpList;
	//указатель на образ
	CExploredImage* mpExploredImage;
	//указатель на адрес
	CYBER_ADDRESS* mpAddress;

protected:
	//оконная процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);
	//выполнить заполнение списка
	VOID FillList(LPCTSTR szFilter);

public:
	//вызов диалога
	//возвращается адрес, в который нужно перейти
	BOOL Show(CExploredImage* pExploredImage,CYBER_ADDRESS* pAddress,HWND hWndParent);
};

#endif
