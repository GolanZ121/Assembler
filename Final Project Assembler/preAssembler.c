#include "data.h"
#include "macroLists.h"
#include "macrosHandler.h"

/* Global pointer to the head of the macro list */
macroPointer globalList;


int listFiller(macroPointer* listPtr, char* inputFile) {
    FILE* input = fopen(inputFile, "r"); 
    char* token, *temp1; 
    char line[ROW_LENGTH+1], strLine[ROW_LENGTH]; 
    int initFlag = 0; /* Variable that represents the number of current macros that are being initialized */
    int i = 0, rowNum = 0, isGood = 1,  dataRows = 1; 
    /* "i" is the current row number, "rowNum" is the number of the row that the macro is being initialized on */
    /* "isGood" indicates error or "errorless" file */ 
    /* "dataRows" the number of rows inside the macros data (used for reallocation of dynamic memory)*/
    char macroName[ROW_LENGTH]; 
    char* macroData, *q;
    macroPointer newNode;
     
    

    temp1 = (char*)(calloc(ROW_LENGTH, sizeof(char)));
    if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	macroData = temp1;
    while (fgets(line, ROW_LENGTH+2, input) != NULL) {
        i++; 
        /* Check if row length is over the limit (80) */
        if(strlen(line) > ROW_LENGTH)
            continue;
        
        strncpy(strLine, line, ROW_LENGTH); /* Copy the current line to a separate array */
        token = strtok(strLine, " \t\n"); 
		
        /* Check if the line contains "mcro", indicating the start of a macro */
        if (token != NULL && strcmp(token, "mcro") == 0) 
        {
            if (initFlag)
            {
                fprintf(stderr, "\t\t-In line %d error:\n\t\t  Nested macros not allowed!\n", i);
                isGood = 0;
                initFlag++; /* The initFlag should be larger then 1 because multiple macros are being initialized */
                continue;
            }
            
            rowNum = i; /* Record the row number where the macro starts */
            initFlag = 1; /* No nested macros indicated (only 1) */
            token = strtok(NULL, " \t\n"); /* Get the next token, which should be the macro name */

            if (token != NULL) 
                strncpy(macroName, token, ROW_LENGTH); /* Copy the macro name to the macroName array */
            else 
            {
                fprintf(stderr, "\t\t-In line %d error:\n\t\t  Macro without a name not allowed!\n", i);
                isGood = 0; /* Error found (will be used to stop further processing of the file) */
                continue; 
            }
            continue; 
        }
        
        /* If the current line is inside a macro (initFlag is true), add its content to macroData */
        if (token != NULL && initFlag == 1 && strcmp(token, "endmcro") != 0) 
        {
            dataRows++; /* Another row inside the macro discovered (increase number of rows)*/
            strcat(macroData, line);
            
            /* Check if memory allocation was successful*/
            q = (char*)(realloc(macroData, dataRows*ROW_LENGTH));
            if (!q)
            {
                fprintf(stderr, "\t\t-In line %d error:\n\t\t memory allocation didn't carried out\n", i);
                exit(0);
            }
            macroData = q;
            continue;

        }

        /* Check if the line contains the "endmcro", indicating the end of a macro */
        if (token != NULL && strcmp(token, "endmcro") == 0 && initFlag > 0) 
        {
            if (initFlag == 1)
            {
                /* Inside ONE valid macro initFlag = 1 */
                initFlag = 0; /* "endmcro" found (macro initializing ended) */
                
                newNode = createNode(macroName, macroData, rowNum, dataRows); 
                
                /* Add the new node to the macro list and check for errors */
                if (addNodeToList(listPtr, newNode) == 0) 
                {
                    fprintf(stderr, "\t\t-In line %d error:\n\t\t  Two or more Macros with the same name!\n", i);
                    isGood = 0; /* Error found (will be used to stop further processing of the file) */
                    continue; 
                } else if (addNodeToList(listPtr, newNode) == -1) 
                {
                    fprintf(stderr, "\t\t-In line %d error:\n\t\t  Macro has the name of Instruction / Guidance\n", i);
                    isGood = 0; /* Error found (will be used to stop further processing of the file) */
                    continue; 
                }
                memset(macroName, 0, sizeof(macroName));  /* Clear the macroName array for the next macro */
                /* macroData = NULL; */  /* Clear the macroData array for the next macro */
                continue; 
            }
            /* initFlag > 1, (inside nested macros) */
            else
                initFlag--; /* Reached the end of one of the nested macros */
            
            continue; 
        }

        /* If "endmcro" is found without a preceding "mcro" */
        if (token != NULL && strcmp(token, "endmcro") == 0 && initFlag == 0) 
        {
            fprintf(stderr, "\t\t-In line %d error:\n\t\t  \"endmcro\" typed without macro being initialized! \n", i);
            isGood = 0;
            continue;
        }
    }

    fclose(input); 
    return isGood; /* return 0 if Error found, else 1 (will be used to indicate whether to continue processing the file) */
}




