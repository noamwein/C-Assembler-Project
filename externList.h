#pragma once
#ifndef EXTERNLIST
#define EXTERLIST

#include "commAndData.h"

/*A struct which holds the label of a symbol, and the index where it was used.*/
typedef struct{
	char * label;
	int commandIndex;
} external;

/*A struct which holds an array of the struct above.*/
typedef struct{
	int size;
	external ** chart;
} externList;

/*Creates a new ExternList type struct.*/
externList * newExternList();

/*Creates a new external type struct.*/
external * newExternal(char * label, int commandIndex);

/*Adds an external type struct to the array in the externList type struct.*/
int addExternal(external * ext, externList * list);

/*Free's the externList type struct.*/
int freeExternList(externList * list);

#endif
