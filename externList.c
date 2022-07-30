#include "externList.h"

/*Creates a new ExternList type struct.*/
externList * newExternList(){
	externList * list = calloc(1, sizeof(externList));
	list->size = 0;
	list->chart = calloc(list->size, sizeof(external*));
	return list;
}

/*Creates a new external type struct.*/
external * newExternal(char * label, int commandIndex){
	external * ext = calloc(1, sizeof(external));
	ext->commandIndex = commandIndex;
	ext->label = calloc(strlen(label)+1, sizeof(char));
	strcpy(ext->label,label);
	ext->label[strlen(label)] = '\0';
	return ext;
}

/*Adds an external type struct to the array in the externList type struct.*/
int addExternal(external * ext, externList * list){
	list->size++;
	list->chart = realloc(list->chart, list->size * sizeof(external*));
	list->chart[list->size-1] = ext;
	return 0;
}

/*Free's the externList type struct.*/
int freeExternList(externList * list){
	int i;
	/*Freeing all of the external type structs in the array.*/
	for(i = 0; i < list->size; i++){
		free(list->chart[i]->label);
		free(list->chart[i]);
	}
	free(list->chart);
	free(list);
	return 0;
}
