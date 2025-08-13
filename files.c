
#include "assembler.h"
#include "tables.h"
#include "word.h"

char *srcFileName; /* the file name without the .as extension*/

void buildObjectFile();

char *octalB(int num, bool withStartingZeros);

void buildEntryFile();

void buildExternFile();


FILE *openFileWithEnding(char *ending, const char *mode) {
    FILE *file;
    char *fileName;
    
    fileName = (char *) malloc(strlen(srcFileName) + strlen(ending) + 1);
    strcpy(fileName, srcFileName);
    strcat(fileName, ending);
    
    file = fopen(fileName, mode); /* we need to read from the .am file*/
    if (file == NULL) {
        error("couldn't open the file");
        exit(1);
    }
    return file;
}

/* build the output files after the second pass*/
void buildOutputFiles() {
    buildObjectFile();
    buildEntryFile();
    buildExternFile();
}

void buildObjectFile() {
    int i;
    FILE *objectFile = openFileWithEnding(".ob", "w");
    /* write to the object file:*/
    fprintf(objectFile, "%s %s\n", octalB(IC, false), octalB(DC, false)); /* headline*/
    /* print the instructionTable:*/
    for (i = 0; i < IC - 100; i++) {
        fprintf(objectFile, "%s %s\n", octalB(i + 100, true), octalB(instructionTable[i].val, true));
    }
    /* print the dataTable*/
    for (i = 0; i < DC; i++) {
        fprintf(objectFile, "%s %s\n", octalB(IC + i, true), octalB(dataTable[i].val, true));
    }
    fclose(objectFile);
}

void buildEntryFile() {
    FILE *entryFile;
    struct SymbolNode *current;
    
    if (!hasEntry(symbolTable))
        return; /* not build empty .ent file*/
    
    entryFile = openFileWithEnding(".ent", "w");
    current = symbolTable;
    
    /* print to the entry file every .entry symbol */
    while (current != NULL) {
        if (current->data->isEntry) {
            fprintf(entryFile, "%s %s\n", current->data->symbol, octalB(current->data->val, true));
        }
        current = current->next;
    }
    fclose(entryFile);
}

void buildExternFile() {
    FILE *externalFile;
    struct SymbolNode *current;
    
    if (externalSymbolsTable->data->symbol == NULL)
        return; /* we do not build empty .ext file*/
    
    externalFile = openFileWithEnding(".ext", "w");
    current = externalSymbolsTable;
    
    /* print to the entry file every .entry symbol and its value*/
    while (current != NULL) {
        fprintf(externalFile, "%s %s\n", current->data->symbol, octalB(current->data->val, true));
        current = current->next;
    }
    fclose(externalFile);
}
