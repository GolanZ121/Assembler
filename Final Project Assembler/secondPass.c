#include "data.h"
#include "utils.h"
#include "secondPass.h"
extern int memory[];
extern ptr p;

const char* guides[5] = {
    ".data", ".string", ".extern",".entry",'\0'
};
const char* registerNames[9] = {
    "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",'\0'
};
const char* actionNames[17] = {/*holds all the names of the instructions*/
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop",'\0'
};


int fillMem(FILE* file, int* ext, int* ent){
    ptr symbols = p;
    char line[ROW_LENGTH], copy[ROW_LENGTH];
    char* token;
    int IC = INIT_MEM_POS, rowNum=0, flag=1, registerFlag = 0;
    
	
    while(fgets(copy, ROW_LENGTH+1, file) != NULL)
    {/*goes on every line in the file */
        rowNum++;/*the line counter*/
		registerFlag = 0;
        if(copy != NULL)
        	strcpy(line, copy);
        else
        	continue;
        token = strtok(line, " ,\n\r\t");/*takes the first word in the line*/
		if(!token)/*if token is null - the line is empty or has only blank chars*/
			continue;
        if(token[0] == ';')/*if its a comment line*/
        	continue;
        else if(token[strlen(token)-1] == ':')/*if the first word is a definition of a label */
        	token = strtok(NULL, " ,\n\r\t");/*continue to the next word*/
        if(strcmp(token, ".entry") == 0)
        {/*if the token is entry */
        	(*ent)++;/*mark there is an entry. counts how many entries are there*/
            token = strtok(NULL, " \n\r\t");/*takes the word declared as entry*/
			
            if(!doesNodeExists(token, rowNum))
            {/*if the word declared as entry isn't defined in the file - error*/
            	fprintf(stderr, "\t\tIn line %d error:\n\t\t  Label set to be \"entry\" but not defined\n", rowNum);
            	flag = 0;
            }
            else if(isExt(token))
            {/*if the word declared as entry is also declared as extern - error*/
            	fprintf(stderr, "\t\tIn line %d error:\n\t\t  Label set to be \"entry\" and  \"extern\"\n", rowNum);
            	flag = 0;	
            }
            continue;/*if the first word in .entry there is nothing to encode and nothing to check in the line and we can continue to the next line*/
            
        }
        else if(!strcmp(token, ".extern"))
        	*ext = 1;/*mark there is an extern*/
        if(in(token, guides))/*if the token is a guide word there is nothing to encode or check in the line and we can continue to the next line. that means 			that all lines we are handling now will be instruction lines*/
            continue;

        IC++;/*promote IC by one for the instrcution word*/
		
        token = strtok(NULL, " ,\n\r\t");/*taking the next word - the first parameter*/
        if(!token)/*if the token is null - the instruction has no param - we can continue*/
            continue;
        /*if a label was defined before a declarartion*/
		symbols = p;
        if(in(token, registerNames))/*if the first parameter is a register*/
        	registerFlag = 1;/*marks that the first parameter is a register*/
      	else if(atoi(token));/*if the first parameter is an integer*/
      	else
      	{/*the parameter is a label*/
			while(symbols != NULL)
			{/*now we go over the table of labels*/
		        if(!strcmp(token, symbols->name))
		        {/*if we found the label in the table*/
		        	memory[IC] = (symbols->type == 'x') ? 1 : (symbols->value<<2)+ 2;/*if its an external label encodes it with the value 1 (as requested), 						otherwise encodes the value of the label in the ten left bits and puts "10" in the first two bits.
		        		it encodes it in IC*/

		        	symbols = p;/*reboot symbols*/
		        	break;
		        }
		        symbols = symbols -> next;/*promotes the node to the next node*/
		    }
		    if(symbols == NULL){/*if symbols is null - symbols has reached to the end of the list in the last while loop - it has found a parameter label which 					hasn't being defined - error*/
		    		fprintf(stderr, "\t\tIn line %d error:\n\t\t  Label is used but not declared\n", rowNum);
		        	flag = 0;
		        	continue;	
		    }
			symbols = p;/*reboot symbols*/
    	}

    	IC++;/*counting the encoding of the first parameter(register,integer,label)*/

    	token = strtok(NULL, " ,\n\r\t");/*taking the second parameter*/
    	
    	if(!token)/*if the token is null - there isn't another parameter - the instruction only gets one parameter - we can continue to the next line*/
        	continue;

        if(in(token, registerNames))
        {/*if the second parameter is a register*/
        	if(!registerFlag)/*if the first and second parameters are registers they are encoded in the same word and we aren't promoting IC. if the second 			if the first parameter isn't a register, every parameter is encoded alone and we promote IC*/
        		IC++;
        }
        else if(atoi(token))/*if the second parameter is an integer*/
        	IC++;
    	else
    	{/*the parameter is a label*/
			while(symbols != NULL)
			{/*now we go over the table of labels*/
		        if(!strcmp(token, symbols->name))
		        {/*if we found the label in the table*/
		        	memory[IC] = (symbols->type == 'x') ? 1 : (symbols->value<<2)+ 2;/*if its an external label encodes it with the value 1 (as requested), 						otherwise encodes the value of the label in the ten left bits and puts "10" in the first two bits.
		        		it encodes it in IC*/

		        	symbols = p;/*reboot symbols*/
		        	break;
		        }
		        symbols = symbols -> next;/*promotes the node to the next node*/
		    }
		    if(symbols == NULL)
		    {/*if symbols is null - symbols has reached to the end of the list in the last while loop - it has found a parameter label which 					hasn't being defined - error*/
		    		fprintf(stderr, "\t\tIn line %d error:\n\t\t  Label is used but not declared\n", rowNum);
		        	flag = 0;
		        	continue;	
		    }
			symbols = p;/*reboot symbols*/
			IC++;
    	}

    	
    }
   
    return flag;/*returns the flag. if there was an error in the line - 0 will be returned. if not, 1 will be returned*/
}

