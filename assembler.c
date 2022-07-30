/* Made by: Noam Weingarten.
 * ID: 328238928. */
#include "passes.h"

int main(int argc, char * argv[]){
	int i;
	FILE * fptr;
	/*Opening, assembling and closing the files that have been passed as parameters.*/
	for(i = 1; i<argc; i++){
		if(argv[i][strlen(argv[i])-1]=='s' && argv[i][strlen(argv[i])-2] == 'a' && argv[i][strlen(argv[i])-3] == '.'){
			fptr = fopen(argv[i], "r");
			if(fptr){
				fclose(fptr);
				firstPass(argv[i]);
			}
			else{
				printf("No file with name \"%s\" found.\n", argv[i]);
			}
		}
		else{
			printf("File \"%s\" is not a .as file.\n", argv[i]);
		}
	}
	return 0;
}

