#ifndef PASS
#define PASS

#include "symbolChart.h"
#include "funcImplem.h"
#include "externList.h"

/*The function of the first pass of the assembler.*/
int firstPass(char * fileName);

/*The function of the second pass of the assembler.*/
int secondPass(char * fileName, symbolChart * symChart, commandChart * commChart, dataChart * datChart, externList * extList);

/*The function that organizes all of the coded commands, symbols and instructions in proper files.*/
int filer(int ICF, int DCF, char * fileName, externList * extList, commandChart * commChart, dataChart * datChart, symbolChart * symChart);
#endif
