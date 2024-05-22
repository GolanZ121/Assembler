#include "data.h"
#include "convert.h"
#include "utils.h"
extern int memory[];
extern int dataArray[];
/*this list represents every char in base 64. 
the list is arranged by order of base 64: the first char in the list is 0 in base 64 and the last char in the list is 63 in base 64*/
const char base64[] ={	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
                        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                        '0','1','2','3','4','5','6','7','8','9', '+','/'
};



void converter(char* Name, int DC){
	int j = INIT_MEM_POS, right = 0, left = 0;
	char* fileName, *temp1;
	FILE* file;
	temp1 = (char*)calloc(strlen(Name) + strlen(".ob") + 1,sizeof(char));/*the name of the object file created*/
	if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	fileName = temp1;
    strcat(fileName,Name);
    strcat(fileName,".ob");
    fileName[strlen(fileName)] = '\0';
    if(!(file = fopen(fileName,"w"))){
			fprintf(stderr, "\t\tcannot open file: %s\n", fileName);
			return;
	}
	/*goes over memory and encodes every 12 bits data to the object file until memory[j] = 0 meaning we encoded all the data*/
	while(memory[j] != 0)
		{
			right = extract(memory[j], 1);/*uses extract and inserts to the integer right the six right bits of the data*/
			left = extract(memory[j], 2);/*uses extract and inserts to the integer left the six left bits of the data*/
			fprintf(file, "%c%c\n", base64[left],base64[right]);/*prints to the object file right and left in base 64.*/
			j++;
		}
	j=0;/*dataArray encoding starts from index 0(unlike memory which starts from 100)*/
	for(j = 0; j < DC;  j++)
		{
			right = extract(dataArray[j], 1);/*uses extract and inserts to the integer right the six right bits of the data*/
			left = extract(dataArray[j], 2);/*uses extract and inserts to the integer left the six left bits of the data*/
			fprintf(file, "%c%c\n", base64[left],base64[right]);/*prints to the object file right and left in base 64.*/
		}
	fclose(file);
}
