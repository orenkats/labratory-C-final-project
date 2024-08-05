#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "libary_h.h"
#include "macro.h"
#include "global_var.h"

/* Global functions */
int isFileOpen(FILE* file, const char* argv);
void printFileContents(const char* filename);
void freeSymbolTable(struct symTable* symHead);
void clear_spaces(char* str);
int isOpcode_second(char* str);
clear_commas(char* str);
int isValidString(const char* str);
int isLineAlreadyPresent(const char* filename, const char* extension, const char* line);

void freeEntryExternTable(struct entryExtern* head);
/* Macro functions */
void replaceMacroInFile(const char* filename, const char* argv);
int isMacroNameUsed(const MacroDefinition* macroList, const char* name);

/* First pass functions */
int isOpcode(char* str);
struct symTable* findSymbol(struct symTable* symHead, char* symbol);
void insertToSymTable(struct symTable* tempSym, struct symTable** symTail, struct symTable** symHead, char* name, int value, symbol_type type, entry_extern flag);
int getLabel(char* line, char* label);
int isInstruction(char* line, int path);
int getInstructionSize(char* line, struct symTable* symHead);
symbol_type isDataOrString(const char* line);
int getDataSize(char* line);
int getStringSize(char* line);
int symbolExists(struct symTable* symHead, char* label);
void getFirstOperand(char* line, char* operand);
void getSecondOperand(char* line, char* operand);
int isEntry(char* line);
int isExtern(char* line);
int isRegister(char* operand);
int isSymbol(char* operand);
int isNumber(char* operand);
int isValidRegister(char* operand);
int getRegisterNumber(char* operand);
int getSymbolValue(struct symTable* symHead, char* operand);
int FirstPass(int i, char* argv[], struct symTable** symHead, int* ic, int* dc, struct entryExtern** entryExternHead);

/* Second pass functions */
opcode getOpcode(char* opcodeName);
ParsedInstruction parseInstruction(char* line);
int getAddressingMode(char* operand, struct symTable* symHead,struct entryExtern* entryExternHead);
int getRegister(char* operand);
machine_instruction translateInstruction(ParsedInstruction instruction, struct symTable* symHead,struct entryExtern* entryExternHead);
unsigned int getARE(char* operand, struct symTable* symHead, struct entryExtern* entryExternHead);
data_word getOperandData(char* operand, struct symTable* symHead,struct entryExtern* entryExternHead);
data_word createDataWord(int operandStr);
void encodeData(data_word dw, char* output);
void encodeBase64(FILE* obFile, ParsedInstruction instruction, machine_instruction input, data_word operand1, data_word operand2);
void findEntrySymbols(struct symTable* symHead, struct entryExtern* entryExternHead, char* filename);
void writeExtFileEntry(const char* operand, int tempIc, struct entryExtern* entryExternHead, FILE* extFile);
void SecondPass(int i, char* argv[], struct symTable* symHead, int* ic, int* dc,struct entryExtern* entryExternHead );
void encodeIntegerToBase64(int value, FILE* file);
int findEntryExternFlag(struct entryExtern* entryExternHead, char* operand);
#endif  /* FUNCTIONS_H */
