#include "assembler.h"
#include "tables.h"
#include "word.h"

void updateDataSymbols();

void firstPassLine(char line[LINE_LEN]);

void secondPass(FILE *srcFile);

Word instructionTable[INSTRUCTION_TABLE_SIZE];

void firstPass(FILE *srcFile) {
    char line[LINE_LEN];
	struct SymbolNode *node = symbolTable;	
    errFlag = false;
    IC = 100;
    DC = 0;
    
    lineNum = 1;
    line[LINE_LEN - 1] = '\n';/*line termination*/
    rewind(srcFile);
    while (fgets(line, LINE_LEN, srcFile)) {/*Read each line from the source file*/
        firstPassLine(line);
        lineNum++;
    }
    
    if (errFlag)
        return;
	/* Update the symbol table with the values of data symbols*/
    
    while (node != NULL) {
        if (node->data->type == DATA_SYMBOL) {
            /*Update the symbol value to 100 + IC*/
            node->data->val = 100 + IC;
        }
        node = node->next;
    }

    updateDataSymbols();
    
    secondPass(srcFile);
}

void firstPassLine(char *line) {
    char *label;
    char *operation;
    int instructionOpcode;
    char *parameters;
    struct SymbolData symbolData;

    /* check the length of this line */
    if (line[LINE_LEN - 1] != '\n') {
        if (line[LINE_LEN - 1] != '\0') {
            error("line is too long");
            return;
        }
        line[LINE_LEN - 1] = '\n'; /*Ensure line ends with a newline*/
    }
    
    if (line[0] == ';')
        return; /* comment line */
    line += skipSpace(line);
    if (line[0] == '\0')
        return; /* blank line */
    
    symbolData.isEntry = false;
    
    label = getLabel(line);
    operation = getOperation(line);
    parameters = getParameters(line);
    
    if (strcmp(operation, ".data") == 0 || strcmp(operation, ".string") == 0) {
        if (label) {
            
            if (searchSymbol(symbolTable, label) != NULL) {
                error("is already exists in the symbol table");
                return;
            }
            /* get all the attributes: */
            symbolData.symbol = (char *) malloc(strlen(label) * sizeof(char));
            strcpy(symbolData.symbol, label);
            symbolData.val = DC;
            symbolData.type = DATA_SYMBOL;
            insertSymbol(symbolTable, &symbolData);
        }
    
        if (strcmp(operation, ".string") == 0) {
            encodeString(parameters);
        }
        else if (strcmp(operation, ".data") == 0) {
            encodeData(parameters);
        }
        return;
    }
    
    if (strcmp(operation, ".extern") == 0 || strcmp(operation, ".entry") == 0) {
        if (strcmp(operation, ".extern") == 0) {
            if (searchSymbol(symbolTable, label) != NULL) {
                error("extern symbol defined");
                return;
            }
            /* get all the attributes: */
            symbolData.symbol = (char *) malloc(strlen(label) * sizeof(char));
            if (countParameters(parameters) != 1 || !isValidCommas(parameters)) {
                error("more than one extern symbol");
                return;
            }
            rmvSpacesStr(parameters);
            if (!isValidLabel(parameters)) {
                error("not a valid symbol");
                return;
            }
            
            strcpy(symbolData.symbol, parameters);
            symbolData.val = 0;
            symbolData.type = EXTERNAL_SYMBOL;
            insertSymbol(symbolTable, &symbolData);
        }
        return; 
    }
    
    if (label) {
        if (searchSymbol(symbolTable, label) != NULL) {
            error("Symbol already exists");
            return;
        }
        /* get all the attributes: */
        symbolData.symbol = (char *) malloc(strlen(label) * sizeof(char));
        strcpy(symbolData.symbol, label);
        symbolData.val = IC;
        symbolData.type = CODE_SYMBOL;
        insertSymbol(symbolTable, &symbolData);
    }
    
    instructionOpcode = getOpcode(operation);
    if (instructionOpcode == -1) {
        error("the name is not recognized");
        return;
    }
    
    /* check for valid syntax for the parameters */
    if (!isValidCommas(parameters)) {
        error("Parameters syntax is incorrect");
        return;
    }
    rmvSpacesStr(parameters);
    instructionTable[IC - 100] = encodeFirstWord(instructionOpcode, parameters);
    IC += howManyWords(instructionTable[IC - 100]); /* IC += L */
}
