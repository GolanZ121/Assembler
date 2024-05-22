#include "data.h"
#include "macroLists.h"


macroPointer createNode(char* name, char* data, int rowNum, int dataRows) {
	char* temp1;
    macroPointer newNode = malloc(sizeof(macro));
    strncpy(newNode->mcroName, name, ROW_LENGTH);
    
    temp1 = (char*)calloc(ROW_LENGTH * dataRows, sizeof(char)); 
    if (!temp1) {
    	fprintf(stderr, "memory allocation failure\n");
    	exit(0);
	}
	(newNode->data) = temp1;
    strcpy(newNode->data, data);
    newNode->row = rowNum;
    newNode->next = NULL;
    return newNode;
}



int addNodeToList(macroPointer* listPtr, macroPointer newNode) {
    macroPointer curr = *listPtr;
    int i = 0;

    /* Array of valid commands */
    const char* cmnds[] = {
        "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
        "jmp", "bne", "red", "prn", "jsr", "rts", "stop",
        ".data", ".string", ".entry", ".extern"
    };
    
    /* Check if the macro name already exists in the list */
    
    while (curr != NULL) {
        if (strcmp(curr->mcroName, newNode->mcroName) == 0) 
            return 0; /* Macro name already exists, return 0 as an error code */
        curr = curr->next;
    }

    /* Check if the macro name is a valid command */
    for (i = 0; i < 20; i++) {
        if (strcmp(newNode->mcroName, cmnds[i]) == 0)
            return -1; /* Macro name matches a valid command, return -1 as an error code */
    }
    
    /* Add the new node to the end of the list */
    if (*listPtr == NULL) 
        *listPtr = newNode; /* The list is empty, set the new node as the head */
    else {
        curr = *listPtr;
        while (curr->next != NULL) 
            curr = curr->next; 
        curr->next = newNode; /* Add the new node to the end */
    }
    
    return 1; 
}


void freeList(macroPointer list) {
    while (list != NULL) {
        macroPointer temp = list;
        list = list->next;
        
        free(temp->data);/* Free the current nodes data */
        free(temp); /* Free the current node */
    }
}
