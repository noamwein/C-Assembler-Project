#ifndef PARSE
#define PARSE

#include "commAndData.h"

/*A function that checks if the line is empty or a comment.*/
int ignoreLine(char * line);

/*A function that checks if there's a symbol label, and returns the start and end index of the label in the line.*/
int * isSymbol(char * line, int lineNum);

/*A function that returns the start and end indexes of the function.*/
int * sepFunc(char * line, int startIndex);

/*A function that checks how many parameters there are in a string.*/
int howManyParam(char * param);

/*A function that separates the parameters in a string to separate strings.*/
char ** sepParam(char * params, char ** param);

/*A function that frees all of the separate parameter strings.*/
int freeParam(char ** param, int howManyParam);

/*A function that checks if a string is the name of a data type instruction.*/
int isData(char * func);

/*A function that checks if a string is an entry instruction.*/
int isEntry(char * func);

/*A function that checks if a string is a command.*/
int isComm(char * func);

/*A function that checks if a string is an extern instruction.*/
int isExtern(char * func);

/*A function that checks if a string is a saved word.*/
int isSavedWord(char * word);

/*A function that checks if a string is properly spaced according to the instructions.*/
int isProperlySpaced(char * parameterArr, int lineNum);
#endif
