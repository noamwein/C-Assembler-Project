#include "funcImplem.h"

/*A function that creates a new commandChart type struct.*/
commandChart * newCommChart(){
	commandChart * commChart = calloc(1, sizeof(commandChart));
	commChart->size = 0;
	commChart->chart = calloc(commChart->size, sizeof(command*));
	return commChart;
}

/*A function that creates a new commandChart type struct.*/
command * newCommand(char * name, char ** param, int codeIndex, int amountOfParameters){
	int i;
	command * comm = malloc(sizeof(command));
	comm->numOfParams = amountOfParameters;
	comm->param = calloc(amountOfParameters, sizeof(char*));
	/*Copies the parameters.*/
	for(i = 0; i < amountOfParameters; i++){
		comm->param[i] = calloc(strlen(param[i])+1, sizeof(char));
		strcpy(comm->param[i], param[i]);
	}
	whichType(comm, name);
	comm->codeIndex = codeIndex;
	return comm;
}

/*A function that determines whether or not a string is a valid address.*/
int isValidAddress(char * add){
	int i;
	/*Checks for the dollar sign.*/
	if(add[0] != '$')
		return 0;
	/*Checks that the rest of the string is a number.*/
	for(i = 1; i < strlen(add); i++){
		if(!isdigit(add[i]))
			return 0;
	}
	return 1;
}

