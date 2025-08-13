
#include "assembler.h"


void initializeTables();

void preAssembler(FILE *srcFile);

int main(int argc, char *argv[]) {
    FILE *srcFile;
    int i;
    for (i = 1; i < argc; i++) {
        srcFileName = argv[i];
        if ((srcFile = openFileWithEnding(".as", "r")) == NULL) { /* Open the source file with a ".as" extension for reading*/
 	 /* Print an error message and skip to the next file if the current file can't be opened*/
            error("couldn't open the file");
            continue;
        }
        
        initializeTables();
        preAssembler(srcFile);
        fclose(srcFile); /* Close the source file after processing*/
    }
    
    return 0; /*exit the program successfully*/
}

