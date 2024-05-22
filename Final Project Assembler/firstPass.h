/*This function gets the value of IC after the first pass and updates the values of data symbols in the linked list of symbols.
**     It adds the given value 'IC' to the value of each data symbol in the list.*/
void update(int);
/*this is the main function in the first pass. this function sorts every line and sends the line to the right functions to handle it. the function collects the label if defined, the instrcution/guide word and the parameters. this function gets a string line - the line being handled, a pointer to IC,DC that points to the next available plave in memory and dataArray and index - the number of the line.
if the line is valid and everything was encoded proparely flag (a  variable) is 1, otherwise 0. at the end of the function fla is returned to the main */
int sentence(char*, int*, int*, int);
