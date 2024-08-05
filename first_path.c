#include "functions.h"
#include "libary_h.h"
#include "global_var.h"

struct symTable* findSymbol(struct symTable* symHead, char* symbol) {
    struct symTable* current = symHead;
    /* Iterate over the symbol table */
    while (current != NULL) {
        /* Compare the current symbol name with the given symbol */
        if (strcmp(current->symName, symbol) == 0) {
            /* If they match, return the pointer to the symbol table node */
            return current;
        }
        current = current->nextSym;
    }
    /* If the symbol was not found in the table, return NULL */
    return NULL;
}

void insertToSymTable(struct symTable* tempSym, struct symTable** symTail, struct symTable** symHead, char* name, int value, symbol_type type, entry_extern flag) {
    /* Allocate memory for a new symbol */
    struct symTable* newSym = (struct symTable*)malloc(sizeof(struct symTable));

    /* Copy data into the new symbol */
    strcpy(newSym->symName, name);
    newSym->symValue = value;
    newSym->symType = type;
    newSym->entryExternFlag = flag;
    newSym->nextSym = NULL;

    /* Check if the symbol table is empty */
    if (*symHead == NULL) {
        /* If the symbol table is empty, set the new symbol as both head and tail */
        *symHead = newSym;
        *symTail = newSym;
    } else {
        /* If the symbol table is not empty, add the new symbol to the tail */
        (*symTail)->nextSym = newSym;
        *symTail = newSym;
    }
}

int getLabel(char* line, char* label) {
    /* If line starts with .entry or .extern, skip to the end of the directive */
    if (isEntry(line) || isExtern(line)) {
        while (!isspace(*line)) {
            line++;
        }
    }

    /* Skip any whitespace after directive or at the start of line */
    while (isspace(*line)) {
        line++;
    }

    char* colonPos = strchr(line, ':');
    if (colonPos != NULL) {
        strncpy(label, line, colonPos - line);
        label[colonPos - line] = '\0';
        return 1;
    }
    return 0;
}

int getInstructionSize(char* line, struct symTable* symHead) {
    int size = 0;

    /*Temp copy of line, for the strtok function*/
    char tempLine[MAX_LINE_LENGTH];
    strncpy(tempLine, line, MAX_LINE_LENGTH);

    /* Skip leading whitespaces*/
    char* linePtr = tempLine;
    while (isspace((unsigned char)*linePtr)) {
        linePtr++;
    }

    /*Check for label*/
    char label[MAX_LINE_LENGTH];
    int hasLabel = getLabel(tempLine, label);
    if (hasLabel) {
        /* Remove label from line*/
        linePtr = strchr(tempLine, ':') + 1;
    }

    /* Check if the line is not empty or a comment*/
    if (*tempLine != '\0' && *tempLine != ';') {
        /*Split the line into opcode and operands*/
        char* opcode = strtok(tempLine, " \t");
        char* operands = strtok(NULL, "");

        /* Count the opcode as one word*/
        size++;

        /* Count the number of operands*/
        if (operands != NULL) {
            char* token = strtok(operands, ",");
            char* firstOperand = token;
            char* secondOperand = NULL;
            int regCount = 0;
            while (token != NULL) {
                secondOperand = token;
                if (isRegister(token)) {
                    regCount++;
                }
                token = strtok(NULL, ",");
            }
                   

            /* If both operands are registers, increment size by only 1*/
            if (regCount == 2) {
                size += 1;
            }
            /* Otherwise, increment size by the number of non-register operands*/
            else {
                /*If firstOperand and secondOperand are same, increment size only once*/
                if (firstOperand != NULL && secondOperand != NULL && strcmp(firstOperand, secondOperand) == 0) {
                    size += 1;
                }
                else {
                    size += (firstOperand != NULL && !isRegister(firstOperand) || firstOperand != NULL  && isRegister(firstOperand)) ? 1 : 0;
                     printf("firstOperand: %s .  second operand: %s size: %d\n", firstOperand,secondOperand,size); 
                    size += (secondOperand != NULL && !isRegister(secondOperand) || secondOperand != NULL 
                      && isRegister(secondOperand) && !isRegister(firstOperand)) ? 1 : 0;
                     
                }
            }
        }
    }

    return size;
}


 

int symbolExists(struct symTable* symHead, char* label) {
    struct symTable* current = symHead;
    while (current != NULL) {
        if (strcmp(current->symName, label) == 0) {
            return 1; /* Symbol already exists in the table*/
        }
        current = current->nextSym;
    }
    return 0; /* Symbol does not exist in the table*/
}

