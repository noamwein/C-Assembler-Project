#include "passes.h"

/*The function of the first pass of the assembler.*/
int firstPass(char * fileName){
	FILE * file = fopen(fileName, "r");
	int DC = 0, IC = 100;
	int addDC = 0;
	int i;
	/*Creating all of the data structurs that will hold all of the charts.*/
	symbolChart * symChart = newSymbolChart();
	commandChart * commChart = newCommChart();
	dataChart * datChart = newDataChart();
	externList * extList = newExternList();
	char ch = 0;
	int lineNum = 1;
	int isOutError = 0;
	printf("Starting to process file: %s\n", fileName);
	while(ch != EOF) {
		/*Defining flags for the iteration.*/
		int dataFlag = 0;
		int entryFlag = 0;
		int externFlag = 0;
		int commFlag = 0;
		int symFlag = 0;
		int usedParam = 0;
		int isError = 0;

		/*Preparing the variables for the symbol.*/
		int * symLabelI;
		int labelSize = 3;
		char * label = calloc(labelSize, sizeof(char));

		/*Preparing the chars for the function array and parameter array.*/
		int fSize = 4;
		int * funcI;
		char * func = calloc(fSize, sizeof(char));
		int pSize = 4;
		char * tempParam = calloc(pSize, sizeof(char));
		int paramSize = 3;
		char ** param = calloc(paramSize, sizeof(char*));

		/*Preparing the data and command lines.*/
		data* dat;
		command * comm;

		/*Saving the next line from the file.*/
		int size = 4;
		char * line = calloc(size, sizeof(char));
		i = 0;
		while((ch = fgetc(file)) != EOF){
			if(ch == '\n')
				break;
			if(i == size){
				size++;
				line = realloc(line, size*sizeof(char));
			}
			line[i] = ch;
			i++;
		}
		line = realloc(line, (size+1) * sizeof(char));
		line[size] = '\0';

		/*Checking if the line is empty or a comment.*/
		if(ignoreLine(line)){
			isError = 2;
		}

		/*Checking if there's a symbol or an error related to a symbol.*/
		symLabelI = isSymbol(line, lineNum);
		if(symLabelI[0] == -1){
			isError = 1;
		}
		else if(symLabelI[1]!=-2){
			symFlag = 1;
			labelSize = symLabelI[1]-symLabelI[0]+1;
			label = realloc(label, labelSize+1);
			strncpy(label, line + symLabelI[0], labelSize);
			label[labelSize] = '\0';
			if(!isLabelLegal(label, lineNum)){
				isError = 1;
			}
		}

		/*Seperating the function array and the parameter array.*/
		if(symFlag){
			funcI = sepFunc(line, symLabelI[1]+2);
		}
		else{
			funcI = sepFunc(line, 0);
		}
		fSize = funcI[1]-funcI[0] + 1;
		func = realloc(func, fSize+1);
		strncpy(func, line + funcI[0], fSize);
		func[fSize] = '\0';

		pSize = strlen(line) - funcI[1] + 1;
		tempParam = realloc(tempParam, pSize+1);
		strncpy(tempParam, line + funcI[1] + 1, pSize);
		tempParam[pSize] = '\0';

		/*Checking what type the function is, or if it's an unknown function.*/
		dataFlag = isData(func);
		entryFlag = isEntry(func);
		externFlag = isExtern(func);
		commFlag = isComm(func);

		if(!isError && !dataFlag && !entryFlag && !externFlag && !commFlag){
			isError = 1;
			printf("Error, line %d: unknown command [%s]. \n", lineNum, func);
		}

		/*Checking if there are too many or too little paramters,
		 * or if there is extraneous text after the end of the command.*/
		if(!isError && (entryFlag || externFlag)){
			if(howManyParam(tempParam) < 1){
				isError = 1;
				printf("Error, line %d: missing parameters. \n", lineNum);
			}
			else if(howManyParam(tempParam) > 1){
				isError = 1;
				printf("Error, line %d: extraneous text after end of command. \n", lineNum);
			}
		}
		else if(!isError && commFlag){
			if(howManyParam(tempParam) < paramNumForFunc(func)){
				isError = 1;
				printf("Error, line %d: missing parameters. \n", lineNum);
			}
			else if(howManyParam(tempParam) > paramNumForFunc(func)){
				isError = 1;
				printf("Error, line %d: extraneous text after end of command. \n", lineNum);
			}
		}

		/*Checking if the parameters are spaced as required, if they are than it separates the parameters.*/
		if(!isError && !isProperlySpaced(tempParam, lineNum)){
			isError = 1;
		}

		if(isError != 1 && isError != 2){
			param = sepParam(tempParam, param);
			usedParam = 1;
		}

		/*If it is a data line, codes it and adds a symbol with the attribute data if there is a symbol.*/
		if(dataFlag && !isError){
			if(symFlag)
				if(!addSymbol(symChart, newSymbol(label, DC, "data"), lineNum)){
					isError = 1;
				}
			dat = newData(func, param, DC, howManyParam(tempParam));
			addData(dat, datChart);
			addDC = dataCoder(dat, howManyParam(tempParam), lineNum);
			if(addDC == -1){
				isError = 1;
			}
			else
				DC+=addDC;
			if(!isError)
				isError = 2;
		}

		/*If it is a .entry line, turns on the flag that says to skip to the next line.*/
		if(entryFlag && !isError)
			isError = 2;

		/*If it is a .extern line, creates a symbol and adds the symbol to the symbol chart as required.*/
		if(externFlag && !isError){
			if(doesSymbolExist(param[0], symChart)!=-1){
				if(!isSymbolExtern(doesSymbolExist(param[0], symChart), symChart)){
					isError = 1;
					printf("Error, line %d: incorrectly defined symbol. \n", lineNum);
				}
			}
			else{
				if(isLabelLegal(param[0], lineNum)){
					addSymbol(symChart, newSymbol(param[0], 0, "external"), lineNum);
				}
				else{
					isError = 1;
				}
			}
			if(!isError)
				isError = 2;
		}

		/*If it is a command line, determines what type of command it is,
		 *  codes it and adds a symbol with the command attribute if there is a lbel*/
		if(commFlag && !isError){
			if(symFlag)
				if(!addSymbol(symChart, newSymbol(label, IC, "code"), lineNum)){
					isError = 1;
				}
			comm = newCommand(func, param, IC, howManyParam(tempParam));
			addCommand(comm, commChart);
			if(whichType(comm, func) == 1){
				if(!RTypeCoder(comm, 1, lineNum))
					isError = 1;
			}
			else if(whichType(comm, func) == 2){
				if(!ITypeCoder(comm, 1, lineNum, symChart))
					isError = 1;
			}
			else if(whichType(comm, func) == 3){
				if(!JTypeCoder(comm, 1, lineNum, symChart, extList))
					isError = 1;
			}
			if(!isError){
				isError = 2;
				IC+=4;
			}
		}

		lineNum++;

		/*Freeing all of the allocated memory that has been use in this iteration.*/
		free(label);
		free(line);
		free(func);
		if(usedParam)
			freeParam(param, howManyParam(tempParam));
		free(param);
		free(tempParam);
		free(funcI);
		free(symLabelI);
		if(isError == 1){
			isOutError = 1;
		}
	}
	fclose(file);

	/*Checking if there was an error in the file.*/
	if(!isOutError){
		addIcToData(IC, symChart);
		for(i = 0; i < datChart->size; i++){
			datChart->chart[i]->dataIndex+=IC;
		}
		isOutError = secondPass(fileName, symChart, commChart, datChart, extList);
	}
	if(!isOutError){
		filer(IC, DC, fileName, extList, commChart, datChart, symChart);
		printf("Processing of file %s finished successfully\n", fileName);
	}else{
		printf("Failed to process file: %s.\n", fileName);
	}
	freeExternList(extList);
	freeSymChart(symChart);
	freeCommChart(commChart);
	freeDataChart(datChart);
	return 0;
}