void ExternalsFileWriter(FILE* file, char* Name){
    ptr symbols = p;
    char line[ROW_LENGTH];
    char* token, *fileName, *temp1;
    FILE* output;
    int IC = INIT_MEM_POS, registerFlag = 0;
    temp1 = (char*)calloc(strlen(Name) + strlen(".ext") + 1,sizeof(char));
    if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	fileName = temp1;
    strcat(fileName,Name);
    strcat(fileName,".ext");
	fileName[strlen(fileName)] = '\0';
	
    if(!(output = fopen(fileName,"w")))
    {/*creating the external file */
			fprintf(stderr, "\tcannot open file: %s\n", fileName);
			return;
	}
    while(fgets(line, ROW_LENGTH+1, file))
    {/*goes over every line in the file*/
    	symbols = p;
    	registerFlag = 0;
        token = strtok(line, " ,\n\r\t");/*takes the first word in the line*/
       if(!token)/*if token is null - the line is empty or has only blank chars*/
			continue;
        if(token[0] == ';')/*if its a comment line*/
        	continue;
        if(token[strlen(token)-1] == ':')/*if the first word is a definition of a label */
        	token = strtok(NULL, " ,\n\r\t");/*continue to the next word*/
        if(in(token, guides))/*if the first word is a guide word there is no label used as parameter in an instruction line*/
        	continue;
        
        token = strtok(NULL, " ,\n\r\t");/*takes the next word*/
        IC++;/*the last token was an instruction word if we got here so now we count his encoding*/
        if(!token)/*if token is null - there is nothing to check - continues*/
            continue;
        if(in(token, registerNames))/*if the first parameter is a register*/
        	registerFlag = 1;/*marks that the first parameter is a register*/
      	else if(atoi(token));/*if the first parameter is an integer*/
      	else
      	{/*the parameter is a label*/
	        while(symbols != NULL)
	        {/*while the node hasn't got to the end of the list*/
	            if(isExt(token))
	            {/*if the label is external*/
	                    fprintf(output, "%s\t%d\n", token, IC);/*we write in the external file the label and place of encodement in memory*/
	                    break;
	            }
	            symbols = symbols->next;/*moving over to the next node*/
	        }
    	symbols = p;/*reboot the node*/
    	}
    	IC++;/*promotes IC after passing the first parameter*/
    	token = strtok(NULL, " ,\n\r\t");/*takes the next parameter*/
    	if(!token)/*if token is null - there is nothing else to check*/
        	continue;
        if(in(token, registerNames))
        {/*if the second parameter is a register*/
        	if(!registerFlag)/*if the first and second parameters are registers they are encoded in the same word and we aren't promoting IC. if the second 			if the first parameter isn't a register, every parameter is encoded alone and we promote IC*/
        		IC++;
        }
        else if(atoi(token))/*if the second parameter is an integer*/
        	IC++;
      	else
      	{/*the parameter is a label*/
	        while(symbols != NULL)
	        {/*while the node hasn't got to the end of the list*/
	            if(isExt(token))
	            {/*if the label is external*/
	                    fprintf(output, "%s\t%d\n", token, IC);/*we write in the external file the label and place of encodement in memory*/
	                    break;
	            }
	            symbols = symbols->next;/*moving over to the next node*/
	        }
    	symbols = p;/*reboot the node*/
    	IC++;/*promotes the counting after counting the encodement of the label*/
    	}
    }
    fileName= NULL;
    free(fileName);
    fclose(output);
}

