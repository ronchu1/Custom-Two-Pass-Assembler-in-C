
#include "assembler.h"
#include "tables.h"

void secondPassLine(char *line);

void completeInstructionEncoding(int opcode, char *parameters);

void buildOutputFiles();


void secondPass(FILE *srcFile) {
    char line[LINE_LEN];
    
    IC = 0;
    DC = 0;
    
    lineNum = 1;/*iniftialize line number*/
    rewind(srcFile);/* Rewind the file to start from the beginning*/
    while (fgets(line, LINE_LEN, srcFile)) {/* Process each line in the source file*/
        secondPassLine(line);
        lineNum++;
    }
    
    /*if we have errors in the second pass we stop*/
    if (errFlag)
        return;

    
    fclose(srcFile); /* Close the source file after processing*/
    buildOutputFiles();  /* Build the output files*/
}

void secondPassLine(char *line) {
    char *operation;
    char *parameters;
    
    operation = getOperation(line);
    parameters = getParameters(line);
    
    if (strcmp(operation, ".data") == 0 || strcmp(operation, ".string") == 0 || strcmp(operation, ".extern") == 0){
        return; 
    }
    
    if (strcmp(operation, ".entry") == 0) {
        struct SymbolNode *node = searchSymbol(symbolTable, parameters);
        if (node == NULL) {
            error("symbol after .entry operation is not defined");
            return;
        }
        node->data->isEntry = true;
        return;
    }
    
    /*encode all the words except the first one*/
   	completeInstructionEncoding(getOpcode(operation), parameters);
}
