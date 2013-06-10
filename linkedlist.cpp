#include "linkedlist.h"

//class LLITEM

LLITEM::LLITEM()
{
	mpData=NULL;
	mllList=NULL;
	mlliNext=NULL;
	mlliPrev=NULL;
}

LLITEM::~LLITEM()
{
	RemoveFromList();
}

void LLITEM::AddToList(LINKEDLIST* list)
{
	mllList=list;

	if(mllList->lliLast)
	{
		mlliPrev=mllList->lliLast;
		mllList->lliLast->mlliNext=this;
	}
	else mllList->lliFirst=this;
	mllList->lliLast=this;
}

void LLITEM::RemoveFromList()
{
	if(!mllList) return;

	if(mlliNext) mlliNext->mlliPrev=mlliPrev;
	else mllList->lliLast=mlliPrev;
	if(mlliPrev) mlliPrev->mlliNext=mlliNext;
	else mllList->lliFirst=mlliNext;
}

//class LINKEDLIST

LINKEDLIST::LINKEDLIST()
{
	lliFirst=NULL;
	lliLast=NULL;
}