void EntriesFileWriter(FILE* file, char* Name, int numOfEntries){
    char line[ROW_LENGTH];
    char* token, *fileName, *temp1;
    int* alreadyPrinted = (int*)calloc(numOfEntries, sizeof(int)), index = 0, i = 0, flag ;/*alreadyPrinted is holding the address of every entry label alreay been printed. index counts how much have been added to the array. flag marks if thhe label we are working has already been added*/
    ptr symbols = p;
    FILE* output;
    /*if there are several entries of the same label*/
   
    temp1 = (char*)calloc(strlen(Name) + strlen(".ent") + 1,sizeof(char));
	if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	fileName = temp1;
    strcat(fileName,Name);
    strcat(fileName,".ent");
    fileName[strlen(fileName)] = '\0';
    if(!(output = fopen(fileName,"w"))){/*creates the file of entries*/
			fprintf(stderr, "\tcannot open file: %s\n", fileName);
			return;
	}
    while(fgets(line, ROW_LENGTH+1, file))
    {/*goes over every line*/
    	symbols = p;
		flag = 0;
        token = strtok(line, " \n\r\t");/*takes the first word */
        if(!token)/*if the line is empty or only contains blank chars we can continue to the next line*/
        	continue;
        if(token[0] == ';')/*if the line is a comment line we can continue to the next line*/
                continue;

        if(strcmp(token, ".entry") == 0)
        {/*if the first word is entry*/
            token = strtok(NULL, " ,\n\r\t");/*takes the label after .entry*/
        	/*this while loop goes over the table of labels and searches for the label. once it found the node it breaks the loop*/
        	while(symbols != NULL)
        	{
	            if(strcmp(token, symbols->name) == 0)
	            	break;
            	symbols = symbols -> next;
        	}
        	for(i = 0; i <= index;i++){/*goes over the array and checks if the label wer'e working on has already been printed*/
        		if(symbols->value == alreadyPrinted[i])
        		{
        			flag = 1;
        			break;
        		}
        	}
        	if(flag)/*means the label has already been printed and we continue to the next line*/
        		continue;
        	else/*the label hasn't been printed so we write its address in the array*/
        		alreadyPrinted[index++] = symbols->value;
        	/*after the loop symbols will now point on the node of the label therefore giving us access the the value of label*/
            fprintf(output,"%s %d\n",token, symbols->value);/*we write in the entries file the label and its value as requested*/
        }
        else/*if a label was declared before entry*/
       	{
       		
       		token = strtok(NULL, " \n\r\t");

       		if(strcmp(token, ".entry") == 0)
       		{/*if the first word is entry*/
		        token = strtok(NULL, " ,\n\r\t");/*takes the label after .entry*/
		    	/*this while loop goes over the table of labels and searches for the label. once it found the node it breaks the loop*/

		    	if(!token)
		    		continue;
		    	while(symbols != NULL)
		    	{

			        if(strcmp(token, symbols->name) == 0)
			        	break;
		        	symbols = symbols -> next;
		    	}
		    	for(i = 0; i <= index;i++){/*goes over the array and checks if the label wer'e working on has already been printed*/
		    		if(symbols->value == alreadyPrinted[i])
		    		{

		    			flag = 1;
		    			break;
		    		}
		    	}
		    	if(flag)/*means the label has already been printed and we continue to the next line*/
		    		continue;
		    	else/*the label hasn't been printed so we write its address in the array*/
		    		alreadyPrinted[index++] = symbols->value;
		    	/*after the loop symbols will now point on the node of the label therefore giving us access the the value of label*/
		        fprintf(output,"%s %d\n",token, symbols->value);/*we write in the entries file the label and its value as requested*/
		        /*reboots symbols*/
        	}
        
    	}
        /*goes over to the next line*/
    }
    fileName= NULL;
    free(fileName);
    alreadyPrinted = NULL;
    free(alreadyPrinted);
    fclose(output);
}