/*A function that returns the amount of parameters a command needs to get.*/
int paramNumForFunc(char * func){
	int i;
	/*Defining the names of the commands.*/
	rCommandNameChart rComms = {{"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"}};
	iCommandNameChart iComms = {{"addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "sw", "lh", "sh"}};
	jCommandNameChart jComms = {{"jmp", "la", "call", "stop"}};

	/*Checks the amount of parameters the command requires if it's an R type command.*/
	for(i = 0; i < 8; i++){
		if(!strcmp(func, rComms.commands[i])){
			if(i < 5)
				return 3;
			else
				return 2;
		}
	}

	/*Checks the amount of parameters the command requires if it's an I type command.*/
	for(i = 0; i < 15; i++){
		if(!strcmp(func, iComms.commands[i])){
			return 3;
		}
	}

	/*Checks the amount of parameters the command requires if it's a J type command.*/
	for(i = 0; i < 4; i++){
		if(!strcmp(func, jComms.commands[i])){
			if(i < 3)
				return 1;
			else
				return 0;
		}
	}
	return -1;
}

/*A function that determines whether a command is of type R, I or J, and completes the command to prepare it for the coding.*/
int whichType(command * comm, char * name){
	int i;
	/*Defining the names of the commands.*/
	rCommandNameChart rComms = {{"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"}};
	iCommandNameChart iComms = {{"addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "sw", "lh", "sh"}};
	jCommandNameChart jComms = {{"jmp", "la", "call", "stop"}};

	/*Setting the opcode and funct of the command if it's an R type command and returning 1.*/
	for(i = 0; i < 8; i++){
		if(!strcmp(rComms.commands[i], name)){
			if(i < 5){
				comm->funct = i+1;
				comm->opcode = 0;
			}
			else{
				comm->funct = i - 4;
				comm->opcode = 1;
			}
			return 1;
		}
	}

	/*Setting the opcode of the command if it's an I type command and returning 2.*/
	for(i = 0; i < 15; i++){
		if(!strcmp(iComms.commands[i], name)){
			comm->opcode = i + 10;
			return 2;
		}
	}

	/*Setting the opcode of the command if it's a J type command and returning 3.*/
	for(i = 0; i < 4; i++){
		if(!strcmp(jComms.commands[i], name)){
			if(i < 3)
				comm->opcode = i + 30;
			else
				comm->opcode = 63;
			return 3;
		}
	}
	return 0;
}

/*A function that finishes the coding of R type commands.*/
int RTypeCoder(command * comm, int passNum, int lineNum){
	int i;
	int rs = 0, rt = 0, rd = 0;
	/*If it's the first type of the R command and the first pass saves the rs, rt, and rd accordingly. also checks for errors.*/
	if(passNum == 1 && comm->opcode == 0){
		comm->coded = 0;
		for(i = 0; i < 3; i++)
			if(!isValidAddress(comm->param[i])){
				printf("Error, line %d: one or more of the parameters is an invalid memory address. \n", lineNum);
				return 0;
			}
		rs = (int)strtol(comm->param[0]+1, (char **)NULL, 10);
		rt = (int)strtol(comm->param[1]+1, (char **)NULL, 10);
		rd = (int)strtol(comm->param[2]+1, (char **)NULL, 10);
		if((rs < 0 || rs > 31) || (rt < 0 || rt > 31) || (rd < 0 || rd > 31)){
			printf("Error, line %d: one or more of the parameters is out of range. \n", lineNum);
			return 0;
		}
	}
	/*If it's the second type of the R command and the first pass saves the rs, rt, and rd accordingly. also checks for errors.*/
	if(passNum == 1 && comm->opcode == 1){
		comm->coded = 0;
		for(i = 0; i < 2; i++)
			if(!isValidAddress(comm->param[i])){
				printf("Error, line %d: one or more of the parameters is an invalid memory address. \n", lineNum);
				return 0;
			}
		rs = (int)strtol(comm->param[0]+1, (char **)NULL, 10);
		rd = (int)strtol(comm->param[1]+1, (char **)NULL, 10);
		rt = 0;
		if((rs < 0 || rs > 31) || (rt < 0 || rt > 31)){
			printf("Error, line %d: one or more of the parameters is out of range. \n", lineNum);
			return 0;
		}
	}
	/*Codes the command.*/
	comm->coded = (comm->coded | (comm->funct<<=6));
	comm->funct>>=6;
	comm->coded = (comm->coded | (rd <<= 11));
	comm->coded = (comm->coded | (rt <<= 16));
	comm->coded = (comm->coded | (rs <<= 21));
	comm->coded = (comm->coded | (comm->opcode <<= 26));
	comm->opcode >>= 26;
	return 1;
}

/*A function that finishes the coding of I type commands.*/
int ITypeCoder(command * comm, int passNum, int lineNum, symbolChart * chart){
	int i;
	int immed = 0;
	int rs = 0;
	int rt = 0;
	/*If it's the first or third type of I type commands and the first pass, saves the immed,rs and rt accordingly and codes the command. also checks for errors.*/
	if(passNum == 1 && (comm->opcode < 15 || comm->opcode > 18)){
		comm->coded = 0;
		if(!isValidAddress(comm->param[0]) || !isValidAddress(comm->param[2])){
			printf("Error, line %d: one or more of the parameters is an invalid address. \n", lineNum);
			return 0;
		}
		for(i = 1; comm->param[1][i]; i++){
			if(!isdigit(comm->param[1][i]) || (comm->param[1][0] != '-' && !isdigit(comm->param[1][0]))){
				printf("Error, line %d: non number parameter. \n", lineNum);
				return 0;
			}
		}
		/*Saving the immded, rs and rt and coding the command.*/
		rs = (int)strtol(comm->param[0]+1, (char **)NULL, 10);
		rt = (int)strtol(comm->param[2]+1, (char **)NULL, 10);
		immed = (int)strtol(comm->param[1], (char **)NULL, 10);
		if((rs < 0 || rs > 31) || (rt < 0 || rt > 31) || (immed < -32768 || immed > 32767)){
			printf("Error, line %d: one or more parameters are out of range. \n", lineNum);
			return 0;
		}
		immed = immed & (int)65535;
		comm->coded = (comm->coded | immed);
		comm->coded = (comm->coded | (rt <<= 16));
		comm->coded = (comm->coded | (rs <<= 21));
		comm->coded = (comm->coded | (comm->opcode <<= 26));
		comm->opcode >>= 26;
	}
	/*If it's the second type of I type commands and the first pass, saves the rs and rt accordingly and codes what it can out of the command. Also checks fr errors.*/
	else if(passNum == 1){
		comm->coded = 0;
		if(!isValidAddress(comm->param[0]) || !isValidAddress(comm->param[1])){
			printf("Error, line %d: one or more of the parameters is an invalid address. \n", lineNum);
			return 0;
		}
		rs = (int)strtol(comm->param[0]+1, (char **)NULL, 10);
		rt = (int)strtol(comm->param[1]+1, (char **)NULL, 10);
		if((rs < 0 || rs > 31) || (rt < 0 || rt > 31)){
			printf("Error, line %d: one or more parameters are out of range. \n", lineNum);
			return 0;
		}
		comm->coded = (comm->coded | (rt <<= 16));
		comm->coded = (comm->coded | (rs <<= 21));
		comm->coded = (comm->coded | (comm->opcode <<= 26));
		comm->opcode >>= 26;
	}
	/*If it's the second type of I type commands and the second pass it finishes the coding and checks for errors.*/
	else if(passNum == 2 && comm->opcode < 19 && comm->opcode > 14){
		if(doesSymbolExist(comm->param[2], chart)==-1){
			printf("Error, line %d: symbol [%s] doesn't exist in symbol chart. \n", lineNum, comm->param[2]);
			return 0;
		}
		else if(isSymbolExtern(doesSymbolExist(comm->param[2], chart), chart)){
			printf("Error, line %d: symbol [%s] was defined as \"external\"", lineNum, comm->param[2]);
		}
		immed  = chart->chart[doesSymbolExist(comm->param[2], chart)]->memoryIndex - comm->codeIndex;
		immed = immed & (int)65535;
		comm->coded = (comm->coded | immed);
	}
	return 1;
}

/*A function that finishes the coding of J type commands.*/
int JTypeCoder(command * comm, int passNum, int lineNum, symbolChart * chart, externList * extList){
	int temp = 0;
	/*If it's the first pass, codes what it can according to the command. Also checks for errors.*/
	if(passNum == 1){
		comm->coded = 0;
		comm->coded = ((comm->coded) | (comm->opcode<<=26));
		comm->opcode>>=26;
		if(comm->opcode == 30){
			if(isValidAddress(comm->param[0])){
				temp = (int)strtol(comm->param[0]+1, (char **)NULL, 10);
				if(temp > 31 || temp < 0){
					printf("Error, line %d: parameter is an invalid address. \n", lineNum);
					return 0;
				}
				comm->coded = (comm->coded | 1<<25);
				comm->coded = (comm->coded | temp);
			}
		}
	}
	/*If it's the second pass finishes the coding of the command according to the symbol chart. Also checks for errors.*/
	else{
		if(comm->opcode == 63)
			return 1;
		else if((comm->opcode == 30 && ((comm->coded) & 1) == 0) || comm->opcode == 31 || comm->opcode == 32){
			if(doesSymbolExist(comm->param[0], chart) != -1){
				if(isSymbolExtern(doesSymbolExist(comm->param[0], chart), chart))
					addExternal(newExternal(comm->param[0], comm->codeIndex), extList);
				temp = chart->chart[doesSymbolExist(comm->param[0], chart)]->memoryIndex;
			}
			else{
				printf("Error, line %d: symbol \"%s\" doesn't exist in symbol chart. \n", lineNum, comm->param[0]);
				return 0;
			}
			comm->coded = (comm->coded | temp);
		}
	}
	return 1;
}

/*A function that adds a command type struct to the array of the commandChart type struct.*/
int addCommand(command * comm, commandChart * chart){
	chart->size += 1;
	chart->chart = realloc(chart->chart, (chart->size * sizeof(command*)));
	chart->chart[chart->size-1] = comm;
	return 0;
}

/*A function that frees the commandChart type struct.*/
int freeCommChart(commandChart * chart){
	int i;
	/*Freeing the commands in the array.*/
	for(i = 0; i < chart->size; i++){
		freeParam(chart->chart[i]->param, chart->chart[i]->numOfParams);
		free(chart->chart[i]->param);
		free(chart->chart[i]);
	}
	free(chart->chart);
	free(chart);
	return 0;
}

/*A function that creates a new dataChart type struct.*/
dataChart * newDataChart(){
	dataChart * chart = calloc(1, sizeof(dataChart));
	chart->size = 0;
	chart->chart = calloc(chart->size, sizeof(data*));
	return chart;
}

/*A function that creates a new data type struct.*/
data * newData(char * name, char ** param, int dataIndex, int amountOfParameters){
	int i;
	data * dat = calloc(1, sizeof(data));
	dat->name = calloc(strlen(name)+1, sizeof(char));
	strcpy(dat->name,name);
	dat->param = calloc(amountOfParameters, sizeof(char*));
	/*Copies the parameters.*/
	for(i = 0; i < amountOfParameters; i++){
		dat->param[i] = calloc(strlen(param[i])+1, sizeof(char));
		strcpy(dat->param[i], param[i]);
	}
	dat->numOfParam = amountOfParameters;
	dat->coded = calloc(0, 0);
	dat->dataIndex = dataIndex;
	return dat;
}

/*A function that codes a data instruction according to the parameters and it's type.*/
int dataCoder(data * dat, int paramNum, int lineNum){
	int i, j;
	double temp;
	/*Coding the data if the instruction is ".asciz", also checks for errors.*/
	if(!strcmp(dat->name, ".asciz")){
		/*Checking for errors.*/
		if(paramNum > 1){
			printf("Error, line %d: extraneous text after end of line. \n", lineNum);
			return -1;
		}
		else if(paramNum < 1){
			printf("Error, line %d: missing parameters. \n", lineNum);
			return -1;
		}
		if(dat->param[0][0] != '\"' || dat->param[0][strlen(dat->param[0])-1] != '\"'){
			printf("Error, line %d: missing quotation mark. \n", lineNum);
			return -1;
		}
		for(i = 1; i < strlen(dat->param[0])-1; i++){
			if(dat->param[0][i] == '\"'){
				printf("Error, line %d: illegal parentheses. \n", lineNum);
				return -1;
			}
		}
		/*Coding the instruction.*/
		dat->coded = (char*)realloc(dat->coded ,(strlen(dat->param[0])-1) * sizeof(char));
		dat->bytes = 1;
		for(i = 1; i < strlen(dat->param[0])-1; i++){
			((char*)dat->coded)[i-1] = dat->param[0][i];
		}
		dat->numOfElements = strlen(dat->param[0])-1;
		((char*)dat->coded)[strlen(dat->param[0])-2] = '\0';
	}
	/*Coding the data instruction if it is not ".asciz". Also checking for errors.*/
	else{
		/*Checks for errors and creates the coding array if the instruction is ".db". Also checks for errors.*/
		if(!strcmp(dat->name, ".db")){
			dat->coded = (char*)realloc(dat->coded, paramNum * sizeof(char));
			dat->bytes = 1;
			dat->numOfElements = paramNum;
			for(i = 0; i < paramNum; i++){
				for(j = 1; dat->param[i][j]!='\0'; j++){
					if(!isdigit(dat->param[i][j]) || (dat->param[i][0] != '-' && !isdigit(dat->param[i][0]))){
						printf("Error, line %d: one or more of the parameters is an invalid number. \n", lineNum);
						return -1;
					}
				}
				temp = (double)strtol(dat->param[i], (char **)NULL, 10);
				if(temp < -128 || temp > 127){
					printf("Error, line %d: one or more of the parameters is out of range. \n", lineNum);
					return -1;
				}
				((char*)dat->coded)[i] = (char)temp;
			}
		}
		/*Checks for errors and creates the coding array if the instruction is ".dh". Also checks for errors.*/
		else if(!strcmp(dat->name, ".dh")){
			dat->coded = (short*)realloc(dat->coded, paramNum * sizeof(short));
			dat->bytes = 2;
			dat->numOfElements = paramNum;
			for(i = 0; i < paramNum; i++){
				for(j = 1; dat->param[i][j]!='\0'; j++){
					if(!isdigit(dat->param[i][j]) || (dat->param[i][0] != '-' && !isdigit(dat->param[i][0]))){
						printf("Error, line %d: one or more of the parameters is an invalid number. \n", lineNum);
						return -1;
					}
				}
				temp = (double)strtol(dat->param[i], (char **)NULL, 10);
				if(temp < -32768 || temp > 32767){
					printf("Error, line %d: one or more of the parameters is out of range. \n", lineNum);
					return -1;
				}
				((short*)dat->coded)[i] = (short)temp;
			}
		}
		/*Checks for errors and creates the coding array if the instruction is ".dw". Also checks for errors.*/
		else if(!strcmp(dat->name, ".dw")){
			dat->coded = (long*)realloc(dat->coded, paramNum * sizeof(long));
			dat->bytes = 4;
			dat->numOfElements = paramNum;
			for(i = 0; i < paramNum; i++){
				for(j = 1; dat->param[i][j]!='\0'; j++){
					if(!isdigit(dat->param[i][j]) || (dat->param[i][0] != '-' && !isdigit(dat->param[i][0]))){
						printf("Error, line %d: one or more of the parameters is an invalid number. \n", lineNum);
						return -1;
					}
				}
				temp = strtod(dat->param[i], (char **)NULL);
				if(temp < INT_MIN || temp > INT_MAX){
					printf("Error, line %d: one or more of the parameters is out of range. \n", lineNum);
					return -1;
				}
				((int*)dat->coded)[i] = (int)temp;
			}
		}
	}
	return (dat->numOfElements * (int)dat->bytes);
}

/*A function that adds a data type struct to that array of a dataChart type struct.*/
int addData(data * dat, dataChart * chart){
	chart->size++;
	chart->chart = realloc(chart->chart, chart->size * sizeof(data));
	chart->chart[chart->size-1] = dat;
	return 1;
}

/*A function that frees the dataChart type struct.*/
int freeDataChart(dataChart * chart){
	int i;
	/*Freeing all of the data instructions from the chart.*/
	for(i = 0; i < chart->size; i++){
		free(chart->chart[i]->coded);
		free(chart->chart[i]->name);
		freeParam(chart->chart[i]->param, chart->chart[i]->numOfParam);
		free(chart->chart[i]->param);
		free(chart->chart[i]);
	}
	free(chart->chart);
	free(chart);
	return 1;
}

