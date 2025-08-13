#include "word.h"
#include "assembler.h"

void error(char *errorMsg);

/* Function to extract bits from 'start' to 'end' from a Word */
int getBits(Word w, unsigned int start, unsigned int end) {
	unsigned int temp;
    if (start > end || end >= 15) {
        error("Invalid bit range");
        return -1; /*Return an error value*/
    }

     temp = w.val;
    
    /*Shift left to remove bits below 'end'*/
  	 temp <<= (14 - end);
    
    /*Shift right to remove bits above 'start'*/
	temp >>= (14 - (end + start+1));

    return temp;
}

/* Function to set bits from 'start' to 'end' in a Word */
void setBits(Word* w, unsigned int start, unsigned int end, int valueToSet) {
    int mask;

    mask = -1;
    mask <<= end + 1; 
    
    mask |= ((1 << start) - 1);     /* mask = 1...1 0...0 1...1 (the zeros are located at "start-end")*/
    w->val &= mask; /* zeros are located at "start-end" bits*/
    
    /* check if valueToSet > 2^(number of bits between start and end):*/
    if (valueToSet >= (1 << (end - start + 1))) {
        error("value for bits in the word is too big");
        return;
    }

    valueToSet <<= start; /*Align the value with the 'start' position*/
    w->val |= valueToSet; /* Set the bits in the word*/
}

/* Function to remove spaces from a string */
char *rmvSpaceStr(char *string) {
    char *dest = string;
    while (*string) {
        if (!isspace((unsigned char)*string)) {
            *dest++ = *string;
        }
        string++;
    }
    *dest = '\0';
    return string;
}

/* Function to determine how many words are required for a given instruction */
int howManyWords(Word word) {
     int L = 1;  /* The first word of the instruction is always required*/
    int srcAddressing = word.firstWord.srcOperand;
    int destAddressing = word.firstWord.destinationOperand;
    int opcode = word.firstWord.opcode;

    switch (opcode) {
        case stop:  /*Instructions that need only 1 word*/
        case rts:
            return L;

        case mov:
        case cmp:
        case add:
        case sub:
        case clr:
        case not:
        case inc:
        case dec:
        case jmp:
        case bne:
        case red:
        case prn:
        case jsr:
            /* Two operands instructions*/
            if (srcAddressing != 2) {
                L++;
            } else {
                L += 2;
            }
            if (destAddressing != 2) {
                L++;
            } else {
                L += 2;
            }
            if (srcAddressing == 3 && destAddressing == 3) {
                L--;  /* Shared word for two registers*/
            }
            break;

        default:
            /* Default case for unsupported or unexpected opcodes*/
            error("Unsupported opcode encountered in howManyWords");
            break;
    }

    return L;
}


