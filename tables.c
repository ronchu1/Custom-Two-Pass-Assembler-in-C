#include "assembler.h"
#include "tables.h"
#include "word.h"

/* Tables for dynamic data: */
struct SymbolNode *symbolTable = NULL; /*Symbol table for defined symbols*/
struct SymbolNode *externalSymbolsTable = NULL; /*Symbol table for external symbols*/

int IC; /* Instruction Count*/
Word instructionT[INSTRUCTION_TABLE_SIZE];/* Array for storing instruction data*/
int DC;/*Data Count*/
Word dataTable[DATA_TABLE_SIZE]; /* Array for storing data*/

const char *instructionNames[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
                                  "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};
const char *registersNames[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/* Array representing octal digits*/
const char octalBasis[] = {'0', '1', '2', '3', '4', '5', '6', '7'};

/* Macro Table Functions */

/* Add a new macro to the macro list */
void insertMacro(struct MacroNode *head, struct MacroData *data) {
    struct MacroNode *newNode = (struct MacroNode *) malloc(sizeof(struct MacroNode));
    newNode->data = (struct MacroData *) malloc(sizeof(struct MacroData));
    newNode->data = data; /* Assign the macro data to the new node*/
    newNode->next = head; /* Point the new node to the old head*/
    head = newNode; /* Update the head to the new node*/
}

/* Search for a macro by name and return the corresponding node */
struct MacroNode *searchMacro(struct MacroNode *head, char *macroname) {
    struct MacroNode *current = head;
    while (current != NULL) {
        if (strcmp(current->data->name, macroname) == 0)
            return current; /* Macro found*/
        current = current->next; /* Move to the next node*/
    }
    return NULL; /* Macro not found*/
}

void insertSymbol(struct SymbolNode *head, struct SymbolData *data) {
    struct SymbolNode *newNode = (struct SymbolNode *) malloc(sizeof(struct SymbolNode));
    newNode->data = (struct SymbolData *) malloc(sizeof(struct SymbolData));
    newNode->data = data; /*Assign the symbol data to the new node*/
    newNode->next = head; /*Point the new node to the old head*/
    head = newNode; /* Update the head to the new node*/
}

/* Find and return a symbol by name from the symbol table */
struct SymbolNode *searchSymbol(struct SymbolNode *head, char *symbol) {
    struct SymbolNode *current = head;
    while (current != NULL) {
        if (strcmp(current->data->symbol, symbol) == 0)
            return current; /*Symbol found*/
        current = current->next; /* Move to the next node*/
    }
    return NULL; /*Symbol not found*/
}

/* Update all data symbols by adding the current instruction counter (IC) value to each */
void updateDataSymbols() {
    struct SymbolNode *current = symbolTable;
    while (current != NULL) {
        if (current->data->type == DATA_SYMBOL)
            current->data->val += IC; /*Update value for data symbols*/
        current = current->next; /*Move to the next symbol*/
    }
}

/* Check if there are any entry symbols in the symbol table */
bool hasEntry(struct SymbolNode *head) {
    struct SymbolNode *current = head;
    while (current != NULL) {
        if (current->data->isEntry)
            return true; /*Entry symbol found*/
        current = current->next; /*Move to the next symbol*/
    }
    return false; /*No entry symbols found*/
}

/* Free the memory used by the symbol or macro table */
void clearTable(void *head, bool isSymbolTable) {
    if (head == NULL) {
        return; /* No memory to free*/
    }

    if (isSymbolTable) { /*If clearing symbol table*/
        struct SymbolNode *current;
        while (head != NULL) {
            current = ((struct SymbolNode *) head)->next;
            free(((struct SymbolNode *) head)->data->symbol); /* Free symbol name*/
            free(((struct SymbolNode *) head)->data); /*Free symbol data*/
            free((struct SymbolNode *) head); /*Free symbol node*/
            head = current; /*Move to the next node*/
        }
    } else { /* If clearing macro table*/
        struct MacroNode *current;
        while (head != NULL) {
            current = ((struct MacroNode *) head)->next;
            free(((struct MacroNode *) head)->data->name); /*Free macro name*/
            free(((struct MacroNode *) head)->data->val); /*Free macro value*/
            free(((struct MacroNode *) head)->data); /*Free macro data*/
            free(((struct MacroNode *) head)); /*Free macro node*/
            head = current; /*Move to the next node*/
        }
    }
}

/* Initialize the symbol and external symbol tables */
void initializeTables() {
    clearTable((struct SymbolNode *) symbolTable, true); /*Clear symbol table*/
    clearTable((struct SymbolNode *) externalSymbolsTable, true); /*Clear external symbol table*/
    symbolTable = NULL; /*Reset symbol table pointer*/
    externalSymbolsTable = NULL; /*Reset external symbol table pointer*/
}
