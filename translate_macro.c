#include "functions.h"
#include "libary_h.h"
#include "macro.h"

void replaceMacroInFile(const char* filename, const char* argv) {
    /* Open the input file for reading */
    FILE* file = fopen(filename, "r");
    if (!isFileOpen(file, filename)) {
        return;
    }

    /* Create a temporary output file name */
    char tempFilename[MAX_LINE_LENGTH];
    strncpy(tempFilename, filename, MAX_LINE_LENGTH);
    size_t filenameLen = strlen(tempFilename);
    if (filenameLen > 3 && strcmp(tempFilename + filenameLen - 3, ".as") == 0) {
        strncpy(tempFilename + filenameLen - 3, ".am", 3);
    } else {
        strncat(tempFilename, ".am", MAX_LINE_LENGTH - filenameLen - 1);
    }

    /* Open the temporary output file for writing */
    FILE* tempFile = fopen(tempFilename, "w");
    if (!isFileOpen(tempFile, tempFilename)) {
        return;
    }

    /* Initialize macro-related variables */
    MacroDefinition* macroList = NULL;
    MacroDefinition* macroTail = NULL;
    int macroDefined = 0;

    /* Process each line in the input file */
    char macroLine[MAX_LINE_LENGTH];
    while (fgets(macroLine, sizeof(macroLine), file)) {
        if (strncmp(macroLine, "mcro ", 5) == 0) {
            /* Start of a macro definition */
            macroDefined = 1;

            /* Create a new macro definition */
            MacroDefinition* newMacro = (MacroDefinition*)malloc(sizeof(MacroDefinition));
            sscanf(macroLine, "mcro %s", newMacro->name);
            newMacro->name[strlen(newMacro->name)] = '\0';
            newMacro->definition[0] = '\0';
            newMacro->next = NULL;

            /* Check if the macro name is already defined */
            if (isMacroNameUsed(macroList, newMacro->name)) {
                printf("Error: Macro: '%s' is already defined in file:'%s'\n", newMacro->name, filename);
                fclose(file);
                fclose(tempFile);
                remove(tempFilename);
                return;
            }

            /* Add the new macro to the list */
            if (macroList == NULL) {
                macroList = newMacro;
                macroTail = newMacro;
            } else {
                macroTail->next = newMacro;
                macroTail = newMacro;
            }

            /* Read the macro definition */
            while (fgets(macroLine, sizeof(macroLine), file)) {
                if (strcmp(macroLine, "endmcro\n") == 0) {
                    /* End of the macro definition */
                    macroDefined = 0;
                    break;
                }

                /* Append the macro line to the definition */
                strcat(newMacro->definition, macroLine);
            }
        } else if (!macroDefined) {
            /* Replace macros with their definitions */
            int found = 0;
            MacroDefinition* current = macroList;
            while (current != NULL) {
                if (strstr(macroLine, current->name)) {
                    found = 1;
                    fputs(current->definition, tempFile);
                    break;
                }
                current = current->next;
            }
            if (!found) {
                fputs(macroLine, tempFile);
            }
        }
    }

    /* Close the input and output files */
    fclose(file);
    fclose(tempFile);

    strcpy((char*)argv, tempFilename);
}

