#ifndef MACRO_H
#define MACRO_H

#define CODE_ARR_IMG_LENGTH 1200
#define MAX_LINE_LENGTH 100
#define MAX_NAME_LENGTH 100
#define IC_INIT_VALUE 100
#define MAX_MACRO_NAME_LENGTH 50
#define MAX 50
#define MAX_SYMBOL_NAME_LENGTH 15
#define NUM_OPCODES 16
#define REGISTER_PREFIX '@'
#define  IC_MEMORY_START 100
struct MacroDefinition {
    char name[MAX_MACRO_NAME_LENGTH];
    char definition[MAX_LINE_LENGTH];
    struct MacroDefinition* next;
};

typedef struct MacroDefinition MacroDefinition;

#endif
