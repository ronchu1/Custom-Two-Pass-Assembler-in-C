#include "assembler.h"

int skipSpace(char *line) {
    int i = 0;
    while (isspace(line[i])) {
        i++;
    }
    return i;
}

/* return the number of chars we need to skip the label*/
int skipLabel(const char *line) {
    int i = 0;
    while (line[i] != ':' && line[i] != '\0') {
        i++;
    }
    if (line[i] != '\0')
        return 0; /* has no label*/
    return i + 1;
}

char *getLabel(char *line) { /*Error*/
    char lineCopy[LINE_LEN];
    char *label;
    
    line += skipSpace(line);
    strcpy(lineCopy, line);
    label = strtok(lineCopy, ":");
    
    if (line[strlen(label) + 1] != ':')
        return NULL; /*There is no label*/
    
    if (!isValidLabel(label)) {
        error("invalid label");
        return NULL;
    }
    
    return label;
}

bool isValidLabel(char *label) {
    int i;
    if (strlen(label) > 30)
        return false;
    
    if (!isalpha(label[0]))
        return false; /* first char must be a letter*/
    
    for (i = 1; label[i] != '\0'; i++) {
        if (!isalnum(label[0]))
            return false; /*There is no label*/
    }
    return true;
}


char *getOperation(char *line) {
    char *lineCopy;
    
    lineCopy = (char *) malloc((strlen(line) + 1) * sizeof(char));
    strcpy(lineCopy, line);
    lineCopy += skipSpace(lineCopy);
    lineCopy += skipLabel(lineCopy);
    
    return strtok(lineCopy, " \t");
}


char *getParameters(char *line) {
    int parametersLen;
    char *parameters;
    
    parameters = (char *) malloc((strlen(line) + 1) * sizeof(char));
    strcpy(parameters, line);
    parameters += skipLabel(parameters);
    
    /* skip the operation:*/
    parameters += skipSpace(parameters);
    while (!isspace(parameters[0])) {
        parameters++;
    }
    parameters += skipSpace(parameters); /* skip the spaces before the parameters*/
    parametersLen = strlen(parameters);
    
    /* skip spaces after the parameters*/
    while (isspace(parameters[parametersLen - 1])) {
        parameters[parametersLen - 1] = '\0'; /* remove the space from the end*/
        parametersLen--;
    }
    
    return parameters;
}


/* Function removing spaces from string*/

char *rmvSpacesStr(char *string) {
    /* non_space_count to keep the frequency of non space characters*/
    int non_space_count = 0;
    
    int i;
    for (i = 0; string[i] != '\0'; i++) {
        if (string[i] != ' ' && string[i] != '\t') {
            string[non_space_count] = string[i];
            non_space_count++;/*non_space_count incremented*/
        }
    }
    
    /*Finally placing final character at the string end*/
    string[non_space_count] = '\0';
    return string;
}

bool isValidCommas(char *parameters) {

    int i = 0;
    while (true) {
        /* skip the spaces before the parameter*/
        while (isspace(parameters[i]))
            i++;
        
        if (parameters[i] == '\0')
            return true; /* all the commas were valid, and we reached to the end of the parameter*/
        
        if (parameters[i] == ',') {
            return false; /* ',' before the parameter*/
        }
        
        /* skip the parameter*/
        while (!isspace(parameters[i]) && parameters[i] != ',')
            i++;
        
        /* skip the spaces after the parameter*/
        while (isspace(parameters[i]))
            i++;
        if (parameters[i] != ',' && parameters[i] != '\0')
            return false; /* missing ',' after the parameter*/
        i++; /* skip the comma*/
    }
}

/* check if the current string is a number*/
bool isNumber(char *number) {
    int i;
    if (number[0] == '-')
        number++; /* skip the minus sign*/
    
    for (i = 0; number[i] != '\0'; i++) {
        if (isdigit(number[i]) == false)
            return false;
    }
    return true;
}

/* count how many times c in string*/
int countChar(const char *string, char c) {
    int i;
    int count = 0;
    for (i = 0; string[i] != '\0'; i++) {
        if (string[i] == c) {
            count++;
	}
    }
    return count;
}

int countParameters(char *parameters) {
    parameters += skipSpace(parameters);
    if (parameters[0] == '\0') {
        return 0; 
    }
    
    return countChar(parameters, ',') + 1; 
}

/* return the opcode*/
int getOpcode(char *instruction) {
    int i;
    for (i = 0; i < NUMBER_OF_INSTRUCTIONS; i++) {
        if (strcmp(instruction, instructionNames[i]) == 0)
            return i;
    }
    return -1;
}

int getRegisterNumber(char *reg) {
	int i;
    for (i = 0; i < NUMBER_OF_REGISTERS; i++) {
        if (strcmp(reg, registersNames[i]) == 0)
            return i;
    }
    return -1;
}

char *octalB(int num, bool withStartingZeros) {
    /* Allocate memory for the octal string: 5 digits + null terminator*/
    char *octalString = (char *)malloc(6); 
    
    if (octalString == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }
    
    /*Format the number as a five-digit octal string*/
    if (withStartingZeros) {
        sprintf(octalString,"%05o", num); /*Include leading zeros*/
    } else {
        sprintf(octalString,"%o", num); /*No leading zeros*/
    }
    
    return octalString;
}

