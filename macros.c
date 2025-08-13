#include "assembler.h"
#include "tables.h"

void preAssemblerLine(char *line, FILE *srcFile, FILE *srcFileWithoutMacros, struct MacroNode *macroTable);

void firstPass(FILE *srcFile);

void preAssembler(FILE *srcFile) {
    FILE *srcFileWithoutMacros;
    struct MacroNode *macroTable;
    char line[LINE_LEN];

    srcFileWithoutMacros = openFileWithEnding(".am", "w+");
    macroTable = NULL;
    while (fgets(line, LINE_LEN, srcFile)) {
        preAssemblerLine(line, srcFile, srcFileWithoutMacros, macroTable);
    }

    clearTable(macroTable, false);
    firstPass(srcFileWithoutMacros);
}


void preAssemblerLine(char *line, FILE *srcFile, FILE *srcFileWithoutMacros, struct MacroNode *macroTable) {
    char *macroName;
    struct MacroNode *macroNode;

    if(line[0] == ';')
        return; /* skip comment lines*/

    line += skipSpace(line);
    if(line[0] == '\0')
        return; /* this is an empty line*/

    macroName = getOperation(line);
    macroNode = searchMacro(macroTable, macroName);

    /* check for macro line:*/
    if (getLabel(line) == NULL && getParameters(line) == NULL && macroNode != NULL) {
        fprintf(srcFileWithoutMacros, macroNode->data->val, "\n");
        return;
    }

    /* define macro*/
    if (getLabel(line) == NULL && strcmp(getOperation(line), "macro") == 0) {
        struct MacroData macroData;
        char temp[LINE_LEN];


        strcpy(macroData.name, getParameters(line)); 
        while (fgets(line, LINE_LEN, srcFile)) {
            strcpy(temp, line);
            if (strcmp(rmvSpacesStr(temp), "endmacro") == 0) {
                insertMacro(macroTable, &macroData);
                return;
            }
            /* we are in a line that is a part of the macro data:*/
            macroData.val = realloc(macroData.val, sizeof(macroData.val)
                                                       + strlen(line) * sizeof(char) + 1);
            /* add the value of the macro:*/
            strcat(macroData.val, line);
            strcat(macroData.val, "\n");
        }
    }
    fprintf(srcFileWithoutMacros, line, "\n");/* Handle lines outside of macro definitions*/
}
