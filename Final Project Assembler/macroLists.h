#ifndef LIST
#define LIST


typedef struct macro* macroPointer;
typedef struct macro {
    char mcroName[ROW_LENGTH]; 
    char* data; /* The data that the macro contains */
    int row; /* The row number that the macro was initialized on */
    macroPointer next;
} macro;

/* Function to create a new node for the macro list */
macroPointer createNode(char*, char*, int, int);

/* Function to add a new node to the macro list */
int addNodeToList(macroPointer*, macroPointer);

/* Function to free the memory allocated for the macro list */
void freeList(macroPointer);
#endif
