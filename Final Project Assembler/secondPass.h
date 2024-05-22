/*This function receives a file and two pointers to integers that are flags for if there is extern and entry declarations. the function checks four things:
1. it checks if every label used as a parameter or declared as entry is defined somewhere in file
2. that every label declared as an extern isn't defind 
3. that a label isn't defined as both entry and extern
4. if there is an extern and entry declarations and if so sends 1 to the integers
while going over the file the function encodes every label that is used as a parameter in memory. if the function a problem it returns 0. otherwise it returns 1.
in every line the function counts using IC the words that are being encoded and update IC accordingly. that helps us encoding the labels(using IC) 
the function is getting the file it passed the first pass so even though it analyzes every line,therefore, there are a lot of assumption we can already assume but the validation of the line*/
int fillMem(FILE*,int*,int*);

/*this function goes over every word in every line and counting every word encoded using IC. every time the function encounters a parameter label(parameter of instruction) which is also external, it writes the label and IC next to it in filename.ext (the file of externals) 
the function gets the file being handled and a string name - the name of the file being handled.*/
void ExternalsFileWriter(FILE*,char*);

/*this function goes over every line and searches for declerations of entries. once it founds one,if it hasn't already been printed), it writes the label that comes after it together with the value of the label written in the table of labels. it gets the file being handled, a string - name the name of the file being handled and number of entries that have been declared*/
void EntriesFileWriter(FILE*,char*, int);
