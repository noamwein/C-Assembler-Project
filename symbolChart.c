#include "symbolChart.h"

/*A function that creates a new symbolChart type struct.*/
symbolChart * newSymbolChart(){
	symbolChart * ptr = calloc(1, sizeof(symbolChart));
	ptr->chart = calloc(0, sizeof(symbol*));
	ptr->symNum = 0;
	return ptr;
}

/*A function that adds a symbol type struct to the array in the symbolChart type struct.*/
int addSymbol(symbolChart * chart, symbol * sym, int lineNum){
	int i;
	/*Checking if the symbol already exists in the chart.*/
	for(i = 0; i < chart->symNum; i++){
		if(!strcmp(chart->chart[i]->name, sym->name)){
			printf("Error, line %d: symbol [%s] already exists.\n", lineNum, sym->name);
			free(sym->name);
			free(sym->tags);
			free(sym);
			return 0;
		}
	}
	chart->symNum++;
	chart->chart = realloc(chart->chart, chart->symNum * sizeof(symbol*));
	chart->chart[chart->symNum-1] = sym;
	return 1;
}

/*A function that creates a new symbol type struct.*/
symbol * newSymbol(char * name, int memoryIndex, char * tag){
	/*Allocating the required memory.*/
	symbol * sym = calloc(1, sizeof(symbol));
	sym->memoryIndex = memoryIndex;
	sym->tags = calloc(2, sizeof(char*));
	sym->name = calloc(strlen(name)+1, sizeof(char));
	/*Setting the tags of the new symbol.*/
	sym->tags[0] = tag;
	sym->tags[1] = "";
	/*Setting the name of the new symbol.*/
	strcpy(sym->name, name);
	sym->name[strlen(sym->name)] = '\0';
	return sym;
}

/*A function that adds a tag to a symbol.*/
int addTag(symbol * sym, char * tag, int lineNum){
	/*Checking that there aren't both entry and external tags.*/
	if(!strcmp(tag, "external")){
		if(!strcmp(sym->tags[0], "entry")){
			printf("Error, line %d: Symbol [%s] cannot"
					" be both \"external\" and \"entry\"\n", lineNum, sym->name);
			return 0;
		}
	}
	else if(!strcmp(tag, "entry")){
		if(!strcmp(sym->tags[0], "external")){
			printf("Error, line %d: Symbol [%s] cannot"
					" be both \"external\" and \"entry\"\n", lineNum, sym->name);
			return 0;
		}
	}

	/*Checking if the symbol already has the tag.*/
	if(!strcmp(sym->tags[0], tag)){
		if(strcmp(tag, "external") && strcmp(tag, "entry"))
			printf("Warning: Symbol [%s] already has attribute \"%s\". \n", sym->name, tag);
	}
	sym->tags[1] = tag;
	return 1;
}

/*A function that frees a symbolChart type struct.*/
int freeSymChart(symbolChart * sym){
	int i;
	for(i=0; i < sym->symNum; i++){
		free(sym->chart[i]->name);
		free(sym->chart[i]->tags);
		free(sym->chart[i]);
	}
	free(sym->chart);
	free(sym);
	return 0;
}

/*A function that checks if there is a symbol with a certain label in the symbolChart array.*/
int doesSymbolExist(char * label, symbolChart * chart){
	int i;
	for(i = 0; i < chart->symNum; i++){
		if(!strcmp(chart->chart[i]->name, label)){
			return i;
		}
	}
	return -1;
}

/*A function that checks if a string is a legal symbol label.*/
int isLabelLegal(char * label, int lineNum){
	int i;
	/*Checking for errors.*/
	if(!isalpha(label[0])){
		printf("Error, line %d: symbol label must begin with a letter\n", lineNum);
		return 0;
	}
	for(i = 0; label[i]; i++){
		if(!isalpha(label[i]) && !isdigit(label[i])){
			printf("Error, line %d: symbol label must consist only of letters and digits\n", lineNum);
			return 0;
		}
	}
	if(i >= 31){
		printf("Error, line %d: symbol label must be only 31 characters long\n", lineNum);
		return 0;
	}
	if(isSavedWord(label)){
		printf("Error, line %d: symbol label must not be a saved word\n", lineNum);
		return 0;
	}
	return 1;
}

/*A function that checks if a symbol in an array is of type "extern".*/
int isSymbolExtern(int index, symbolChart * chart){
	int i;
	for(i = 0; i < 2; i++){
		if(!strcmp("external", chart->chart[index]->tags[i]))
			return 1;
	}
	return 0;
}

/*A function that adds the current IC to the memoryIndex of all the symbol structs in an array.*/
int addIcToData(int IC, symbolChart * chart){
	int i, j;
	for(i = 0; i < chart->symNum; i++){
		for(j = 0; j<2; j++){
			if(!strcmp(chart->chart[i]->tags[j], "data")){
				chart->chart[i]->memoryIndex+=IC;
			}
		}
	}
	return 0;
}
