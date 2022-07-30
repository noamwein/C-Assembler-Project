#ifndef SYMBOL
#define SYMBOL

#include "parsing.h"

/*A struct that represents a symbol.*/
typedef struct{
	int memoryIndex;
	char **tags;
	char * name;
} symbol;

/*A struct that holds an array of symbol type structs.*/
typedef struct {
	int symNum;
	symbol ** chart;
} symbolChart;

/*A function that creates a new symbolChart type struct.*/
symbolChart * newSymbolChart();

/*A function that adds a symbol type struct to the array in the symbolChart type struct.*/
int addSymbol(symbolChart * chart, symbol * sym, int lineNum);

/*A function that creates a new symbol type struct.*/
symbol * newSymbol(char * name, int memoryIndex, char * tag);

/*A function that adds a tag to a symbol.*/
int addTag(symbol * sym, char * tag, int lineNum);

/*A function that frees a symbolChart type struct.*/
int freeSymChart(symbolChart * sym);

/*A function that checks if there is a symbol with a certain label in the symbolChart array.*/
int doesSymbolExist(char * label, symbolChart * chart);

/*A function that checks if a string is a legal symbol label.*/
int isLabelLegal(char * label, int lineNum);

/*A function that checks if a symbol in an array is of type "extern".*/
int isSymbolExtern(int index, symbolChart * chart);

/*A function that adds the current IC to the memoryIndex of all the symbol structs in an array.*/
int addIcToData(int IC, symbolChart * chart);

#endif

