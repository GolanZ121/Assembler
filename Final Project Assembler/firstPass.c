#include "data.h"
#include "macroLists.h"
#include "utils.h"
#include "firstPass.h"

extern int memory[MAX_FILE_LENGTH];
extern int dataArray[MAX_FILE_LENGTH]; 
extern ptr p;
extern macroPointer globalList;

const char* allActions[17] = {/*holds all the names of the instructions*/
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop",'\0'
};



const char* allGuideSentences[5] = {/*holds all the names of the guide instructions*/
    ".data", ".string", ".entry", ".extern",'\0'
};



/*This function gets the value of IC after the first pass and updates the values of data symbols in the linked list of symbols.
**     It adds the given value 'IC' to the value of each data symbol in the list.*/
void update(int IC) {
    ptr current = p;/* Start from the first symbol in the linked list */
   
    while (current != NULL) 
    { /* Iterate through the linked list of symbols */
        if (current->type == 'd') 
            current->value += IC;
        current = current->next;/* Move to the next symbol in the linked list */
    }
}



/*this is the main function in the first pass. this function sorts every line and sends the line to the right functions to handle it. the function collects the label if defined, the instrcution/guide word and the parameters. this function gets a string line - the line being handled, a pointer to IC,DC that points to the next available plave in memory and dataArray and index - the number of the line.
if the line is valid and everything was encoded proparely flag (a  variable) is 1, otherwise 0. at the end of the function fla is returned to the main */
int sentence(char* line, int* IC, int* DC, int index){
	char* label, *token,*token2 ,*token3 , *rest;
	char* temp1,*temp2,*temp3,*temp4;
	int labelFound = 0, flag = 0, holder = 0;
	temp1 = (char*)calloc(ROW_LENGTH, sizeof(char));
	temp2 = (char*)calloc(ROW_LENGTH, sizeof(char));
	temp3 = (char*)calloc(ROW_LENGTH, sizeof(char));
	temp4 = (char*)calloc(ROW_LENGTH, sizeof(char));
	if (!temp1 || !temp2 || !temp3 || !temp4) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	token2 = temp1;
	token3 = temp2;
	rest = temp3;
	label = temp4;
	/**/
	
	if(*line == ';' || line == NULL)/*if the line is a comment line or empty line return 1 - the line is valid*/
		return 1;
	if(!validation(line, index)){/*if the line isn't valid return 0. we use this function to filter out a lot of false lines that otherwise would be harder to handle*/freeFourStrings(token2, token3, rest, label);
		return 0;
	}
	token = strtok(line, " \t\n\r");
	if(!token){
		freeFourStrings(token2, token3, rest, label);
		return 1;
		}
	if(memory[MAX_FILE_LENGTH-1] != 0)
	{/*if we reached here and the line is valid something needs to be encoded. if the memory is already full - error*/
		fprintf(stdout, "\tmemory has reached max capacity!");
		freeFourStrings(token2, token3, rest, label);
		return 0;
	}
	if(isLabel(&label, token, index))
	{/*if the first word is a definition of a label: insert the label to "label", light the flag and take the next word*/
		
		labelFound = 1;
		token = strtok(NULL, " \t\n\r");

	}


	if(in(token, allGuideSentences))
	{/*if the token is one of the guide words(.extern .entry .data .string)*/
		if(!strcmp(token, ".entry") || !strcmp(token, ".extern")) 
		{/*if its .entry or .extern*/
			if(labelFound)/*if a label was defined before - error*/
				fprintf(stdout, "\t\t-In line %d warning:\n\t\t  label is defined before .entry or .extern\n", index);
			
			token2 = strtok(NULL, " \t\n\r");/* takes the word on which the user do .entry or .extern*/
			if(!token2)
			{/*if token2 is null it means there is no word - error*/
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  missing label after .entry or .extern\n", index);
				freeFourStrings(token2, token3, rest, label);
				return 0;
			}
			strcat(token3, token2);/*transfer the word to token3*/
			token2 = strtok(NULL," \t\n\r");/*takes the rest of the line after the word*/
			if(token2)
			{/*if there is a char which isn't blank - error*/
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  more than one parameter was given after .entry/.extern \n",index);
				freeFourStrings(token2, token3, rest, label);
				return 0;
			}
			if (isLabel(&(token3), strncat(token3, ":", 1), index)){/*if the word in token3 is a label(not necessarily defined). adds':' because isLabel is designated to check if a definition of a label is valid. */
				if (!strcmp(token, ".extern")) /*if the guide word is .extern*/
				    flag = addLabelToList(token3, 0, 'x', index);/*adds the label to the table of labels with 0 value an 'x' type to mark him as external*/  	
				else
				    flag = 1;/*if the guide word is .entry, its handled in the second pass but the line is valid so we mark the flag as 1*/
				
			}
			else
			{
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a label to define as entry or extern but the label isn't valid\n", index);
				freeFourStrings(token2, token3, rest, label);
				return 0;
			}
		}
		else if(!strcmp(token, ".data")){/*if the guide word is .data*/
			token = strtok(NULL, "\n");/*takes the rest of the line.data*/
			if(!token)
			{/*if the token is null - there is no parameter - error*/
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  missing data after .data\n", index);
				freeFourStrings(token2, token3, rest, label);
				return 0;
			}
			strcpy(rest,token);/*copies the rest of the line to rest*/
			if(!missingCommaOrNumber(replaceComma(rest), index))/*if the parameters arent proparely seperated by commas return 0*/
				flag = isDataValid(rest, label, DC, labelFound,index);/*sends the parameters to isDataValid to check if they are valid and sperated by commas and if so encodes them. if it is completed 1 returns to flag , otherwise 0*/
		}
		else
		{/*if the guide word is .string*/
			
			token = strtok(NULL, "\n");/*takes then given string*/
			if(!token)
			{/*if there isn't a string - error*/
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  missing string after .string\n", index);
				freeFourStrings(token2, token3, rest, label);
				return 0;
			}
			strcpy(rest, token);/*copies the string to rest*/
			

			flag = isStringValid(rest, label, DC, labelFound, index);/*sends the rest of the line to isStringValid to check if it is valid and if so encodes it char by char. if it is completed 1 returns to flag , otherwise 0*/
		}
	}
	else if(in(token, allActions))
	{/*if the token is an instruction word*/
		holder = *IC;/*holds IC's integer if the line is valid and there is a label. IC is changing in paramAnalyzer and holder saves his value*/
		token2 = strtok(NULL, "\n");/*takes the rest of the line*/
		if(!token2)/*if token2 is null - there is no parameter - action with no param - sends token2 = null*/
			flag = paramsAnalyzer(token,token2,label,IC, index);
		else if(!missingCommaOrNumber(replaceComma(token2), index))/*if the parameters are seperated proparely by commas*/
			flag = paramsAnalyzer(token,replaceComma(token2),label,IC, index);/*sends the instruction and parameters collected(it sends the parameters after being handled by replaceComma)*/
		if(flag && labelFound)/*if the command and parameter have succcessfully been encoded - flag = 1. if there is label, labelFound = 1. if they are both 1 we need to encode the label with the IC before the changes - meaning using holder*/
			flag = addLabelToList(label, holder, 'c', index);

		
	}
	else
	{/*if the word didn't enter to any of the conditions - its an undefined word - error*/
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  given neither guide sentence nor instruction sentence or label wasn't defined well\n", index);
		flag = 0;
	}
	
	freeFourStrings(token2, token3, rest, label);
	

	return flag;
}
