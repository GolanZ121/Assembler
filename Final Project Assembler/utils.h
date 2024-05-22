#ifndef SYMBOL
#define SYMBOL

typedef struct node* ptr;
typedef struct node {
	char* name;
	int value;
	char type;
	ptr next;
}symbol;

/*This function receives a string and a name of a (global) list and checks if the string is inside the list and if so returns 1. otherwise 0.*/
int in(char*,const char* list[]);

/*This function gets a string "command" and integers IC,org,des.
it encodes the command in "memory" using the three integers sent and the rules for encoding a command.
IC represents the next empty place to encode the command. org represents the Addressing method of the first paramter(origin). 
des represents the Addressing method og the descond parameter(destination).
incase there is only one parameter, org = 0. if there are no parameters,org = des = 0*/
void encode(char*, int, int, int );

/*This function gets a string str which represents thhe integer we want to encode in memory,int IC that represents the next empty place to encode the command
and an inde for the error message. this function encodes the number given as a parameter by the rules in memory
if the number was encoded - the function returns 1, otherwise 0*/
int integerEncoding(char*, int, int);

/*This function gets reg1, reg2 strings that represents the registers iven as parameters and IC the next available place in memory. This function encodes registers given as a parameter by the rules.if an "x" was sent in one of the two strings it means that this parameter isnt a register so we need to encode zero instead of it.*/
void registerEncoding(char*, char*, int);

/*This function inserts a new label to the table of labels if possible and aloud. The function gets a string name, an integer value, a char type and an integer index. name represents the name of the label. value - the place in the memory of the label. type - if the label was encoded in an intruction sentence type = 'c'. if it a encoded in a ".data" or ".string" type ='d'. if in ".extern" type = x and value = 0
if it has added the label - returns 1, otherwise 0*/
int addLabelToList(char*, int, char, int);

/*this function gets an int num - the number with 12 bits encoded in memory and an int guide - indicates which 6 bits(right or left) does the function needs to extract and return.
guide = 2 - we want the left six bits
guide = 1 we want the right six bits
*/
int extract(int, int);


/*This function checks if a definition of a label is label by the given rules.if it is valid it inserts the label to the pointer of the string "label". line represents the string intended to be a label. 
if the given string is according to the rules of a label - returns 1,otherwise 0*/
int isLabel(char**, char*, int);

/*This function get a string label and a char index which represents if the label is an external. this function checks if a given label is already in the table of labels
if it has found the label - returns 1, otherwise 0*/
int doesNodeExists(char*, char);

/* This function checks if a given label is a macro by comparing it with the macro names in the globalList. 
if it is also a macro name - returns 1,otherwise 0*/
int labelIsAMacro(char*);

/*this function is used to determine a parameter's addressing method. the given token represents the parameter. if it is indeed a valid parameter, it returns his addressing method(1,3 or 5)*/
int identifier(char*, int);

/*This function gets a line with the parameters of an instruction or ".data". the parameters are supposed to be seperated by one comma and the function checks if it is true. the parameter "rest" represents the parameters with the commas and blank chars but the rest given comes after being handled by "replaceComma" function.
that means that every word when using strtok when the delimiters are only blank chars - every token will be either "," or a parameter.
returns 1 if there is a problem with the parameters and the speration, otherwise 0.
*/
int missingCommaOrNumber(char*, int);

/*this function checks if the parameters of ".data" are valid(not including their seperating commas- which is already been checked) and if so encodes it in dataArray.
it gets a string rest which represent the line holding the parameters, the string label holds the label if defined before ".data", DC - the next available place in dataArray, labelFound is an integer that if a label were defined is 1 and if not 0. index holds the number of the line handled
if the parameters of data is valid - returns 1,otherwise 0*/
int isDataValid(char*, char*, int*, int, int);

/*this function checks if the parameter of ".string" is valid and if so encodes it in dataArray.
it gets a string rest which represent the line holding the parameter, the string label holds the label if defined before ".data", DC - the next available place in dataArray, labelFound is an integer that if a label were defined is 1 and if not 0. index holds the number of the line handled
if the string is valid - returns 1, otherwise 0*/
int isStringValid(char*, char*, int*, int, int);

/*this function gets a string and adds  before and after each ',' a ' '. this function is being used to help check if a line containing several parammeters is valid
the function returns the fixed string(or null if the line is empty)*/
char* replaceComma(char*);

/*this function checks if the first two words in a given line are valid meaning that the first or second word must be an instruction or guide word. otherwise the line isn't valid 
if te line is valid - returns 1, otherwise 0*/
int validation(char*, int);

/*This function analyzes an intruction sentence, checks if the sentence is valid and if so encodes it according to the rules.
it gets a string command - the instruction, string rest - the rest of the line containing the parameters(replaceComma(rest) is sent), int IC an index of the next available place in memory
and index - the number of the line we are handling 
if the command and parameters have have been successfully encoded - returns 1, otherwise 0*/
int paramsAnalyzer(char*, char*,char*, int*, int);

/*This function receives a string which represents a number and checks is it sa decimal number and if so return 1. otherwise 0.*/
int isDecimalNumber(char*);

/*this function gets the name of a label and checks in the table of label if it is marked as extenal
if the given label is external - returns 1, otherwise 0*/
int isExt(char*);

/*this functio gets the ptr of the first node of the table of labels(symbol). it frees the allocated memory in it*/
void freeSymbolList(ptr head);

/*this function is designated for the sentence function to free all four variables there at once*/
void freeFourStrings(char* str1, char* str2, char* str3, char* str4);


#endif
