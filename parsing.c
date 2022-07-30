#include "parsing.h"

/*A function that checks if the line is empty or a comment.*/
int ignoreLine(char * line){
	int i;
	/*Iterating through the line and checking if it is a comment or an empty line*/
	for(i=0; line[i] != '\0'; i++){
		if(line[i] == ';')
			return 1;
		else if(line[i] != ';' && !isspace(line[i]))
			return 0;
	}
	return 1;
}

/*A function that checks if there's a symbol label, and returns the start and end index of the label in the line.*/
int * isSymbol(char * line, int lineNum){
	char ch;
	int i;
	int didStart = 0;
	int startIndex = 0;
	int endedFirstWord = 0;
	int endIndex = 0;
	int parenFlag = 0;
	int * ret = calloc(2, sizeof(int));
	ret[0] = -2;
	ret[1] = -2;
	/*Iterating through the line string.*/
	for(i = 0; i < strlen(line); i++){
		ch = line[i];
		/*Checking for parentheses.*/
		if(ch == '\"')
			parenFlag = !parenFlag;
		/*Checking if we have not started reading the first word in the line, and setting flags and returning accordingly.*/
		if(!didStart){
			if(isspace(ch))
				continue;
			else{
				/*Checking if the first character is a ":".*/
				if(ch == ':'){
					printf("Error, line %d: symbol label can't be empty.\n", lineNum);
					ret[0] = -1;
					ret[1] = -1;
					return ret;
				}
				didStart = 1;
				startIndex = i;
				continue;
			}
		}
		/*If we did start reading the first word.*/
		else{
			if(!endedFirstWord && isspace(ch)){
				endedFirstWord = 1;
			}
			/*Checking for errors or if the symbol was defined.*/
			else if(!endedFirstWord && ch == ':'){
				if(!isspace(line[i+1])){
					printf("Error, line %d: missing space after label definition.\n", lineNum);
					ret[0] = -1;
					ret[1] = -1;
					return ret;
				}
				endIndex = i-1;
				ret[0] = startIndex;
				ret[1] = endIndex;
				return ret;
			}
			/*Checking for a space during the label definition.*/
			else if(endedFirstWord && ch == ':' && !parenFlag){
				printf("Error, line %d: illegal space during symbol label definition.\n", lineNum);
				ret[0] = -1;
				ret[1] = -1;
				return ret;
			}
		}
	}
	ret[0] = -2;
	ret[1] = -2;
	return ret;
}

/*A function that returns the start and end indexes of the function.*/
int * sepFunc(char * line, int startIndex){
	int i, j;
	int * funcI = calloc(2, sizeof(int));
	for(i = startIndex; isspace(line[i]); i++);
	for(j = i; !isspace(line[j]) && line[j]!='\0'; j++);
	funcI[0] = i;
	funcI[1] = j-1;
	return funcI;
}

/*A function that checks how many parameters there are in a string.*/
int howManyParam(char * param){
	int i = 0;
	int paramCount = 0;
	int isInParam = 0;
	int parenFlag = 0;
	/*Iterating through the string.*/
	for(i = 0; param[i]; i++){
		if(param[i] == '\"')
			parenFlag = !parenFlag;
		if(!isspace(param[i]) && param[i] != ',' && !isInParam){
			isInParam = 1;
			paramCount++;
		}
		else if(isInParam && (isspace(param[i]) || param[i] == ',') && !parenFlag){
			isInParam = 0;
		}
	}
	return paramCount;
}

/*A function that separates the parameters in a string to separate strings.*/
char ** sepParam(char * params, char ** param){
	int i = 0, j = 0, k = 0;
	int paramCounter = 0;
	int apostroFlag = 0;
	int tempSize = 5;
	char * temp = calloc(tempSize, sizeof(char));
	param = realloc(param, howManyParam(params) * sizeof(char*));
	/*Iterating through the params array.*/
	for(i = 0; i < strlen(params) && k < howManyParam(params); i++){
		/*Skipping unneeded spaces.*/
		if(isspace(params[i]) && !apostroFlag && i < strlen(params)-1){
			continue;
		}
		if(j >= tempSize){
			tempSize++;
			temp = realloc(temp, tempSize * sizeof(char));
		}
		/*if we reach the next parameter than sets the next paramter of the param string array to be the last parameter and preparing for the next iteration.*/
		if((params[i] == ',' && !apostroFlag)||i == strlen(params)-1){
			if(i == strlen(params)-1 && !isspace(params[i])){
				temp[j] = params[i];
				j++;
			}
			if(j >= tempSize){
				tempSize++;
				temp = realloc(temp, tempSize * sizeof(char));
			}
			temp[j] = '\0';
			param[k] = calloc(strlen(temp)+1, sizeof(char));
			strcpy(param[k], temp);
			k++;
			j=0;
			free(temp);
			tempSize = 5;
			temp = calloc(tempSize, sizeof(char));
			paramCounter++;
		}
		else{
			temp[j] = params[i];
			j++;
		}
		/*Checking for apostrophes.*/
		if(!apostroFlag && params[i] == '\"')
			apostroFlag = 1;
		else if(apostroFlag && params[i] == '\"')
			apostroFlag  = 0;
	}
	free(temp);
	return param;
}

/*A function that frees all of the separate parameter strings.*/
int freeParam(char ** param, int howManyParam){
	int i;
	for(i = 0; i < howManyParam; i++){
		free(param[i]);
	}
	return 0;
}

