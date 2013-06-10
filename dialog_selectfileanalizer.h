#ifndef QS_DIALOG_SELECTFILEANALIZER_H
#define QS_DIALOG_SELECTFILEANALIZER_H

/*
Файл содержит определение диалога выбора анализатора файла.
*/

#include "dialog.h"
#include "file_parser.h"
#include "usercontrol_list.h"
#include <list>

class CDialogSelectFileAnalizer : public CDialog
{
protected:
	class CListElement;
protected:
	//иня файла
	LPCTSTR mszFileName;
	//указатель на проекцию файла
	PVOID mpFileMapping;
	//размер файла
	DWORD mFileSize;

	//список анализаторов
	std::list<CFileParser*> mParsersList;
	//список анализаторов в диалоге
	CUserControlList* mpList;
	//выбранный анализатор (итератор)
	std::list<CFileParser*>::iterator mSelectedParserIterator;

protected:
	//диалоговая процедура
	virtual BOOL DlgProc(UINT Msg,WPARAM wParam,LPARAM lParam);

	//обновить информацию о памяти в окне
	VOID UpdateMemoryInformation();

public:
	//показать диалог (возвращается настроенный анализатор, который вызывающая процедура должна удалить с помощью Reelase)
	CFileParser* Show(HWND hWndParent,LPCTSTR szFileName,PVOID pFileMapping,DWORD FileSize);
};

#endif
