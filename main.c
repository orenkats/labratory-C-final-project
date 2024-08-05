#include "libary_h.h"
#include "global_var.h"
#include "functions.h"

int main(int argc, char* argv[]) {
    int i;

    for (i = 1; i < argc; i++) {
        /* Add ".as" extension to the input arguments if missing */
        char tempArg[MAX_LINE_LENGTH];
        strcpy(tempArg, argv[i]);
        strcat(tempArg, ".as");

        int len = strlen(tempArg);

        if (strcmp(tempArg + len - 3, ".as") != 0) {
            printf("Error: File '%s' does not end with '.as', it is not an assembly file\n", argv[i]);
        } else {
            FILE* file = fopen(tempArg, "r");
            if (file != NULL) {
                fclose(file);

                /* Update the argument in argv with the modified one */
                argv[i] = tempArg;

                replaceMacroInFile(argv[i], argv[i]);

                /* Declare variables here to reset them for each iteration */
                struct entryExtern* entryExternHead = NULL;
                struct symTable* symHead = NULL;
                int ic = 0, dc = 0;

                if (FirstPass(i, argv, &symHead, &ic, &dc, &entryExternHead) == 0) {
                    SecondPass(i, argv, symHead, &ic, &dc, entryExternHead);
                } else {
                    printf("Failed to perform the first pass on file: %s\n", argv[i]);
                }

                freeSymbolTable(symHead);
                freeEntryExternTable(entryExternHead);
            } else {
                printf("Error: File '%s' does not exist\n", argv[i]);
            }
        }
    }

    return 0;
}







