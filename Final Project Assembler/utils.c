#include "data.h"
#include "macroLists.h"
#include "macrosHandler.h"
#include "utils.h"

int memory[MAX_FILE_LENGTH];/*the array where all instruction are being encoded*/
int dataArray[MAX_FILE_LENGTH];/*the array where all .data .string parameters are being encoded*/
ptr p;/*the table of labels*/
extern macroPointer globalList;

const char* actions[17] = {/*holds all the names of the instructions by opcode order*/
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "stop",'\0'
};

const char* oneParam[10] = {"not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", '\0'};/*holds all the names of the instructions with one parameter by opcode order*/

const char* noParam[3] = {"rts", "stop",'\0'};/*holds all the names of the instructions with 0 parameters by opcode order*/

const char* guideSentences[5] = {/*holds all the names of the guide instructions*/
    ".data", ".string", ".entry", ".extern",'\0'
};

const char* registers[9] = {/*holds all the names of the registers by order*/
    "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7",'\0'
};

int in(char* str,const char* list[]){
	int j = 0;
	for(j=0;list[j] != '\0';j++)/*goes over the entire list until reaches null at the end of the list*/
		if(!strcmp(str, list[j]))/*if the string was found in the list return 1*/
			return 1;
	return 0;/*it went over the entire list and didnt gound the string : return 0*/
}

int isDecimalNumber(char* str) {
	while (*str)
	{/*while the string hasn't reached the end*/
    	if (*str == '.') /*if a dot was found: it must be a decimal number */
        	return 1;
        str++;/*promote the pointer by one each time*/
    }
    return 0;/*a dot wasnt found - not a decimal number*/
}

void encode(char* command, int IC, int org, int des){
	int opcode = 0, j = 0, n = 0;
	for(j = 0;j<sizeof(actions);j++)
		if(!strcmp(actions[j], command)){/*the list of the actions is ordered by the action's opcode so when the index finds te place of te action in the list, the index itself represents its opcode and then we creak the loop.*/
			opcode = j;
			break;
		}
	/*encoding by the rules with actions on the bits*/
	n = (org<<4);/*encoding by the rules - leaving  4 bits for the opcode*/
	n = (n|opcode)<<3;/*encoding by the rules - 5-8 bits are opcode - using "or" when 4 first bits are 0 and then leaving three bits for destination Addressing method*/
	n = (n|des)<<2;/*encoding by the rules - first two bits are 0 and des 2-4 using "or" when the three first bits are 0*/
	memory[IC] = n;

}

int integerEncoding(char* str, int IC, int index) { 
	int num = 0;
	char copy[ROW_LENGTH];
	strcpy(copy, str);
	
	if(atoi(copy) >= MAX_FILE_LENGTH-1)
	{/*if the given integer is bigger than 2^10 - 1 it can't be encoded in 10 bits from the given twelve */
		fprintf(stderr,"\t\t-In line %d error:\n\t\t  reached over max space encoding integers\n", index);
		return 0;
	}
	
	num = atoi(copy) << 2;/*encoding by the rules - first two bits are 0*/
    memory[IC] = num;
    
    return 1;
}
void freeSymbolList(ptr head) {

    while (head != NULL) 
    {/*while there are more nodes*/
        ptr temp = head;
        head = head->next;
        free(temp->name); /*Free the dynamically allocated name*/
        free(temp);/*Free the node itself*/
    }
}

void registerEncoding(char* reg1, char* reg2, int IC){
	int n = 0, org = reg1[2], des = reg2[2];/*the registers sent are valid so the third char in the string of each one represents the number of the register. so it puts the ascii code of the numbers in org and des which represents the origin and destination regarding parameters.*/
	
	if(!strcmp(reg1,"x"))/*as explained at te description of the function.*/
		org = 0;
	else 
		org = reg1[2] - ASCII_SUB;/*a register was indeed send so we transfer org to the real number by doing -48 beacause all numbers from 0-9 are built by: number = 48 + number(ascii)*/
		
	if(!strcmp(reg2,"x"))/*same as above */
		des = 0;
	else 
		des = reg2[2]- ASCII_SUB;
		
	n = org<<5;/*encoding by the rules - leaving 5 bits for the second parameter*/
	n = (n|des)<<2;/*encoding by the rules - using "or" on the five bits to insert des in the coding and adding two more 0 bits at the begining by the rules*/

	memory[IC] = n;
}

