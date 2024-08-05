
#include "libary_h.h"
#include "macro.h"
#include "global_var.h"

void freeEntryExternTable(struct entryExtern* head) {
    struct entryExtern* current = head;
    while (current != NULL) {
        struct entryExtern* temp = current;
        current = current->nextentryExtern;
        free(temp);
    }
} 



/*checks if file isOpen*/
int isFileOpen(FILE* file,const char* filename) {
    if (file == NULL) {
          printf("hello world\n");
        printf("Error: Failed to open file, file not exist or failed '%s'\n", filename);
        return 0;
    } else {
        return 1;
    }
}
 /*print file content*/
void printFileContents(const char* filename) {
    FILE* file = fopen(filename, "r");
   if(!isFileOpen(file, filename)) {
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

/*checks if theres no double macro with same name in file*/
int isMacroNameUsed(const MacroDefinition* macroList, const char* name) {
    const MacroDefinition* current = macroList;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}


void freeSymbolTable(struct symTable* symHead) {
    struct symTable* current = symHead;
    struct symTable* next;

    while (current != NULL) {
        next = current->nextSym;
        free(current);
        current = next;
    }
}
void clear_commas(char* str) {
    char* src = str;
    char* dest = str;
    char ch;

    while ((ch = *src++) != '\0') {
        if (!isspace((unsigned char)ch) && ch != ',') {
            *dest++ = ch;
        }
    }

    *dest = '\0';
}

void clear_spaces(char* str) {
 
    char* start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    
    char* end = str + strlen(str) - 1;
    while (end > start && (isspace((unsigned char)*end) || *end == ',')) {
          
        end--;
    }
    *(end + 1) = '\0';
     
    if (str != start) {
        memmove(str, start, (end - start) + 2);
    }

}

int isValidString(const char* str) {
 
    const unsigned char MIN_ASCII = 32;   /* Minimum valid ASCII value (space)*/
    const unsigned char MAX_ASCII = 126;  /*Maximum valid ASCII value (tilde)*/
      size_t i;
    for ( i = 0; i < strlen(str); i++) {
        unsigned char ch = str[i];
        if (ch < MIN_ASCII || ch > MAX_ASCII) {
   
            return 0;  /* Invalid character found*/
        }
    }
  
    return 1;  /* String is valid*/
}

int isLineAlreadyPresent(const char* filename, const char* extension, const char* line) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 0;  /* File doesn't exist, line is not present */
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        if (strstr(buffer, line) != NULL) {
            fclose(file);
            return 1;  /* Line found in the file */
        }
    }

    fclose(file);
    return 0;  /* Line not found in the file */
}
