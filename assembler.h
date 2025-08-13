/**Ron Lapushner 206455206
**/
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#define LINE_LEN 82
#define NUMBER_OF_INSTRUCTIONS 16
#define NUMBER_OF_REGISTERS 8

/* enums:*/
enum {
    DATA_SYMBOL, CODE_SYMBOL, EXTERNAL_SYMBOL
};

enum {
    mov, cmp, add, sub, lea, clr, not, inc, dec,
    jmp, bne, red, prn, jsr, rts, stop
};

/* extern file*/

extern char *srcFileName;

FILE *openFileWithEnding(char *ending, const char *mode);

/*tables*/

extern const char *instructionNames[];

extern const char *registersNames[];

extern const char octalBasis[];

/* error*/
extern int lineNum;
extern bool errFlag;

extern void error(char *errorMsg);

/* processingFunction:*/
int skipSpace(char *line);

char *getLabel(char *line);

char *getOperation(char *line);

char *getParameters(char *line);

char *rmvSpacesStr(char *string);

int countChar(const char *string, char c);

int countParameters(char *param);

bool isValidCommas(char *parameters);

bool isValidLabel(char *label);

bool isNumber(char *number);

int getOpcode(char *instruction);

int getRegisterNumber(char *reg);

/* encoding:*/

void encodeString(char *parameters);

void encodeData(char *parameters);

void encodeStruct(char *parameters);