int doesNodeExists(char* label,char index) {
    ptr current = p;/*copy the head of the list to current */
    
    while (current != NULL) 
    {
        if(!strcmp(current->name, label)) 
        {
        	if(index == 'x')/*its a re - decleration of an extern - not an error*/
        		return -1;
            return 1;
        }
        current = current->next;
    }
    return 0;
}

int addLabelToList(char* name, int value, char type, int index) {
	ptr newCell = (ptr)calloc(1, sizeof(symbol));/*the new node to add to the list*/
	int indicator = doesNodeExists(name, type);/*basically checks if the label to add is an extern which is already defined*/
	char* temp1;
	if(indicator == 1)
	{/*if the name of the label - which represents the label- is already in the list - its an error because its supposed to be new label*/
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  tries to redefine an existing label\n",index);
		return 0;
	}
	
	else if(indicator == -1)/*if its a re - decleration of an extern - not an error*/
		return 1;
	
    temp1 = calloc(strlen(name) + 1, sizeof(char)); 
    if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	(newCell->name) = temp1;
    /*initiating the node with the given parameters*/
    strcpy(newCell->name, name);
    newCell->value = value;
    newCell->type = type;
    newCell->next = NULL;
	
    if (p == NULL) /*if the list is NULL this node will be the head of the list*/
        p = newCell;
    else 
    {
        ptr current = p;
        while (current->next != NULL) {/*goes over the list until it reaches the end and add there the new label*/
            current = current->next;
        }
        current->next = newCell;
    }
    
    return 1;/*the processhas succeeded and we return 1*/
}

int labelIsAMacro(char* label){
    macroPointer node; 
    node = globalList; /*copies the head of the macros list*/ 
    while(node != NULL)
    
    {  /* Iterate through the globalList. */
        if(strcmp(node->mcroName, label) == 0)  
            return 1;  /* If a match is found, return 1, label is a macro. */
        node = node->next;
    }
    return 0;  /* If no match is found, return 0, label is not a macro. */
}
int isLabel(char** label, char* line, int index){
	int j = 0;
	char copy[ROW_LENGTH];
	strcpy(copy, line);/*copies the line to "copy" to avoid problems with strtok*/
	
	if(copy[strlen(copy)-1] != ':')/*if line doesn't end with ':' it means the label isn't defined well and returns 0*/
		return 0;
		
	strcpy(*label, copy);/*copies the line to the address of label*/
	(*label)[strlen(*label)-1] = '\0';/*replacing the ':' with '\0' to end the string and to remove the':' because its not part of the label*/
	
	if((isalpha((*label)[0]) == 0 && !in(*label, registers))|| strlen(*label) > MAX_LABEL_LENGTH)
	{/*if the label starts with an integer or the label is longer than MAX_LABEL_LENGTH chars its against the rules of a label and returns 0*/

        fprintf(stderr, "\t\t-In line %d error:\n\t\t  defined label with more than 31 characters or a digit at the begining\n",index);

        return 0;

    }
	
	for(j = 0; j < strlen(*label) ;j++)
	{/*goes over the label and check if every char in it is an integer or a letter and if its not both - against the rules of a label - returns 0*/
		if(!isdigit((*label)[j]) && !isalpha((*label)[j]))
			return 0;
	}
	
	if(in(*label, actions) || in(*label, guideSentences) || in(*label, registers) || labelIsAMacro(*label))
	{/*if a label is a name of a macro,instruction,guide instruction or register - against the rules of a label - return 0*/
		fprintf(stderr, "\t\t-in line %d error:\n\t\t  the label is already an action or guide name or a macro\n",index);
		return 0;
	}
	
	return 1;/*if the function arrived here it means the label is valid and it will return 1 and the main function now has the label*/
}

