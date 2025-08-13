
#define INSTRUCTION_TABLE_SIZE 4096 
#define DATA_TABLE_SIZE 4096


struct firstWord {
    unsigned int ARE: 3;/*encoding type (A,R,E) (0-2)*/
    unsigned int destinationOperand: 4;/*Addressing method for destination (3-6)*/
    unsigned int srcOperand: 4;/*Addressing method for source (7-10)*/
    unsigned int opcode: 4;/*Operation code(11-14)*/
};

struct labelWord {
    unsigned int ARE: 3;/*encoding type (A,R,E)*/
    unsigned int labelVal: 12; /*12-bit label value*/
};

typedef union {
    struct firstWord firstWord;  /* First word format */
    struct labelWord labelWord;  /* Label word format */
    unsigned int val: 15;      /* Raw 15-bit value */
} Word;

/* Extern declarations for tables */
extern Word instructionTable[INSTRUCTION_TABLE_SIZE];

extern Word dataTable[DATA_TABLE_SIZE];

/*functions*/

int getBits(Word w, unsigned int start, unsigned int end);

void setBits(Word *w, unsigned int start, unsigned int end, int valueToSet);

Word encodeFirstWord(int opcode, char *parameters);

int howManyWords(Word word);