void getFirstOperand(char* line, char* operand) {
    char* start = strchr(line, ' '); /* assuming space after instruction*/
    if (start != NULL) {
        char* end = strchr(start + 1, ',');
        if (end != NULL) {
            strncpy(operand, start + 1, end - start - 1);
            operand[end - start - 1] = '\0';
        }
        else {
            printf("Error: No operand found in line: %s\n", line);
        }
    }
}

void getSecondOperand(char* line, char* operand) {
    char* start = strchr(line, ',');
    if (start != NULL) {
        char* end = strchr(start + 1, '\n');
        if (end != NULL) {
            strncpy(operand, start + 1, end - start - 1);
            operand[end - start - 1] = '\0';
        }
        else {
            printf("Error: No operand found in line: %s\n", line);
        }
    }
}



int getRegisterNumber(char* operand) {
    if (!isValidRegister(operand)) {
        printf("Error: Attempted to get the number of a non-register operand: %s\n", operand);
        return -1;
    }

    return atoi(operand + 1);
}

int getSymbolValue(struct symTable* symHead, char* operand) {
    if (!isSymbol(operand)) {
        printf("Error: Attempted to get the value of a non-symbol operand: %s\n", operand);
        return -1;
    }

    struct symTable* symbol = findSymbol(symHead, operand);
    if (symbol == NULL) {
        printf("Error: Undefined symbol: %s\n", operand);
        return -1;
    }

    return symbol->symValue;
}

int FirstPass(int i, char* argv[], struct symTable** symHead, int* ic, int* dc, struct entryExtern** entryExternHead) {
    int tempIc = IC_INIT_VALUE, tempDc = 0;
    char fileName[MAX_NAME_LENGTH];
    char currentLine[MAX_LINE_LENGTH];
    char label[MAX_NAME_LENGTH];

    memset(currentLine, '\0', MAX_LINE_LENGTH);
    memset(fileName, '\0', MAX_NAME_LENGTH);

    strcpy(fileName, argv[i]);

    FILE* file;
    file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Error: Couldn't open the file %s\n", fileName);
        return -1;
    }

    struct symTable* symTail = *symHead;
    int hasInstruction = 0;
    
    while (fgets(currentLine, MAX_LINE_LENGTH, file)) {
          if (strncmp(currentLine, ".entry", 6) == 0) {
            struct entryExtern* newEntry = (struct entryExtern*)malloc(sizeof(struct entryExtern));
            sscanf(currentLine, ".entry %s", newEntry->name);
            strcpy(newEntry->status, "ENTRY");
            newEntry->nextentryExtern = *entryExternHead;
            *entryExternHead = newEntry;
            continue;
        }
                if (strncmp(currentLine, ".extern", 7) == 0) {
            struct entryExtern* newEntry = (struct entryExtern*)malloc(sizeof(struct entryExtern));
            sscanf(currentLine, ".extern %s", newEntry->name);
            strcpy(newEntry->status, "EXTERN");
            newEntry->nextentryExtern = *entryExternHead;
            *entryExternHead = newEntry;
            continue;
        }
        currentLine[strcspn(currentLine, "\n")] = '\0';
        struct symTable* tempSym = (struct symTable*)malloc(sizeof(struct symTable));
        tempSym->nextSym = NULL;
        tempSym->entryExternFlag = NONE;

        int hasLabel = getLabel(currentLine, label);

        if (hasLabel && symbolExists(*symHead, label) == FALSE) {
            char* colonIndex = strchr(currentLine, ':');

            if (colonIndex != NULL) {
                char* substring = colonIndex + 1;
                if (isInstruction(currentLine,1)) {
                      
                    insertToSymTable(tempSym, &symTail, symHead, label, tempIc, CODE, NONE);
                     int size=getInstructionSize(substring, *symHead);
                         
                       tempIc += size;
                 
                }
                else if (isDataOrString(substring) == DATA) {
                    insertToSymTable(tempSym, &symTail, symHead, label, tempIc, DATA, NONE);

                    int size = getDataSize(substring);
                    tempDc += size;
                    tempIc += size;
                }
                else if (isDataOrString(substring) == STRING) {
                    insertToSymTable(tempSym, &symTail, symHead, label, tempIc, STRING, NONE);

                    int size = getStringSize(substring);
                    tempDc += size;
                    tempIc += size;
                }
            }
        }
        else if (!hasLabel || (hasLabel && !hasInstruction && isInstruction(currentLine,1))) {
               
            if (isInstruction(currentLine,1)) {
              int size =getInstructionSize(currentLine, *symHead);
                
                tempIc += size;
                hasInstruction = 1;
            }
        }

        free(tempSym);
    }

    *ic = tempIc - IC_INIT_VALUE;
    *dc = tempDc;
    fclose(file);
    return 0;
}



