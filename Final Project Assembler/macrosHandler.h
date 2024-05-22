/* Function to read and fill the macro list from an input file */
int listFiller(macroPointer*, char*);

/* Function to fill the output file based on the input file and macro list */
int outputFill(char*, char*, macroPointer);

/* Function to process an input file and create an output file */
int fileHandler(char*, char*);