int identifier(char* token,int index){
	char add = ':';/*used in isLabel*/
	if(atoi(token))
	{/*if the token is an number*/
		if(isDecimalNumber(token))
		{/*if its a decimal number return error*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  received a decimal number in data \n",index);
			return 0;
		}
		return INTEGER_ADDRESSING;/*returns 1 = the addressing method of an integer*/
	}
	
	if(isLabel(&token,strncat(token, &add, 1), index))/*sends the token with a ':' in the end of it and checks if its a label. because it uses the "islabel" method it must receive the label with ':'. */
		return LABEL_ADDRESSING;/*returns 3 = the addressing method of a label*/
		
	if(in(token,registers))/*checks if the token is a register*/
		return REGISTER_ADDRESSING;/*returns 5 = the addressing method of a register*/
	
	fprintf(stderr, "\t\t-In line %d error:\n\t\t  the given parameter Addressing's method isn't defined\n", index);
	
	return 0;/*the parameter is not one of the three possible addreing method - means something undefined was given and returns an error*/
}

char* replaceComma(char* copy) {
    int len = strlen(copy);
    int i = 0, j = 0;
    char line[ROW_LENGTH];/*to this variable copy will be copied*/
    char* result, *temp1 = (char*)calloc(len * 3, sizeof(char));/* allocate memory for the result string which the function returns. taking worse case - *3 */
    
    if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
    result = temp1;
    if (copy == NULL)/*if given an empty string*/
        return copy; /* Return NULL*/
        
    strcpy(line, copy);/*copies the given line to "line"*/
    
    for (i = 0, j = 0; i < len; i++) 
    { /*Iterate through each character in the temporary copy of the input string.*/
        if (line[i] == ',') 
        {/*If the current character is a comma - Adds a space, a comma, and another space to the result string.*/
            result[j++] = ' ';
            result[j++] = ',';
            result[j++] = ' ';
        } 
        else 
            result[j++] = line[i];/* If the current character is not a comma, add it to the result string. */
        
    }
    result[j] = '\0';/* Adds null-terminating character to the end of the result string. */

    return result;
}

int missingCommaOrNumber(char *line, int index){
	char* token;
	int counter = 1;/*counts the parameters and commas. if a comma is being counted - count--, otherwise - count++*/
	
	token = strtok(line," \t\n\r");
	
	if(strcmp(token, ",") != 0)/*if the first token is a parameter(could be either a parameter or a comma*/
		counter--;
		
	else/*the first token isn't an parameter - error*/
		{
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  comma before first parameter \n",index);
			return 1;
		}
		
	while(token)/*while token isn't NULL*/
	{
		if(!strcmp(token, ","))
		{/*if the token is a comma*/
			if(counter)/*if counter isn't 0 - the token before was a parameter- valid*/
				counter--;/*a comma has been counted - counter = 0*/
				
			else/*before the parameter there wasn't a parameter, meaning double comma*/
			{
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  more than one comma between two parameters \n",index);
				free(line);
				return 1;
			}
		}
		
		else
		{/*if the token is a parameter*/
			if(counter == 0)/*if the token before was a comma*/
				counter++;/*a parameter has been counted - counter = 1*/
				
			else/*means there are two parameters without a comma seperating them*/
			{
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  missing a comma between two parameters \n",index);
				free(line);
				return 1;
			}
		}
		
		token = strtok(NULL," \t\n\r");/*takes the next word*/
	}
	
	if(counter != 1)/*means line either doesn't end with a parameter*/
		{
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  the parameters weren't seperated correctly\n",index);
			free(line);
			return 1;
		}
		
	free(line);
	
	return 0;/*the line is valid and returns 0*/
	
}

int isDataValid(char* rest, char* label, int* DC, int labelFound, int index){
	int j = 0, numbers[ROW_LENGTH], k = 0;/*numbers is the array that holds all the integers of data*/
	char* token;

	token = strtok(rest, ", \t\n\r");
	/*goes over the parameters one by one and checks if they are all integers and if not returns 0*/
	while(token)
	{
		if(!atoi(token))
		{
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  received a parameter which isn't an integer \n",index);
				return 0;
		}
		
		if(isDecimalNumber(token))
		{
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  received a decimal number in data \n",index);
				return 0;
		}
		
		numbers[k++] = atoi(token);/*if the token is an integer it adds it to an array that keeps all the parameters */
		token = strtok(NULL, ", \t\n\r");
	}
	
	if(labelFound)
	{/*if a label is defined*/
		if(!addLabelToList(label, *DC, 'd', index))/*adds a node definding the label to the table of labels*/
			return 0;
	}
	
	for(j = 0; j < k ; j++)
	{
		if(numbers[j] >= MAX_BITS || numbers[j] < -MAX_BITS)
		{/*if an integer cant be represnents by 11 bits (MSB is the sign of the integers)*/
			fprintf(stderr,"\t\t-In line %d error:\n\t\t  reached over max space encoding integers\n", index);
			return 0;
		}
		
		dataArray[(*DC)++] = numbers[j];/*encode the integers in dataArray*/

	}
	return 1;/*the process has successfully ended and return 1*/
}

int isStringValid(char* restOfLine, char* label, int* DC, int labelFound, int index){
	
	int j = 0, i =0;
	char string[ROW_LENGTH] = {'\0'}, rest[ROW_LENGTH+1] = {'\0'}, copy[ROW_LENGTH+1] = {'\0'}, *token;
	/*string is the variable that will hold the parameter string that is given to .string
	rest is the variable to him restOfLine is copies
	copy is the variable that will hold the rest of the line after the prameter of .string*/
	strcpy(rest,restOfLine);
	
	/*restOfLine represents the rest of the line after .string not including the blank chars between the string and .string. meaning the first char is supposed to be ". if not - the string doesn't begin with "*/
	if(rest[0] != '"')
	{/*if the string isnt starting or ending with '"' or it only one char(meaning there arent opening and closing quotation marks) */
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  the given string doesn't have an opening quotation mark\n",index);
		return 0;
	}
	
	j++;/*we move to the next char after " we found*/
	strcat(string , "\"");/*we add " to the string*/
	
	for(j = 1; j < strlen(rest); j++){
		if(rest[j] == '\0' || rest[j] == '\n')/*if we got to the end of the line*/
			break;

		if(rest[j] == '"'){/*if we got to '"' : we are supposed to be at the end of the string given as a parameter  */
			string[j] = rest[j];/*we add the closing quatiotion to string*/
			j++;/*we move to the next char after " we found*/
			
			for(i = j; i < strlen(rest);i++){/*this loop copies the rest of the line to the variable copy. i */
			
				if(rest[i] == '\0' || rest[i] == '\n')/*if we got to the end of the line*/
					break;
				
				else if(rest[i] == '"')/*if we got to an another quatation mark - we were still inside the parameter so the rest copied to "copy" should be in 											string*/
				{
					strcat(string, copy);
					strcat(string, "\"");/*we add the closing quatiotion to string*/
					memset(copy, '\0' , strlen(copy));/*initialize copy so that now he will get the rest of the line*/
				}
				else
					copy[i - j] = rest[i];/*insert the char in rest to copy.*/
			}
			
			break;
		}
		string[j] = rest[j];/*insert the char to string.*/
	}
	if(string[strlen(string)-1] != '"')/*if the string doesn't have closing brackets*/
	{/*if the string isnt starting or ending with '"' or it only one char(meaning there arent opening and closing quotation marks) */
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  the given string doesn't have a closing quotation mark\n",index);
		return 0;
	}
	token = strtok(copy, " \t\n\r");/*searches for another paraeter*/
	
	if(token != NULL)/*if one was found - error*/
	{/* .string is supposed to get only one parameter so if tere is another word - error*/
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  more than one parameter was given after .string\n", index);
		return 0;
	}
	if(labelFound)
	{/*if a label is defined*/
		if(!addLabelToList(label, *DC, 'd', index))/*adds a node definding the label to the table of labels*/
			return 0;
	}
	
	/*this loops starts from the second char of the string(becaue the first '"') and encode each char in dataArray (by ascii code) until one before last char. after the loop has ended it encodes '\0' in dataArray a required*/
	for(j = 1; j < strlen(string)-1 ;j++)
		dataArray[(*DC)++] = string[j];
	dataArray[(*DC)++] = '\0';
	
	return 1;/*the process has successfully ended and return 1*/
}

int validation(char* line, int index){
	char* token, str[ROW_LENGTH] = {'\0'};
	/*str is the string to him the given line in the parameter will be copied*/
	strcat(str, line);
	token = strtok(str, " \t\n\r");
	
	if(token == NULL)/*means the line is empty */ 
		return 1;
		
	if(in(token, actions) || in(token, guideSentences))/*if the word is an instruction or guide word it passed the test */
		return 1;
		
	token = strtok(NULL," \t\n\r");
	
	if(token == NULL)
	{/*means there is only one word in the line - error */
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  there is only one word in the sentence - undefined command\n",index);
		return
		 0;
	}
	if(in(token, actions) || in(token, guideSentences))/*if the word is an instruction or guide word it passed the test */
		return 1;
		
	fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a sentence with eiter unknown command or unknown guide sentence\n",index);
	
	return 0;/*didnt passed the test - error */
}

int paramsAnalyzer(char* command,char* rest, char* label ,int* IC, int index){
	char* token, *first_param, *second_param, *temp1, *temp2;
	int first = 0, second = 0;
	temp1 = (char*)calloc(ROW_LENGTH, sizeof(char));
	temp2 = (char*)calloc(ROW_LENGTH, sizeof(char));
	/*first and second will hold the addressing methods of the first and second parameter (if exist)
	first_param and second_param will hold the first and second parameters given in the line*/
	if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	if (!temp2) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	first_param = temp1;
	second_param = temp2;
	if(in(command, noParam))/*if the given command is a command that gets no parameters*/
	{
		if(rest)
		{/*if rest isn't null it means a parameter was given to the command - error*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a command with no parameters but a parameter was found\n",index);
			return 0;
		}
		encode(command, (*IC)++, 0, 0);/*we know now the command and line are valids so we encode the command and raise IC*/
		
		return 1;
	}
	
	/*if we reached here that means the command gets at least one parameter*/
	if(!rest)
	{/*if rest is null it means the command didn't receive no parameter - error*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a command that requires at least one parameter but no parameter was received\n",index);
			return 0;
	}
	
	token = strtok(rest," \t\n\r");/*every word is separated by a blank char(because we sent replaceComma(rest) ) so now we take the first parameter*/
	strcpy(first_param,token);
	
	if(in(command, oneParam))/*if the command is receiving only one parameter*/
	{
		token = strtok(NULL," \t\n\r");/*token is the rest of the line after the first parameter*/
		
		if(token)
		{/*because the command is supposed to get only parameter, if token isnt null it means another parameter was sent - error*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a command with one parameter but more than one was found\n",index);
			return 0;
		}
		
		if(!(first = identifier(first_param, index)))/*if the given parameter isnt any of the three addressing method - error*/
			return 0;
		
		if(label && first_param)
			if(!strcmp(label , first_param))
			{
				fprintf(stderr, "\t\t-In line %d error:\n\t\t  the label was defined in the same line and also has been used as a parameter\n",index);
				return 0;
			}
			
		if((!strcmp("prn", command)) || (first != INTEGER_ADDRESSING && strcmp("prn", command)))
		{/*now we check if the correct addressing method was given to the command:
		if the command is "prn" all addressing methods are correct so its valid either way. if its not "prn" and the addressing method isnt 1 its also valid
		if one of the two options are correct we encode the command*/
			encode(command, (*IC)++, 0, first);
			
			switch(first)
			{/*encoding the first parameter according to his addressing method*/
				case INTEGER_ADDRESSING:/*if the parameter is an integer*/
				
					if(!integerEncoding(first_param, (*IC)++, index))/*if there was an error during encoding - error*/
						return 0;
					break;
					
				case LABEL_ADDRESSING:/*if the parameter is a label, promote IC by one - encoding label is in the second pass. by promoting IC we really save an 											empty place to the label so it will later be encoded there*/
					(*IC)++;
					break;
					
				case REGISTER_ADDRESSING:/*if the parameter is a register*/
					registerEncoding("x", first_param, (*IC)++);/*we encode the register and send "x" in the place of the second parameter because there isn't a 																	second one*/
					break;
			}
			
			return 1;/*the command and paramter have successfully been encoded and it returns 1*/
		}
		
		else
		{/*the addressing method isnt correct for the command*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  given a command with an integer - parameter, though given function doesnt receive integers\n",index);
			return 0;
		}
	}
	/*if we reached here, the instruction must be one of those who receives two parameters*/
	/*because we sent replaceComma(rest) every comma is followed by a ' ' so the comma can be either next to the last char of the token or stands by it self not connected to either of them*/

	if(!(first = identifier(first_param, index)))/*if the first parameter addressing method isn't defined - error*/
		return 0;
	
	token = strtok(NULL," \t\n\r");/*takes the second parameter*/
	
	if(!token)
	{/*if the token is null there isn't a second parameter - error*/
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  two parameters are required when only one was given\n", index);
		return 0;
	}
	if(!strcmp(token, ","))/*if the comma stands alone: take the next parameter*/
		token = strtok(NULL," \t\n\r");
	
	strcpy(second_param,token);

	token = strtok(NULL," \t\n\r");
	
	if(token)
	{/*now token represents the rest of the line after the second parameter and must be null because the command cant recieve more than 2 parameters. if not - error*/
		fprintf(stderr, "\t\t-In line %d error:\n\t\t  two parameters are required when only one was given\n", index);
		return 0;
	}
	
	if(!(second = identifier(second_param, index)))/*if the first parameter addressing method isn't defined - error*/
		return 0;
		
	if(label && first_param && second_param)
		if(!strcmp(label , first_param) || !strcmp(label , second_param))
		{
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  the label was defined in the same line and also has been used as a parameter\n",index);
			return 0;
		}
	
	if((!strcmp("lea",command) && first == LABEL_ADDRESSING && second != INTEGER_ADDRESSING) || !strcmp("cmp",command) || (strcmp("lea", command) && second != INTEGER_ADDRESSING))
	{/*now we check if the correct addressing method was given to the command:
if the command is "lea and the addressing method of the first parameter is 3 and the second's is 1 or 5 (not 3) .its valid.
if the given command is "cmp" all addressing methods are valid 
if the command isnt "lea" and the addressing method of the second parameter isn't 1 its valid*/

		encode(command, (*IC)++, first, second);/*we encode the command*/
		
		if(first == second && first == REGISTER_ADDRESSING)/*if all the parameters are regiters*/
			registerEncoding(first_param,second_param, (*IC)++);/*we encode both registers in the same word as requested*/
		/*the same idea applied in instruction with one parameter is apllied here with tow parameters*/
		
		else if(first == INTEGER_ADDRESSING)/*if the first parameter is a number*/
		{
			if(!integerEncoding(first_param, (*IC)++, index))
				return 0;
				
			switch(second)/*now go by cases according to the second parameter's addressing method*/
			{
				case INTEGER_ADDRESSING:
					if(!integerEncoding(second_param, (*IC)++, index))
						return 0;
					break;
					
				case LABEL_ADDRESSING:
					(*IC)++;
					break;
					
				case REGISTER_ADDRESSING:
					registerEncoding("x", second_param, (*IC)++);
					break;
			}
		}
		else if(first == LABEL_ADDRESSING)/*if the first parameter is a label*/
		{
			(*IC)++;
			switch(second)/*now go by cases according to the second parameter's addressing method*/
			{
				case INTEGER_ADDRESSING:
					if(!integerEncoding(second_param, (*IC)++, index))
						return 0;
					break;
					
				case LABEL_ADDRESSING:
					(*IC)++;
					break;
					
				case REGISTER_ADDRESSING:
					registerEncoding("x", second_param, (*IC)++);
					break;
				
			}
		}
		else if(first == REGISTER_ADDRESSING)/*if the first parameter is a register*/
		{
			registerEncoding(first_param,"x", (*IC)++);

			switch(second)/*now go by cases according to the second parameter's addressing method*/
			{
				case INTEGER_ADDRESSING:
					if(!integerEncoding(second_param, (*IC)++, index))
						return 0;
					break;
					
				case LABEL_ADDRESSING:
					(*IC)++;
					break;
			}
		}

		return 1;
		}
	else
	{
		if(!strcmp("lea", command))/*meaning it didnt pass the first condition*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  function lea doesn't receive a label as origin or it receives an integer as destination \n", index);
		
		else/*the only other problem is when second = 1*/
			fprintf(stderr, "\t\t-In line %d error:\n\t\t  function %s doesn't receives an integer as destination \n", index, command);
		
		return 0;
	}
	
	first_param = NULL;
	second_param = NULL;
	free(first_param);
	free(second_param);
	
}

int isExt(char* label){
    ptr symbols = p;/* Start from the first symbol in the linked list */
    
    while(symbols != NULL)
    {/*while we haven't got to the end of the list*/
        if(!strcmp(symbols->name,label))/*if we found the label in the table of labels*/
        	if(symbols -> type == 'x')/*if the label is external*/
            	return 1;/*the label is external - return 1*/
    symbols = symbols -> next;/*promote to the next node*/
    }
    return 0;/*the label isn't external - return 0*/
}

int extract(int num, int guide){
	if(guide == 2){/*if we want the six left bits*/
		num = num>>6;/*we move by bits the six left to the begining*/
		num = num & FIRST_SIX;/*we do "and" to ensure that the six left bits are zero and not "garbage" chars. the six first bits dont change because of the definition of FIRST_SIX*/
		return num;/*return the six left bits*/
	}
	return (num&FIRST_SIX);/*because of the definition of FIRST_SIX, it returns only the first six bits(we do "and")*/
}

void freeFourStrings(char* str1, char* str2, char* str3, char* str4) {
	str1 = NULL; str2 = NULL; str3 = NULL; str4 = NULL; 
    free(str1); free(str2); free(str3); free(str4);
}