/*The function of the second pass of the assembler.*/
int secondPass(char * fileName, symbolChart * symChart, commandChart * commChart, dataChart * datChart, externList * extList){
	FILE * file = fopen(fileName, "r");
	int i = 0;
	int commIndex = 0;
	char ch = 0;
	int lineNum = 1;
	int isOutError = 0;
	while(ch != EOF) {
		/*Defining flags for the iteration.*/
		int entryFlag = 0;
		int commFlag = 0;
		int symFlag = 0;
		int isError = 0;

		/*Preparing the variables for the symbol.*/
		int * symLabelI;

		/*Preparing the chars for the function array and parameter array.*/
		int fSize = 4;
		int * funcI;
		char * func = calloc(fSize, sizeof(char));
		int pSize = 4;
		char * tempParam = calloc(pSize, sizeof(char));
		int paramSize = 3;
		char ** param = calloc(paramSize, sizeof(char*));

		/*Saving the next line from the file.*/
		int size = 4;
		char * line = calloc(size, sizeof(char));
		i = 0;
		while((ch = fgetc(file)) != EOF){
			if(ch == '\n')
				break;
			if(i == size){
				size++;
				line = realloc(line, size*sizeof(char));
			}
			line[i] = ch;
			i++;
		}
		line = realloc(line, (size+1) * sizeof(char));
		line[size] = '\0';

		/*Checking if the line is empty.*/
		if(ignoreLine(line)){
			isError =  2;
		}

		/*Checking if there's a symbol or an error related to a symbol.*/
		symLabelI = isSymbol(line, lineNum);
		if(symLabelI[0] == -1){
			isError = 1;
		}
		else if(symLabelI[1] != -2){
			symFlag = 1;
		}

		/*Separating the function and parameters array.*/
		if(symFlag){
			funcI = sepFunc(line, symLabelI[1]+2);
		}
		else{
			funcI = sepFunc(line, 0);
		}

		fSize = funcI[1]-funcI[0] + 1;
		func = realloc(func, fSize+1);
		strncpy(func, line + funcI[0], fSize);
		func[fSize] = '\0';
		pSize = strlen(line) - funcI[1] + 1;
		tempParam = realloc(tempParam, pSize+1);
		strncpy(tempParam, line + funcI[1] + 1, pSize);
		tempParam[pSize] = '\0';

		if(!isError)
			param = sepParam(tempParam, param);

		/*Checking the type of the function.*/
		entryFlag = isEntry(func);
		commFlag = isComm(func);

		if(!entryFlag && !commFlag){
			isError = 2;
		}

		/*If the function is a .entry, than adds the tag entry to the symbol.*/
		if(entryFlag && !isError){
			if(!doesSymbolExist(param[0], symChart)){
				printf("Error, line %d: symbol defined as \"entry\" doesn't exist.\n", lineNum);
				isError = 1;
			}else{
				if(!addTag((symChart->chart[doesSymbolExist(param[0], symChart)]), "entry", lineNum)){
					isError = 1;
				}
			}
		}

		/*If the function is a command then completes the coding of the command line usinf the symbol chart.*/
		if(commFlag && !isError){
			if(whichType(commChart->chart[commIndex], func) == 1){
				if(!RTypeCoder(commChart->chart[commIndex], 2, lineNum))
					isError = 1;
				}
			else if(whichType(commChart->chart[commIndex], func) == 2){
				if(!ITypeCoder(commChart->chart[commIndex], 2, lineNum, symChart))
					isError = 1;
			}
			else if(whichType(commChart->chart[commIndex], func) == 3){
				if(!JTypeCoder(commChart->chart[commIndex], 2, lineNum, symChart, extList))
					isError = 1;
			}
			if(!isError){
				isError = 2;
			}
			commIndex++;
		}
		if(isError == 1)
			isOutError = 1;
		lineNum++;

		/*Freeing all of the data structures that have been used during this iteration.*/
		free(line);
		free(func);
		freeParam(param, howManyParam(tempParam));
		free(param);
		free(tempParam);
		free(funcI);
		free(symLabelI);
	}
	fclose(file);
	if(isOutError)
		return 1;
	else
		return 0;
}

