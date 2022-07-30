assembler: assembler.o passes.o symbolChart.o funcImplem.o parsing.o externList.o
	gcc -ansi -Wall -pedantic -g assembler.o passes.o symbolChart.o funcImplem.o parsing.o externList.o -o assembler
	
assembler.o:assembler.c passes.o
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
	
passes.o: passes.c passes.h symbolChart.o funcImplem.o
	gcc -c -ansi -Wall -pedantic passes.c -o passes.o
	
symbolChart.o: symbolChart.h symbolChart.c parsing.o
	gcc -c -ansi -Wall -pedantic symbolChart.c -o symbolChart.o

funcImplem.o: funcImplem.h funcImplem.c parsing.o symbolChart.o
	gcc -c -ansi -Wall -pedantic funcImplem.c -o funcImplem.o
	
parsing.o: parsing.h parsing.c commAndData.o
	gcc -c -ansi -Wall -pedantic parsing.c -o parsing.o
	
externList.o: externList.h externList.c commAndData.o
	gcc -c -ansi -Wall -pedantic externList.c -o externList.o
	
commAndData.o: commAndData.h
	gcc -c -ansi -Wall -pedantic commAndData.h -o commAndData.o
	
clean:
	rm *.o
	rm assembler