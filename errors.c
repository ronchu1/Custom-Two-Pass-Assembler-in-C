
#include "assembler.h"

int lineNum;
bool errFlag;

void error(char *errorMsg) {
    errFlag = true; /*error has occured*/
	/*print the error message that includesthe source file name, line number, and the error message*/
    fprintf(stderr, "In File %s, in line number %d: %s.\n", srcFileName, lineNum, errorMsg);
}
