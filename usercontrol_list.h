#ifndef QS_USERCONTROL_LIST_H
#define QS_USERCONTROL_LIST_H

/*
Файл содержит объявления, связанные с поддержкой "фирменного" элемента
управления "список".
Класс CUserControlList обеспечивает рисование и обработку выделения
элементов списка. Само рисование элементов выполняется как вызов метода
элементов класса.

Со списком разрешается использовать большинство стандартных сообщений,
кроме добавления и удаления элементов.
Также разрешается использовать только списки со стилем LBS_OWNERDRAWVARIABLE.
*/

#include "windows.h"

//рекомендуемые отступы слева для элементов списков
#define LIST_ELEMENT_MARGIN_HEADER		20
#define LIST_ELEMENT_MARGIN_TEXT			40

//абстрактный класс элемента списка
class CUserControlListElement
{
public:
	//деструктор
	virtual ~CUserControlListElement();

	//нарисовать элемент
	virtual VOID Draw(HDC hdc,RECT* pRect) PURE;
	//получить высоту элемента
	virtual UINT GetHeight() PURE;
};

//класс списка
class CUserControlList
{
protected:
	//идентификатор элемента управления
	UINT mControlID;
	//окно элемента управления
	HWND mhWindow;

public:

	CUserControlList(UINT ControlID,HWND hWindow);
	virtual ~CUserControlList();
	
	//добавить элемент в список
	VOID AddElement(CUserControlListElement* pElement);
	//получить выделенный элемент списка
	CUserControlListElement* GetSelectedElement();
	//удалить выделенный элемент из списка
	VOID DeleteSelectedElement();
	//очистить список
	VOID Clear();
	//есть ли выделенный элемент
	BOOL IsExistSelection();
	//пуст ли список
	BOOL IsEmpty();
	//начать обновление списка (предотвратить постоянную перерисовку)
	void BeginUpdate();
	//закончить обновление списка
	void EndUpdate();

	//обработать сообщения; TRUE, если обработано
	BOOL ProcessMessages(UINT Msg,WPARAM wParam,LPARAM lParam);
};

#endif
