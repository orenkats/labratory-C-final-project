#include "functions.h"
#include "libary_h.h"
#include "global_var.h"

int isInstruction(char* line, int path) {
    char* tempLine = line;

    while (isspace(*tempLine)) {
        tempLine++;
    }

    /* If the line starts with a label, skip past it */
    char* colonPos = strchr(tempLine, ':');
    if (colonPos != NULL) {
        tempLine = colonPos + 1; /* Move line pointer to character after the colon */
        while (isspace(*tempLine)) {
            tempLine++; /* Skip any whitespace after the label */
        }
    }

    if (path == 1) {
        return isOpcode(tempLine);
    } else {
        return isOpcode_second(tempLine);
    }

    return 0;
}

symbol_type isDataOrString(const char* line) {
    const char* tempLine = line;

    while (isspace(*tempLine)) {
        tempLine++;
    }

    if (strncmp(tempLine, ".data", 5) == 0) {
        return DATA;
    } else if (strncmp(tempLine, ".string", 7) == 0) {
        return STRING;
    } else {
        return CODE;
    }
}

int isEntry(char* line) {
    while (isspace(*line)) {
        line++;
    }
    return strncmp(line, ".entry", 6) == 0;
}

int isExtern(char* line) {
    while (isspace(*line)) {
        line++;
    }
    return strncmp(line, ".extern", 7) == 0;
}

int isNumber(char* operand) {
    int i;

    /* Handle the first character separately to check for the '-' sign */
    if (operand[0] == '-') {
        /* If the operand has only a '-' sign, it's not a valid number */
        if (operand[1] == '\0') {
            return 0;
        }
        /* Skip the first character as it's already checked */
        i = 1;
    } else {
        i = 0;
    }

    for (; operand[i] != '\0'; i++) {
        /* If the current character is not a digit */
        if (!isdigit(operand[i])) {
            return 0;
        }
    }

    return 1;
}

int isRegister(char* operand) {
    clear_spaces(operand);

    return operand[0] == REGISTER_PREFIX;
}

int isSymbol(char* operand) {
    return !isNumber(operand) && !isRegister(operand);
}

int isValidRegister(char* operand) {
    /* The list of valid register names */
    char* valid_registers[] = { "@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7" };

    /* Iterate over the list of valid register names */
    int i;
    for (i = 0; i < 8; i++) {
        /* If the operand matches a valid register name, return true */
        if (strcmp(operand, valid_registers[i]) == 0) {
            return 1;
        }
    }

    /* If no match was found, the operand is not a register */
    return 0;
}

int getStringSize(char* line) {
    int size = 0;
    int inString = 0; /* Flag to track if we are inside a string */

    while (*line != '\0') {
        /* In the example file, the string is closed by a fancy quote, 
           and on some systems, it may be recognized as '?' or '? mark',
           so we need to check for those characters as well
        */
        if (*line == '"' || *line == '\u201C' || *line == '\u201D' || *line == '\uFFFD') {
            if (!inString) {
                inString = 1; /* Start of the string */
                size++; /* Count the opening double quote */
            } else {
                inString = 0; /* End of the string */
                line++; /* Move past the closing double quote */
                break; /* We have found the closing double quote, exit the loop */
            }
        } else if (inString) {
            if (*line == '\\' && *(line + 1) != '\0') {
                /* Handle escape characters, such as '\n', '\t', etc. */
                size++;
                line++; /* Move past the escape character */
            }
            size++; /* Count the characters inside the string */
        }
        line++;
    }

    if (inString) {
        printf("Error: Missing closing double quote in string declaration.\n");
        return -1;
    }

    return size;
}

int getDataSize(char* line) {
    int size = 0;
    char* commaPos = strchr(line, ',');
    while (commaPos != NULL) {
        if (size == 0) {
            size += 2;
        } else {
            size++;
        }
        commaPos = strchr(commaPos + 1, ',');
    }

    return size;
}

/* The array of known opcodes */
char* opcodes[NUM_OPCODES] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"
};

int isOpcode(char* str) {
    char* copy = strdup(str);
    if (copy == NULL) {
        printf("Error: Memory allocation failed.\n");
        return 0;
    }

    clear_spaces(copy);

    char* token = strtok(copy, " ");
    if (token == NULL) {
        free(copy);
        return 0;
    }

    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        if (strcmp(token, opcodes[i]) == 0) {
            free(copy);
            return 1;
        }
    }

    free(copy);
    return 0;
}

int isOpcode_second(char* str) {
    int i;
    for (i = 0; i < NUM_OPCODES; i++) {
        /* If the opcode matches the beginning of the string */
        if (strncmp(str, opcodes[i], strlen(opcodes[i])) == 0) {
            /* Make sure the next character is whitespace or the end of the string */
            char nextChar = str[strlen(opcodes[i])];
            if (isspace(nextChar) || nextChar == '\0') {
                return 1;
            }
        }
    }

    return 0;
}


opcode getOpcode(char* opcodeName) {
    if (strcmp(opcodeName, "mov") == 0) return mov;
    if (strcmp(opcodeName, "cmp") == 0) return cmp;
    if (strcmp(opcodeName, "add") == 0) return add;
    if (strcmp(opcodeName, "sub") == 0) return sub;
    if (strcmp(opcodeName, "not") == 0) return not;
    if (strcmp(opcodeName, "clr") == 0) return clr;
    if (strcmp(opcodeName, "lea") == 0) return lea;
    if (strcmp(opcodeName, "inc") == 0) return inc;
    if (strcmp(opcodeName, "dec") == 0) return dec;
    if (strcmp(opcodeName, "jmp") == 0) return jmp;
    if (strcmp(opcodeName, "bne") == 0) return bne;
    if (strcmp(opcodeName, "red") == 0) return red;
    if (strcmp(opcodeName, "prn") == 0) return prn;
    if (strcmp(opcodeName, "jsr") == 0) return jsr;
    if (strcmp(opcodeName, "rts") == 0) return rts;
    if (strcmp(opcodeName, "stop") == 0) return stop;
    return no_op; /*Return this if the opcodeName does not match any opcode*/
}






