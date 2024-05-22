assembler: assembler.o preAssembler.o firstPass.o utils.o convert.o secondPass.o macroLists.o
	gcc -g preAssembler.o assembler.o firstPass.o utils.o convert.o secondPass.o macroLists.o -o assembler

assembler.o: assembler.c preAssembler.c firstPass.c convert.c secondPass.c data.h macroLists.h macrosHandler.h utils.h firstPass.h secondPass.h convert.h
	gcc -c -Wall -pedantic -ansi assembler.c -o assembler.o

preAssembler.o: preAssembler.c macroLists.c data.h macroLists.h macrosHandler.h
	gcc -c -Wall -pedantic -ansi preAssembler.c -o preAssembler.o  

macroLists.o: macroLists.c data.h macroLists.h 
	gcc -c -Wall -pedantic -ansi macroLists.c -o macroLists.o 

firstPass.o: firstPass.c utils.c data.h utils.h firstPass.h
	gcc -c -Wall -pedantic -ansi firstPass.c -o firstPass.o 

utils.o: utils.c data.h macroLists.h macrosHandler.h utils.h
	gcc -c -Wall -pedantic -ansi utils.c -o utils.o 

convert.o: convert.c data.h convert.h utils.h
	gcc -c -Wall -pedantic -ansi convert.c -o convert.o 

secondPass.o: secondPass.c data.h secondPass.h
	gcc -c -Wall -pedantic -ansi secondPass.c -o secondPass.o 