int outputFill(char* inputFile, char* outputFile, macroPointer list) {
    FILE* input = fopen(inputFile, "r"); 
    FILE* output = fopen(outputFile, "w"); 
    char* token; 
    char line[ROW_LENGTH+1], strLine[ROW_LENGTH];
    macroPointer currNode; 
    int initFlag = 0, i = 0, isGood = 1; 
    /* Variable that represents the number of current macros that are being initialized */
    /* "i" is the current row number */
    /* "isGood" indicates error or "errorless" file */ 

    while (fgets(line, ROW_LENGTH+2, input) != NULL) 
    {
        i++; 
        if(strlen(line) > ROW_LENGTH)
            continue;
        
        strncpy(strLine, line, ROW_LENGTH); /* Copy the current line to a separate array */
         
        token = strtok(line, " \t\n");

        /* If the line is empty or contains only whitespaces, write it directly to output */
        if (token == NULL) 
        {
            fputs(strLine, output);
            continue;
        }

        currNode = list;  /* Set the current node to the head of the macro list */
        while (currNode != NULL) 
        {
            /* If a macro name matches the token, replace it with its data in output */
            if (strcmp(token, currNode->mcroName) == 0) {
                if (currNode->row < i)
                {
                    fputs(currNode->data, output); /* Write the macro data to the output file */
                }
                else 
                {
                    fprintf(stderr, "\t\t-In line %d error:\n\t\t  macro used before initialized!\n", i);
                    isGood = 0; /* Set isGood to 0 to indicate an error */
                    break;
                }
                break; 
            }
            currNode = currNode->next; 
        }

        /* If "mcro" is found, set the initFlag flag to true */
        if (strcmp(token, "mcro") == 0) {
            initFlag = 1;
            token = strtok(NULL, " \t\n"); 
            continue;
        }
        
        /* If inside a macro (initFlag is true) and not reached "endmcro", continue processing */
        if (initFlag == 1 && strcmp(token, "endmcro") != 0) 
            continue; 
        
        /* If "endmcro" is found and currently inside a macro, set the initialization flag to false */
        if (strcmp(token, "endmcro") == 0 && initFlag == 1) 
            initFlag = 0;

        /* If "endmcro" is found, continue to the next line */
        if (strcmp(token, "endmcro") == 0)
            continue;

        /* If the token doesn't match any macro name, write the original line to the output file */
        if (currNode == NULL) 
            fputs(strLine, output);
    }

    fclose(input); 
    fclose(output); 
    return isGood; /* return 0 if Error found, else 1 (will be used to indicate whether to continue processing the file) */
}




int fileHandler(char* inputFile, char* outputFile) {
    FILE* input = fopen(inputFile, "r"); 
    FILE* output = fopen(outputFile, "w"); 
    macroPointer macroList = createNode("\0", "\0", 0, 0); /* "Null" head node */
    int isGood = 1; 
	
    /* Check if input and output files were opened successfully */
    if (input == NULL || output == NULL) {
        fprintf(stderr, "\tFailed to open files for processing.\n");
        return 0; 
    }

    /* Fill the macro list from the input file using listFiller function */
    if (!listFiller(&macroList, inputFile))
        isGood = 0;  /* Error found (will be used to stop further processing of the file) */
	
    globalList = macroList->next;  /* Set the global macro list pointer to the actual head of the list */
    
	
/* Write the output after macros spread to the output file using outputFill function */
    if (!outputFill(inputFile, outputFile, macroList))
        isGood = 0;  /* Error found (will be used to stop further processing of the file) */
    
    free(macroList);
    fclose(input); 
    fclose(output); 
    return isGood; /* return 0 if Error found, else 1 (will be used to indicate whether to continue processing the file) */
}

