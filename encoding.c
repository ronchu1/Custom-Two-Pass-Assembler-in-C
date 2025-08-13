
#include "assembler.h"
#include "tables.h"
#include "word.h"

bool isValidAddressing(int opcode, int addressingMethod, bool isSrcOperand) {
    /* LEA instruction does not have a source operand*/
    if (opcode == lea && isSrcOperand) {
        return false;
    }

    /* Addressing method 0 is not valid for LEA as a source operand*/
    if (addressingMethod == 0) {
        if (!isSrcOperand) {
            return opcode == inc || opcode == dec || opcode == jmp || opcode == bne;
        }
        return opcode != lea;
    }

    return true;
}

bool isValidFirstWord(Word word) {
    /* check both sourceOperand addressing method and destinationOperand addressing method */
    return isValidAddressing(word.firstWord.opcode, word.firstWord.srcOperand, true)
           && isValidAddressing(word.firstWord.opcode, word.firstWord.destinationOperand, false);
}

int getAddressingMethod(char *parameter) {
    if (parameter[0] == '#')
        return 0; 
    if (parameter[0] == '*' && getRegisterNumber(parameter+1) != -1)
        return 2; 
	if (getRegisterNumber(parameter) != -1)
        return 3; 
    if (countChar(parameter, '.') == 1)
        return 1; 
    return 1; 
}

int instructionGroupNumber(int opcode) {
    if (opcode == mov || opcode == cmp || opcode == add || opcode == sub || opcode == lea)
        return 1;/*The instructions mov, cmp, add, sub, and lea fall into group 1*/
    if (opcode == rts || opcode == stop)
        return 3;/*The instructions rts and stop fall into group 3*/
    return 2;/*All other instructions fall into group 2 by default*/
}

int howManyWordForThisInstruction(Word word) {
    int L;
    int opcode, srcAddressing, destAddressing;
    int group;
    
    L = 1;
    opcode = word.firstWord.opcode;
    srcAddressing = word.firstWord.srcOperand;
    destAddressing = word.firstWord.destinationOperand;
    group = instructionGroupNumber(opcode);
    
    /* two operands */
    if (group == 1) {
        if (srcAddressing != 2)
            L++;
        else /* addressing method 2*/
            L += 2;
        
        if (destAddressing != 2)
            L++;
        else
            L += 2;
        if (srcAddressing == 3 && destAddressing == 3)
            L++; /* shared word for the two registers */
    }
    else if (group == 2) { 
        if (srcAddressing != 2)
            L++;
        else /* addressing method 2 */
            L += 2;
    }
    /* for zero operands L is 1 */
    return L;
}

/* recieves the opcode and parameters as a single, comma-separated string without spaces, and outputs the first word of the instruction*/
Word encodeFirstWord(int opcode, char *parameters) {
    Word word;
    int group;
    
    word.val = 0; /* A,R,E to 0*/
    word.firstWord.opcode = opcode;
    group = instructionGroupNumber(opcode);
    /* two operands*/
    if (group == 1) {
        char *currentParameter;
        if (countParameters(parameters) != 2) {
            error("group 1 instructions should have exactly 2 parameters");
            return word;
        }
        
        /* source operand*/
        currentParameter = strtok(parameters, ",");
        word.firstWord.srcOperand = getAddressingMethod(currentParameter);
        /* dest operand*/
        currentParameter = strtok(parameters, ",");
        word.firstWord.destinationOperand = getAddressingMethod(currentParameter);
        
    }
    else if (group == 2) { /* one operand */
        word.firstWord.destinationOperand = getAddressingMethod(parameters);
    }
    /* group 3 has no addressing method to put in the first word*/
    if (!isValidFirstWord(word)) {
        error("one or more invalid addressing method for for the instruction");
    }
    
    return word;
}

void encodeNumberForInstruction(char *number) {
    Word word;
    
    word.val = 0;
    if (!isNumber(number)) {
        error("invalid number parameter");
        return;
    }
    
    word.labelWord.ARE = 0; /* put "00"*/
    word.labelWord.labelVal = atoi(number);
    instructionTable[++IC - 100] = word;
}

/* for addressing methods 1,2*/
void encodeSymbolForInstruction(char *symbol) {
    Word word;
    struct SymbolNode *node;
    
    word.val = 0;
    node = searchSymbol(symbolTable, symbol);
    if (node == NULL) {
        error("invalid label for addressing method 1,2 - not in the symbol table");
        return;
    }
    if (node->data->type != EXTERNAL_SYMBOL) {
        word.labelWord.ARE = 2; /* put "10"*/
    }
    else { /* external symbol*/
        struct SymbolData data;
        word.labelWord.ARE = 1; /* put "01"*/
        
        /* insert the symbol to the table for the .ext file*/
        data.symbol = symbol;
        data.val = IC;
        insertSymbol(externalSymbolsTable, &data);
    }
    
    /* put the label value in the other 8 bits:*/
    word.labelWord.labelVal = node->data->val;
    instructionTable[++IC - 100] = word;
}