/*The function that organizes all of the coded commands, symbols and instructions in proper files.*/
int filer(int ICF, int DCF, char * fileName, externList * extList, commandChart * commChart, dataChart * datChart, symbolChart * symChart){
	FILE * mainfptr;
	FILE * extfptr;
	FILE * entfptr;
	/*Defining three different variables so that we can iterate through different levels of the charts.*/
	int i = 0;
	int j = 0;
	int k = 0;
	/*Defining other variables that we will use.*/
	int byteCounter = 0;
	char byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0;
	long temp = 0;
	int commandIndex = 100;
	char line[40] = "";
	char newFileName[20];

	/*Opening the .ob file and adding the first line with the data sizes.*/
	fileName = strtok(fileName, ".");
	sprintf(newFileName, "%s.ob", fileName);
	mainfptr = fopen(newFileName, "w");
	sprintf(line, "\t %02d %02d\n", ICF-100, DCF);
	fputs(line, mainfptr);

	/*Iterating through the command array and breaking them to four bytes, and adding them to the .ob file.*/
	for(i = 0; i < commChart->size; i++){
		byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0;
		temp = commChart->chart[i]->coded;
		byte1 = (byte1|temp);
		temp >>= 8;
		byte2 = (byte2|temp);
		temp >>= 8;
		byte3 = (byte3|temp);
		temp >>= 8;
		byte4 = (byte4|temp);
		sprintf(line, "%04d %02X %02X %02X %02X\n", commandIndex, byte1 & 0xff, byte2 & 0xff, byte3 & 0xff, byte4 & 0xff);
		fputs(line, mainfptr);
		commandIndex += 4;
	}

	/*Iterating through the data array, which has three levels:
	 *1. Each line, which we will use the i variable for.
	 *2. Each member within the line, which we will use the j variable for.
	 *3. Each byte within each individual data structure, which k will represent the offset of.*/
	i = 0, j = 0, k = 0;
	byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0;
	byteCounter = 0;
	while(i < datChart->size){
		/*If every member is a single byte.*/
		if(datChart->chart[i]->bytes == 1){
			while(j < datChart->chart[i]->numOfElements && byteCounter < 4){
				if(byteCounter == 0){
					byte1 = ((char*)(datChart->chart[i]->coded))[j];
					byteCounter++;
				}
				else if(byteCounter == 1){
					byte2 = ((char*)(datChart->chart[i]->coded))[j];
					byteCounter++;
				}
				else if(byteCounter == 2){
					byte3 = ((char*)(datChart->chart[i]->coded))[j];
					byteCounter++;
				}
				else if(byteCounter == 3){
					byte4 = ((char*)(datChart->chart[i]->coded))[j];
					byteCounter++;
				}
				j++;
			}
		}
		/*If every individual member is the size of 2 bytes,
		 * keeps track of the amount of bytes needed to complete the line and the offset within each member,
		 * represented by k.*/
		else if(datChart->chart[i]->bytes == 2){
			while(j < datChart->chart[i]->numOfElements && byteCounter < 4){
				if(byteCounter == 0){
					if(k == 0){
						byte1 = byte1 | ((short*)(datChart->chart[i]->coded))[j];
						((short*)(datChart->chart[i]->coded))[j] >>= 8;
						byte2 = byte2 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter = 2;
					}
					else if(k == 1){
						byte1 = byte1 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter++;
						k = 0;
					}
					j++;
				}
				else if(byteCounter == 1){
					if(k == 0){
						byte2 = byte2 | ((short*)(datChart->chart[i]->coded))[j];
						((short*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = byte3 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter += 2;
					}
					else if(k == 1){
						byte2 = byte2 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter++;
						k = 0;
					}
					j++;
				}
				else if(byteCounter == 2){
					if(k == 0){
						byte3 = byte3 | ((short*)(datChart->chart[i]->coded))[j];
						((short*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = byte4 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter += 2;
					}
					else if(k == 1){
						byte3 = byte3 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter++;
						k = 0;
					}
					j++;
				}
				else if(byteCounter == 3){
					if(k == 0){
						byte4 = byte4 | ((short*)(datChart->chart[i]->coded))[j];
						((short*)(datChart->chart[i]->coded))[j] >>= 8;
						byteCounter++;
						k = 1;
					}
					else if(k == 1){
						byte4 = byte4 | ((short*)(datChart->chart[i]->coded))[j];
						byteCounter++;
						k = 0;
						j++;
					}
				}
			}
		}
		/*If every individual member is the size of 4 bytes,
		 * keeps track of the amount of bytes needed to complete the line and the offset within each member,
		 * represented by k.*/
		else if(datChart->chart[i]->bytes == 4){
			while(j < datChart->chart[i]->numOfElements && byteCounter < 4){
				if(byteCounter == 0){
					if(k == 0){
						byte1 = (byte1|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter += 4;
					}
					else if(k == 1){
						byte1 = (byte1|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter += 3;
						k = 0;
					}
					else if(k == 2){
						byte1 = (byte1|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter += 2;
						k = 0;
					}
					else{
						byte1 = (byte1|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter++;
						k = 0;
					}
					j++;
				}
				else if(byteCounter == 1){
					if(k == 0){
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byteCounter += 3;
						k = 3;
					}
					else if(k == 1){
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter += 3;
						k = 0;
						j++;
					}
					else if(k == 2){
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter+=2;
						k = 0;
						j++;
					}
					else{
						byte2 = (byte2|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter++;
						k = 0;
						j++;
					}
				}
				else if(byteCounter == 2){
					if(k == 0){
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byteCounter += 2;
						k = 2;
					}
					else if(k == 1){
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byteCounter += 2;
						k = 3;
					}
					else if(k == 2){
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter += 2;
						k = 0;
						j++;
					}
					else{
						byte3 = (byte3|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter++;
						k = 0;
						j++;
					}
				}
				else if(byteCounter == 3){
					if(k != 3){
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						((int*)(datChart->chart[i]->coded))[j] >>= 8;
						byteCounter++;
						k++;
					}
					else{
						byte4 = (byte4|((int*)(datChart->chart[i]->coded))[j]);
						byteCounter++;
						k = 0;
						j++;
					}
				}
			}
		}

		/*If the line is completed, meaning that 4 consecutive bytes from the data arrays were saved,
		 * we put them in the proper file according to the instructions with the proper index.*/
		if(byteCounter == 4){
			sprintf(line, "%04d %02X %02X %02X %02X\n", commandIndex, byte1 & 0xff, byte2 & 0xff, byte3 & 0xff, byte4 & 0xff);
			fputs(line, mainfptr);
			commandIndex+=4;
			/*resets for the next four bytes.*/
			byteCounter = 0;
			byte1 = 0, byte2 = 0, byte3 = 0, byte4 = 0;
			/*Checks if we reached the end of an array.*/
			if(j == datChart->chart[i]->numOfElements){
				i++;
				j = 0;
			}
			continue;
		}
		/*Checks if we reached the end of an array.*/
		else if(j == datChart->chart[i]->numOfElements){
			i++;
			j = 0;
			continue;
		}
	}

	/*Checks if there are bytes remaining, if there are puts them in the proper file according to the instructions.*/
	if(byteCounter != 0){
		commandIndex+=4;
		if(byteCounter == 3)
			sprintf(line, "%04d %02X %02X %02X\n", commandIndex, byte1 & 0xff, byte2 & 0xff, byte3 & 0xf);
		else if(byteCounter == 2)
			sprintf(line, "%04d %02X %02X\n", commandIndex, byte1 & 0xff, byte2 & 0xff);
		else if(byteCounter == 1){
			sprintf(line, "%04d %02X\n", commandIndex, byte1 & 0xff);
		}
		fputs(line, mainfptr);
	}

	/*Opens the .ext file and puts the used external symbols with the IC of it in the file according to the instructions. */
	sprintf(newFileName, "%s.ext", fileName);
	extfptr = fopen(newFileName, "w");

	for(i = 0; i < extList->size; i++){
		sprintf(line, "%s %04d\n", extList->chart[i]->label, extList->chart[i]->commandIndex);
		fputs(line, extfptr);
	}

	/*Opens the .ent file and puts the used entry symbols with the memory location where they were defined in the file according to the instructions. */
	sprintf(newFileName, "%s.ent", fileName);
	entfptr = fopen(newFileName, "w");

	for(i = 0; i < symChart->symNum; i++){
		if(!strcmp(symChart->chart[i]->tags[1], "entry")){
			sprintf(line, "%s %04d\n", symChart->chart[i]->name, symChart->chart[i]->memoryIndex);
			fputs(line, entfptr);
		}
	}

	/*Closing the files.*/
	fclose(mainfptr);
	fclose(extfptr);
	fclose(entfptr);
	return 0;
}
