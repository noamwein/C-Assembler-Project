#ifndef FUNC
#define FUNC

#include "parsing.h"
#include "symbolChart.h"
#include "externList.h"

/*A struct which represents a command.*/
typedef struct{
	int opcode;
	int funct;
	int codeIndex;
	int numOfParams;
	long coded;
	char ** param;
}command;

/*A struct that holds an array of command type structs.*/
typedef struct{
	int size;
	command ** chart;
}commandChart;

/*A struct which represents a data type instruction.*/
typedef struct{
	char * name;
	char ** param;
	int dataIndex;
	int numOfElements;
	int numOfParam;
	char bytes;
	void * coded;
}data;

/*A struct that holds an array of data type structs.*/
typedef struct{
	int size;
	data ** chart;
}dataChart;

/*A function that creates a new commandChart type struct.*/
commandChart * newCommChart();

/*A function that creates a new command type struct.*/
command * newCommand(char * name, char ** param, int codeIndex, int amountOfParameters);

/*A function that determines whether a command is of type R, I or J, and completes the command to prepare it for the coding.*/
int whichType(command * comm, char * name);

/*A function that returns the amount of parameters a command needs to get.*/
int paramNumForFunc(char * func);

/*A function that determines whether or not a string is a valid address.*/
int isValidAddress(char * add);

/*A function that finishes the coding of R type commands.*/
int RTypeCoder(command * comm, int passNum, int lineNum);

/*A function that finishes the coding of I type commands.*/
int ITypeCoder(command * comm, int passNum, int lineNum, symbolChart * chart);

/*A function that finishes the coding of J type commands.*/
int JTypeCoder(command * comm, int passNum, int lineNum, symbolChart * chart, externList * extList);

/*A function that adds a command type struct to the array of the commandChart type struct.*/
int addCommand(command * comm, commandChart * chart);

/*A function that frees the commandChart type struct.*/
int freeCommChart(commandChart * chart);

/*A function that creates a new dataChart type struct.*/
dataChart * newDataChart();

/*A function that creates a new data type struct.*/
data * newData(char * name, char ** param, int dataIndex, int amountOfParam);

/*A function that codes a data instruction according to the parameters and it's type.*/
int dataCoder(data * dat, int paramNum, int lineNum);

/*A function that adds a data type struct to that array of a dataChart type struct.*/
int addData(data * dat, dataChart * chart);

/*A function that frees the dataChart type struct.*/
int freeDataChart(dataChart * chart);

#endif