void encodeAddressingMethod2(char *parameter) {
    char *number;
    int parameterLen = strlen(parameter);
    if (parameter[parameterLen - 2] != '.') {
        error("invalid operand for addressing method 2");
        return;
    }
    parameter[parameterLen - 2] = '\0'; /* separate the symbol and the number*/
    number = &parameter[parameterLen - 1];
    
    /* encode the two parts*/
    encodeSymbolForInstruction(parameter);
    encodeNumberForInstruction(number);
}

void encodeRegister(char *reg, bool isSource) {
    int registerNumber;
    if (!(strlen(reg) == 2 && reg[0] == 'r' && reg[1] >= '0' && reg[1] <= '7')) {
        error("invalid register name for addressing method 3");
        return;
    }
    IC++; /* for the next word of the register*/
    registerNumber = reg[1] - '0';
    if (isSource) {
        setBits(&instructionTable[IC - 100], 3, 10, registerNumber);
    }
    else { /* dest register*/
        setBits(&instructionTable[IC - 100], 11, 14, registerNumber);
    }
}


void completeInstructionEncoding(int opcode, char *parameters) {
    Word firstWord;
    char *currentParameter;
    int group;
    
    firstWord = instructionTable[IC - 100];
    group = instructionGroupNumber(opcode);
    currentParameter = parameters;
    /* two operands*/
    if (group == 1) {
        /* source operand*/
        
        currentParameter = strtok(parameters, ",");
        switch (firstWord.firstWord.srcOperand) {
            case 0:
                currentParameter++; /* skip the # */
                encodeNumberForInstruction(currentParameter);
                break;
            case 1:
                encodeSymbolForInstruction(currentParameter);
                break;
            case 2:
                encodeAddressingMethod2(currentParameter);
                break;
            case 3:
                encodeRegister(currentParameter, true); /* encode source register*/
                break;
        }
        currentParameter = strtok(NULL, ","); /* move to the dest operand*/
    }
    if (group == 1 || group == 2) {
        /* encode dest operand*/
        switch (firstWord.firstWord.destinationOperand) {
            case 0:
                currentParameter++; /* skip the # */
                encodeNumberForInstruction(currentParameter);
                break;
            case 1:
                encodeSymbolForInstruction(currentParameter);
                break;
            case 2:
                encodeAddressingMethod2(currentParameter);
                break;
            case 3:
                encodeRegister(currentParameter, false); /* encode dest register*/
                break;
        }
    }
    
    /* check for special case with two registers */
    if (group == 1 && firstWord.firstWord.srcOperand == 3 && firstWord.firstWord.destinationOperand == 3) {
        IC++;
        /* merge the two last register word into 1: */
        instructionTable[IC - 100].val = instructionTable[IC - 100 - 1].val | instructionTable[IC - 100 - 2].val;
    }
}


void encodeString(char *parameters) {
    int i;
    int parameterLen = strlen(parameters);
    if (!(parameters[0] == '\"' && parameters[parameterLen - 1] == '\"')) {
        error("invalid string in .string parameter - does start or end with \"");
        return;
    }
    parameters++; /* skip the first " */
    parameters[parameterLen - 1] = '\0'; /* remove the last " */
    
    /* encode the chars of the string */
    for (i = 0; parameters[i] != '\0'; i++) {
        dataTable[DC++].val = parameters[i];
    }
}

void encodeData(char *parameters) {
    /* check if the parameters are valid*/
    char *currentNumber;
    if (!isValidCommas(parameters)) {
        error("invalid parameters for .data operation");
        return;
    }
    rmvSpacesStr(parameters);
    /* encode every number:*/
    currentNumber = strtok(parameters, ",");
    while (currentNumber != NULL) {
        if (!isNumber(currentNumber)) {
            error("invalid number on .data operation");
            return;
        }
        dataTable[DC++].val = atoi(currentNumber);
        currentNumber = strtok(NULL, ","); /* update to the next number*/
    }
    dataTable[DC++].val = 0; /* encode the last char at the string*/
}

void encodeStruct(char *parameters) {
    char *firstParameter;
    char *secondParameter;
    if (!isValidCommas(parameters) || countParameters(parameters) != 2) {
        error("invalid parameters for .struct operation");
        return;
    }
    firstParameter = strtok(parameters, ",");
    rmvSpacesStr(firstParameter);
    if (!isNumber(firstParameter)) {
        error("invalid number on .data operation");
        return;
    }
    dataTable[DC++].val = atoi(firstParameter);
    
    /* now we encode the string*/
    secondParameter = strtok(NULL, ",");
    encodeString(secondParameter);
}

