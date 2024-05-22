#include "data.h"
#include "macroLists.h"
#include "macrosHandler.h"
#include "utils.h"
#include "firstPass.h"
#include "secondPass.h"
#include "convert.h" 

extern int memory[];
extern int dataArray[];
extern ptr p;
extern macroPointer globalList;

int main(int argc, char* argv[]) {
	char  line[ROW_LENGTH+1]={'\0'},cpy[ROW_LENGTH+1]={'\0'}, *inputFile, *outputFile;
	int IC = INIT_MEM_POS ,DC = 0, valid = 0 , flag = 1, i = 0, index = 0, isEntry = 0, isExtern = 0;
	char* temp1, *temp2;
	/*DC points to the next empty place in dataArray.
	IC points to the next empty place in memory.
	valid is a sub flag that gets if a line is valid
	flag is a flag that marks eventually if a file is valid after first pass
	isEntry and isExtern marks if there is an entry and extern declerations in the file*/
	FILE *file;
	
    if (argc < 2) /*if no file name was given*/
        return 1;
    for (i = 1; i < argc; i++) {/*goes over each file name given in the command line*/
    	/*initialize all the variables in each iteration of the loop*/
    	
    	
    	if(i > 1){/*free all the lists and allocated memories for string after the last file.
					in addition, resets all variables after the last file*/
			p = NULL;
			inputFile = NULL;
			outputFile = NULL;
			globalList = NULL;
			freeSymbolList(p);
			p = NULL;
			freeList(globalList);
			globalList = NULL;
			free(inputFile);
			free(outputFile);
			index = 0; IC = INIT_MEM_POS; DC = 0; flag = 1; isEntry = 0; isExtern = 0, valid =0;
			memset(line,0,sizeof(line));
			memset(cpy,0,sizeof(cpy));
			memset(memory,0,MAX_FILE_LENGTH);
			memset(dataArray,0,MAX_FILE_LENGTH);
    	
    	}
    	
    	printf("\n");
		temp1 = (char*)(calloc(strlen(argv[i]) + strlen(".as") , sizeof(char)));
        temp2 = (char*)(calloc(strlen(argv[i]) + strlen(".am"), sizeof(char)));
        if (!temp1 || !temp2) {
			fprintf(stderr, "memory allocation failure\n");
			exit(0);
		}
		inputFile = temp1;
		outputFile = temp2;
       
		printf("Currently compiling file : %s.as (File# %d/%d)\n\n", argv[i], i, argc-1);
		printf("\t-Starting Pre-Processor on %s.as\n\n", argv[i]);
		strcpy(inputFile, argv[i]);
        inputFile[strlen(inputFile)] = '\0';
        strcat(inputFile, ".as");
		
		if(!(file = fopen(inputFile,"r"))){
			fprintf(stderr, "\t Cannot open file: %s\n", inputFile);
			continue;
		}
        strcpy(outputFile, argv[i]);
        outputFile[strlen(outputFile)] = '\0';
        strcat(outputFile, ".am");
        
        if(!fileHandler(inputFile, outputFile)){

             continue;
  			}
        if(!(file = fopen(outputFile,"r"))){
			fprintf(stderr, "\tCannot open file");
			continue;
		}
		printf("\t  == Pre-Processor has finished successfully! ==\n\n");
		printf("\t-Starting First-Pass on %s\n\n", outputFile);

		while(fgets(line, ROW_LENGTH+1, file)){/*starts going over every line the outputFile*/
			index++;/*the index of the line being handled*/
			
			if(strlen(line) > ROW_LENGTH || line==NULL){ /*if the line is null(file is empty) or the line is bigger than 80 chars */
				fprintf(stderr, "\tIn line %d error:\n\t row length is over 80 characters\n", index);
				flag = 0;/*error - we mark 0*/
				continue;/*continue to the next line*/
			}

			strcpy(cpy,line);/*copies the line to cpy*/
			valid = sentence(cpy, &IC,&DC, index);/*sends cpy to sentence to proccess it and if the line is valid valid = 1, otherwise 0*/
			
			if(valid == 0)/*if the line sent isn't valid*/
				flag = 0;/*mark the flag as 0*/

		}
		
		if(flag)/*if the file is valid after first pass*/
			update(IC);/*updates the values of labels defined in a guide instruction*/
		else{/*the given file isn't valid */
			
			continue;/*continue to the next file*/
			}
			
		
		printf("\n\t  == First-Pass has finished successfully! ==\n\n");
		printf("\t-Starting Second-Pass on %s\n\n", outputFile);
		
		rewind(file);
		if(!fillMem(file, &isExtern, &isEntry))/*if the file didn't pass the second pass*/
			continue;/*continue to the next file*/
		
		
		if(memory[INIT_MEM_POS] || dataArray[0])/*if there is data to encode(in memory or dataArray)*/
			converter(argv[i], DC);/*create the object file*/
			
		rewind(file);
		if(isExtern)/*if there is an extern declaration*/
			ExternalsFileWriter(file, argv[i]);/*create the ext file*/
		
		rewind(file);
        if(isEntry)/*if there is an entry declarations*/
			EntriesFileWriter(file, argv[i], isEntry);/*create an ent file*/

		printf("\t  == Second-Pass has finished successfully! ==\n\n");
		printf("File %s.as was successfully compiled!\tIC = %d, DC = %d\n\n", argv[i],IC-INIT_MEM_POS,DC);
		rewind(file);
		
    	
	}
	
   	if(i == 1){/*if i == 1 - meaning there was only one file and all the variables and lists still exists so we free them. free all the lists and allocated memories for string after the last file.
				in addition, resets all variables after the last file*/
		p = NULL;
		inputFile = NULL;
		outputFile = NULL;
		globalList = NULL;
		freeSymbolList(p);
		p = NULL;
		freeList(globalList);
		globalList = NULL;
		free(inputFile);
		free(outputFile);
		index = 0; IC = INIT_MEM_POS; DC = 0; flag = 1; isEntry = 0; isExtern = 0, valid =0;
		memset(line,0,sizeof(line));
		memset(cpy,0,sizeof(cpy));
		memset(memory,0,MAX_FILE_LENGTH);
		memset(dataArray,0,MAX_FILE_LENGTH);
		
    }
    
    if(file != NULL)
		fclose(file);
	
	return 0;
}
