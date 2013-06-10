#ifndef QS_DIALOG_LIST_EXPORTS_H
#define QS_DIALOG_LIST_EXPORTS_H

/*
Файл содержит определение класса диалога списка экспорта.
*/

#include "dialog.h"
#include "usercontrol_list.h"
#include "explored_image.h"
#include "image_list_exports.h"

class CDialogListExports : public CDialog
{
protected:
	//класс элемента списка
	class CListElement : public CUserControlListElement
	{
	public:
		//указатель на функцию
		CImageListExports::CExportFunction* mpFunction;

		//конструктор и деструктор
		CListElement(CImageListExports::CExportFunction* pFunction);
		virtual ~CListElement();

		//нарисовать элемент списка
		virtual VOID Draw(HDC hdc,RECT* pRect);
		//получить высоту элемента
		virtual UINT GetHeight();
	};

protected:
	//указатель на объект списка экспорта
	CImageListExports* mpExportsList;
	//список экспорта
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