/*A function that checks if a string is the name of a data type instruction.*/
int isData(char * func){
	int i;
	dataNameChart dChart = {{"db", "dh", "dw", "asciz"}, {".db", ".dh", ".dw", ".asciz"}};
	/*Checking if the function is a data type word.*/
	for(i = 0; i < 4; i++){
		if(!strcmp(func, dChart.dottedDirectives[i]))
			return 1;
	}
	return 0;
}

/*A function that checks if a string is an entry instruction.*/
int isEntry(char * func){
	if(!strcmp(func, ".entry"))
		return 1;
	return 0;
}

/*A function that checks if a string is an entry instruction.*/
int isExtern(char * func){
	if(!strcmp(func, ".extern"))
		return 1;
return 0;
}

/*A function that checks if a string is a command.*/
int isComm(char * func){
	int i;
	rCommandNameChart rChart = {{"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"}};
	iCommandNameChart iChart = {{"addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "sw", "lh", "sh"}};
	jCommandNameChart jChart = {{"jmp", "la", "call", "stop"}};

	/*Checking if the string is the name of an R type command.*/
	for(i = 0; i < 8; i++){
		if(!strcmp(func, rChart.commands[i]))
			return 1;
	}

	/*Checking if the string is the name of an I type command.*/
	for(i = 0; i < 16; i++){
		if(!strcmp(func, iChart.commands[i]))
			return 1;
	}

	/*Checking if the string is the name of an J type command.*/
	for(i = 0; i < 4; i++){
		if(!strcmp(func, jChart.commands[i]))
			return 1;
	}
	return 0;
}

/*A function that checks if a string is a saved word.*/
int isSavedWord(char * word){
	int i;
	rCommandNameChart rComms = {{"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"}};
	iCommandNameChart iComms = {{"addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "sw", "lh", "sh"}};
	jCommandNameChart jComms = {{"jmp", "la", "call", "stop"}};
	dataNameChart dChart = {{"db", "dh", "dw", "asciz"}, {".db", ".dh", ".dw", "asciz"}};

	/*Checking if the word is entry or extern.*/
	if(!strcmp(word, "entry") || !strcmp(word, "extern") || !strcmp(word, ".entry") || !strcmp(word, ".extern"))
		return 1;

	/*Checking if the word is a command.*/
	for(i = 0; i < 16; i++){
		if(!strcmp(word, iComms.commands[i]))
			return 1;
	}
	for(i = 0; i < 4; i++){
		if(!strcmp(word, jComms.commands[i]))
			return 1;
	}
	for(i = 0; i < 8; i++){
		if(!strcmp(word, rComms.commands[i]))
			return 1;
	}

	/*Checking if the word is a data type word.*/
	for(i = 0; i < 4; i++){
		if(!strcmp(word, dChart.directives[i]))
			return 1;
	}
	for(i = 0; i < 4; i++){
		if(!strcmp(word, dChart.dottedDirectives[i]))
			return 1;
	}

	return 0;
}

/*A function that checks if a string is properly spaced according to the instructions.*/
int isProperlySpaced(char * parameterArr, int lineNum){
	int i;
	int apostroFlag = 0;
	int paramSize = 0;
	int commaFlag = 0;
	int paramFlag = 0;
	for(paramSize = 0; parameterArr[paramSize] != '\0'; paramSize++);

	/*Checking for the required space between the function and parameters.*/
	if(!isspace(parameterArr[0]) && paramSize!=0){
		printf("Error, line %d: missing space between the function declaration and the parameters.\n", lineNum);
		return 0;
	}
	/*Checking for two or more consecutive commas.*/
	for(i=0; i<paramSize; i++){
		if(commaFlag && parameterArr[i]==','){
			printf("Error, line %d: two or more consecutive commas.\n", lineNum);
			return 0;
		}
		if(!apostroFlag && parameterArr[i]==',')
			commaFlag = 1;
		if(!isspace(parameterArr[i]) && parameterArr[i] != ',')
			commaFlag = 0;
		if(!apostroFlag && parameterArr[i]=='\"')
			apostroFlag = 1;
		else if(apostroFlag && parameterArr[i]=='\"')
			apostroFlag = 0;
	}

	/*Checking if the parameters end in a comma.*/
	if(commaFlag){
		printf("Error, line %d: extraneous text after end of command.\n", lineNum);
		return 0;
	}
	/*Checking for missing commas.*/
	commaFlag = 1;
	apostroFlag = 0;
	for(i=0; i<paramSize; i++){
		if(!apostroFlag && parameterArr[i]==','){
			commaFlag = 1;
			paramFlag = 0;
		}
		else if(!apostroFlag && commaFlag && !paramFlag && !isspace(parameterArr[i]) && parameterArr[i] != ','){
			commaFlag = 0;
			paramFlag = 1;
		}
		else if(!apostroFlag && isspace(parameterArr[i])){
			paramFlag = 0;
		}
		else if(!commaFlag && !paramFlag){
			printf("Error, line %d: missing commas.\n", lineNum);
			return 0;
		}
		if(!apostroFlag && parameterArr[i]=='\"')
			apostroFlag = 1;
		else if(apostroFlag && parameterArr[i]=='\"')
			apostroFlag = 0;
	}
	return 1;
}
