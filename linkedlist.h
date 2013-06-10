#ifndef QUYSE_LINKEDLIST_H
#define QUYSE_LINKEDLIST_H

#include "windows.h"

class LINKEDLIST;

class LLITEM
{
public:
	LINKEDLIST* mllList;
	LLITEM *mlliNext;
	LLITEM *mlliPrev;
	LPVOID mpData;

	LLITEM();
	~LLITEM();
	void AddToList(LINKEDLIST*);
	void RemoveFromList();
};

class LINKEDLIST
{
public:
	LLITEM *lliFirst;
	LLITEM *lliLast;

	LINKEDLIST();
};

#endif
