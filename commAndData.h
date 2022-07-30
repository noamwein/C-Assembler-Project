#ifndef DATA
#define DATA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/*A struct which holds an array of all the names of the R type commands.*/
typedef struct{
	char * commands[8];
}rCommandNameChart;

/*A struct which holds an array of all the names of the I type commands.*/
typedef struct {
	char * commands[16];
}iCommandNameChart;

/*A struct which holds an array of all the names of the J type commands.*/
typedef struct {
	char * commands[4];
}jCommandNameChart;

/*A struct which holds two arrays, the first of all the names of the data type instructions,
 *  the second holds all of the names of the data type instructions with the dot at the start.*/
typedef struct {
	char * directives[4];
	char * dottedDirectives[4];
}dataNameChart;

#endif
