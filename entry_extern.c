
#include "functions.h"
#include "libary_h.h"
#include "global_var.h"


/*create entry file*/
void findEntrySymbols(struct symTable* symHead, struct entryExtern* entryExternHead, char* filename) {
    struct symTable* currentSymbol = symHead;
    FILE* outputFile;
    char outputFilename[strlen(filename) + 5];

    /* Remove the original extension from the filename */
    char* dotPos = strrchr(filename, '.');
    if (dotPos != NULL) {
        *dotPos = '\0';
    }

    /* Construct the .ent filename with the ".ent" extension */
    sprintf(outputFilename, "%s.ent", filename);

    /* Open the .ent file for writing in text mode */
    outputFile = fopen(outputFilename, "wt");
    if (outputFile == NULL) {
        printf("Error opening file %s.\n", outputFilename);
        exit(1);
    }

    /* Iterate through the symTable */
    while (currentSymbol != NULL) {
        /* Check if the symbol name is present in the entryExtern linked list with status "ENTRY" */
        struct entryExtern* currentEntry = entryExternHead;
        while (currentEntry != NULL) {
            if (strcmp(currentSymbol->symName, currentEntry->name) == 0 && strcmp(currentEntry->status, "ENTRY") == 0) {
                /* Write the symbol information to the .ent file */
                fprintf(outputFile, "%s %d\n", currentSymbol->symName, currentSymbol->symValue);
            }
            currentEntry = currentEntry->nextentryExtern;
        }
        currentSymbol = currentSymbol->nextSym;
    }

    /* Close the .ent file */
    fclose(outputFile);
}

/*create extern file*/
void writeExtFileEntry(const char* operand, int tempIc, struct entryExtern* entryExternHead, FILE* extFile) {
    struct entryExtern* currentEntry = entryExternHead;
    while (currentEntry != NULL) {
        if (strcmp(operand, currentEntry->name) == 0 && strcmp(currentEntry->status, "EXTERN") == 0) {
            fprintf(extFile, "%s %d\n", operand, tempIc);
            break; /* No need to check further if one match is found*/
        }
        currentEntry = currentEntry->nextentryExtern;
    }
}



int findEntryExternFlag(struct entryExtern* entryExternHead, char* operand) {
    struct entryExtern* currentEntry = entryExternHead;
    while (currentEntry != NULL) {
        if (strcmp(currentEntry->name, operand) == 0) {
            /* Return the appropriate value based on the status in the entryExtern table*/
            if (strcmp(currentEntry->status, "EXTERN") == 0) {
                return 1;  /* E bit is set, binary: 01*/
            } else {
                return 0;  /* R bit is set, binary: 10*/
            }
        }
        currentEntry = currentEntry->nextentryExtern;
    }
    return 0; /* A bit is set, binary: 00 (not found in entryExtern table)*/
}
