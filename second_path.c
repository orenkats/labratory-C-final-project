#include "functions.h"
#include "libary_h.h"
#include "global_var.h"


int getRegister(char* operand) {
    if (operand[0] == '@' && operand[1] == 'r') {
        /*The operand is a register*/ 
        return atoi(operand + 2);
    } else {
        /* The operand is not a register*/ 
        return -1;
    }
}



ParsedInstruction parseInstruction(char* line) {
    
    char* substring = strchr(line, ':');
         if (substring == NULL) {
        substring = line;  
    }
  else{
   substring=substring+1;
  }  
    ParsedInstruction parsed;
   
    sscanf(substring, "%s %s %s", parsed.operation, parsed.operand1, parsed.operand2);
    
        clear_spaces(parsed.operand2);
        clear_commas(parsed.operand2);
        clear_spaces(parsed.operation);
        if(isDataOrString(parsed.operation) != DATA){
         clear_commas(parsed.operand1);
          }
         clear_spaces(parsed.operand1);
        clear_commas(parsed.operation);
       

      if(!isValidString(parsed.operation)){
         parsed.operation[0] = '\0'; /* Empty string*/
       }
        if(!isValidString(parsed.operand1) && isDataOrString(parsed.operation) != STRING )
          { 
           printf("the not valid: %s\n",parsed.operand1);
           parsed.operand1[0] = '\0'; /*Empty string*/
             }
         if(!isValidString(parsed.operand2) ||isDataOrString(parsed.operation) == STRING)
            {
             printf("the not valid: %s\n",parsed.operand2);
             parsed.operand2[0]='\0';
             }
  
     return parsed;
}

/* Function to get addressing mode*/
int getAddressingMode(char* operand, struct symTable* symHead,struct entryExtern* entryExternHead) {
    if (operand[0] == '\0') {
        /* Empty operand, indicating no addressing mode */
        return no_addr;
    }
    else if (isdigit(operand[0]) || (operand[0] == '-' && isdigit(operand[1]))) {
        /* Immediate addressing */
        return immd_addr;
    }
    else if (isRegister(operand)) {
        /* Register direct addressing */
        return reg_direct_addr;
    }
    else {
           if(findEntryExternFlag(entryExternHead,  operand)==1){
            return direct_addr;
     }
        /* Direct addressing */
        /* Check if the operand is a label in the symbol table */
        struct symTable* current = symHead;
        while (current != NULL) {
            if (strcmp(current->symName, operand) == 0) {
                return direct_addr;
            }
            current = current->nextSym;
        }
    }
    /* If the operand is not a label in the symbol table or an empty operand, return -1 to indicate an error */
    return no_addr;
}

machine_instruction translateInstruction(ParsedInstruction instruction, struct symTable* symHead,struct entryExtern* entryExternHead) {

    machine_instruction machineCode;

    opcode op = getOpcode(instruction.operation);   

    /* Set up operand modes */
    int srcMode = getAddressingMode(instruction.operand1, symHead,entryExternHead);
    int destMode = getAddressingMode(instruction.operand2, symHead,entryExternHead);

    /* Set up ARE bits */
    int srcARE = getARE(instruction.operand1, symHead,entryExternHead);
    int destARE = getARE(instruction.operand2, symHead,entryExternHead);
     
    /* Clear the instruction */
    
   if (destMode == -1) {
        destMode = srcMode;
        srcMode = -1;
    }
    if (destARE == -1) {
        destARE = srcARE;
        srcARE = -1;
    }
  
    /* Build the instruction for operand1 if exist */
    machineCode.instruction = 0; /* Clear the instruction*/
    
    if (srcARE != -1) {
        machineCode.instruction |= (srcARE & 0x3);           /* Position ARE bits at 0-1*/
    }
    if (destMode != -1) {
        machineCode.instruction |= (destMode & 0x7) << 2;    /* Position destination at 2-4*/
    }
    machineCode.instruction |= (op & 0xF) << 5;          /* Position opcode at 5-8*/
    if (srcMode != -1) {
        machineCode.instruction |= (srcMode & 0x7) << 9; /* Position source at 9 - 11 */
    }
   
    return machineCode;
}


unsigned int getARE(char* operand, struct symTable* symHead, struct entryExtern* entryExternHead) {
    if (operand[0] == '\0') {
        /* Empty operand, indicating no ARE bits */
        return -1;
    }

    struct symTable* symbol = findSymbol(symHead, operand);

    /* Check if the symbol was not found in the table */
    if (symbol == NULL) {
        /* Search in the entryExtern table*/
        return findEntryExternFlag(entryExternHead, operand);
    }
    /* Check if operand is a label that is declared as 'extern' */
    else if (symbol->entryExternFlag == EXTERN) {
        return 1;  /* E bit is set, binary: 01 */
    }
    /* Operand is a label that is defined in the same file */
    else {
        return 2;  /* R bit is set, binary: 10 */
    }
}

data_word getOperandData(char* operand, struct symTable* symHead,struct entryExtern* entryExternHead) {
    data_word dw = { 0, 0, 0 }; /* initialize with ARE=0, operand=0, and data=0*/
    if (operand == NULL || operand[0] == '\0') {
        /*No operand provided*/
        printf("Error: no operand provided\n");
        exit(1);
    }
    else if (isdigit(operand[0])) {
        /* If the operand is a numeric literal, the data is the value of the literal*/
        dw.ARE = 0; /* Numeric literals don't have ARE bits*/
        dw.operand = atoi(operand);
    }
    else if (getRegister(operand) != -1) {
        /* If the operand is a register, the data is the register number*/
        dw.ARE = 0; /* Registers don't have ARE bits*/
        dw.operand = getRegister(operand);
    }
    else {
         
        struct symTable* symbol = findSymbol(symHead, operand);
        if (symbol == NULL) {
            printf("Error: symbol %s not found\n", operand);
            
        }
        else {
            /* If the operand is a symbol, the data is the value of the symbol*/
            dw.ARE = getARE(operand, symHead,entryExternHead);
            dw.operand = symbol->symValue;   
        }
    }

    /* Form the 12-bit data word*/
    dw.data = 0;  /* Initialize to 0*/
    dw.data |= (dw.ARE & 0x3);        /* Position ARE bits at 0-1*/
    dw.data |= (dw.operand & 0x3FF) << 2;  /* Position operand at 2-11*/

    return dw;

}

data_word createDataWord(int operandValue) {

    data_word operand;
    operand.ARE = 0;  /* Assuming A, R, E bits are not needed for direct numerical operands */
    operand.operand = operandValue;

    /* Since operandValue can be negative, we need to represent it using 12 bits (two's complement)*/
    operand.data = operand.operand & 0xFFF; /* Mask the 12 least significant bits*/

    return operand;
}

void encodeData(data_word dw, char* output) {
    /* The base64 encoding table*/
    char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    unsigned int value = dw.data;  /* The full data word*/
    
    /* Encode the first 6 bits*/
    output[0] = base64_chars[(value >> 6) & 0x3F];

    /* Encode the second 6 bits*/
    output[1] = base64_chars[value & 0x3F];

    /* Null-terminate the output string*/
    output[2] = '\0';
}

void encodeBase64(FILE* obFile, ParsedInstruction instruction, machine_instruction input, data_word operand1, data_word operand2) {
    /* The base64 encoding table*/
    char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
     unsigned int value = input.instruction;
    char output[3];
  
    if (value != -1) {
    output[0] = base64_chars[(value >> 6) & 0x3F];
    output[1] = base64_chars[value & 0x3F];
    output[2] = '\0';

    /* Write the base64 encoded machine instruction to the file */
    fprintf(obFile, "%s\n", output);
   }

    /* Create separate arrays for the operands*/
    char operand1Base64[3] = "", operand2Base64[3] = "";
    /*check if both of operand*/
     if(operand1.data != 0){
    /* If both operands are registers, encode them into a single word*/
    if (operand1.ARE == 0 && operand2.ARE == 0 ) {
        
        /* Combine the operands into a single data word*/
       data_word reg1,reg2;
        data_word combined;
        reg1.operand = getRegister(instruction.operand1);
        reg2.operand = getRegister(instruction.operand2);
       
        combined.ARE = 0;  /* No A, R, or E for registers*/
        combined.operand = (reg2.operand << 2) | (reg1.operand << 7);
       combined.data = combined.operand | combined.ARE;
        
        /*Encode the combined data word*/
        encodeData(combined, operand1Base64);
        fprintf(obFile, "%s\n", operand1Base64);
    }
    else {
        /* If only one or none of the operands are registers, encode them separately*/
            if(isNumber(instruction.operand1)){

    encodeIntegerToBase64(atoi(instruction.operand1),obFile);
     }

    else{

        encodeData(operand1, operand1Base64);
        fprintf(obFile, "%s\n", operand1Base64);

         }

         if(operand2.data != 0){

        if (operand2.operand != 0 || operand2.ARE != 0) { /* Checking ARE also, in case operand is zero but ARE is not*/
          
            encodeData(operand2, operand2Base64);
            fprintf(obFile, "%s\n", operand2Base64);
        }
       }
      }
    }
}

  
/*translate to machine code strings and data*/
void encodeIntegerToBase64(int value, FILE* file) {
    const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char encoded[3];
    encoded[0] = base64_chars[(value >> 6) & 0x3F];
    encoded[1] = base64_chars[value & 0x3F];
    encoded[2] = '\0';
    fprintf(file, "%s\n", encoded);
}

void SecondPass(int i, char* argv[], struct symTable* symHead, int* ic, int* dc , struct entryExtern* entryExternHead) {
   FILE* inputFile = fopen(argv[i], "r");
    findEntrySymbols(symHead, entryExternHead, argv[i]);
    char line[MAX_LINE_LENGTH];
    int tempIc = IC_INIT_VALUE, tempDc = 0;
    
    /* Extract the filename without the extension */
    char filename[FILENAME_MAX];
    strncpy(filename, argv[i], FILENAME_MAX - 1);
    filename[FILENAME_MAX - 1] = '\0';

    /* Find the last occurrence of '.' in the filename */
    char* dotPos = strrchr(filename, '.');
    if (dotPos != NULL) {
        *dotPos = '\0';  /* Remove the extension by null-terminating at the dot position */
    }

    /* Append ".ob" to the modified filename */
    strcat(filename, ".ob");

    /* Open the .ob file and write the size of the instruction and data sections */
    FILE* obFile = fopen(filename, "w");
    if (obFile == NULL) {
        printf("Error opening file %s.\n", filename);
        exit(1);
    }
    fprintf(obFile, "%d %d\n", (*ic-*dc),(*dc+1));

    /* Construct the .ext filename without the ".ob" extension */
    char extFilename[FILENAME_MAX];
    strncpy(extFilename, argv[i], FILENAME_MAX - 1);
    extFilename[FILENAME_MAX - 1] = '\0';
    strcat(extFilename, ".ext");

    /* Open the .ext file for writing in text mode */
    FILE* extFile = fopen(extFilename, "wt");
    if (extFile == NULL) {
        printf("Error opening file %s.\n", extFilename);
        exit(1);
    }

    /* Iterate over all the lines in the file */
    while (fgets(line, MAX_LINE_LENGTH, inputFile)) {
     /*ignore .entry\.extern line in file*/
        if (strncmp(line, ".entry", 6) == 0 || strncmp(line, ".extern", 7) == 0)
            continue;
            
        /* Parse the instruction */
        ParsedInstruction instruction = parseInstruction(line);
             
        /* Translate the parsed instruction into machine code */
          
          machine_instruction machineCode = translateInstruction(instruction, symHead,entryExternHead);
       
        /* Prepare the operands for base64 encoding */
        data_word operand1Code;
        data_word operand2Code;

        /* Check if the first operand is not empty and is a valid operand */
        if (strlen(instruction.operand1) > 0 && isValidString(instruction.operand1) ||strlen(instruction.operand1) >0 && isDataOrString(instruction.operation) ==      STRING ) {
          
           writeExtFileEntry(instruction.operand1, tempIc+1 , entryExternHead, extFile);
            tempIc += 2;
            /* Check if the first operand is a register */
            if (isValidRegister(instruction.operand1)) {
               
                operand1Code.ARE = 0;  /* No A, R, or E for registers */
                operand1Code.operand = getRegister(instruction.operand1);
                operand1Code.data = (operand1Code.operand)<<7 | operand1Code.ARE;
                        
                  }
            
           else if (isDataOrString(instruction.operation) == STRING) {

           int i = 0;
         while (instruction.operand1[i] != '\0') {
           char currentChar = instruction.operand1[i];
             /*in the example file string is closed by fancy qoute , and ubuntu recognize it as ? mark so i had to check this as well in order the code will work*/
            if (currentChar != '"' && currentChar != '\u201C' && currentChar != '\u201D' && currentChar != '\uFFFD' || currentChar !='"') {
                          
                            if((int)currentChar>0){
                        int asciiCode = (int)currentChar;
                         tempIc += 1;
                         encodeIntegerToBase64(asciiCode, obFile);
                         }
                    }
          
        i++; /* Move to the next character */
    }/*end while loop*/
   encodeIntegerToBase64(0, obFile);
    
  } /* end of else if statement */

            else if(isDataOrString(instruction.operation) == DATA){
            
            int dataSize = getDataSize(instruction.operand1);
              
              if(dataSize==0 && isNumber(instruction.operand1)){
                    long int SingleNum = strtol(instruction.operand1, NULL, 10);
                   encodeIntegerToBase64(SingleNum,obFile);
               }
               else{
              
              int i=0;
              int numbersCount = 0;  
              while (numbersCount < dataSize) {
                while (instruction.operand1[i] == ' ')
                  i++;
                  char *endptr;
                 int num = (int) strtol(&instruction.operand1[i], &endptr, 10);
                i += endptr - &instruction.operand1[i];
                       
                         tempIc += 1;
                         encodeIntegerToBase64(num, obFile);
                         numbersCount++;
                     
            if (instruction.operand1[i] == ',') {
            i++;  
            } 
       else if (instruction.operand1[i] == '\0') {
          break; 
            } 
              } /*end of while*/
             }
            }/*end of else if statment*/
                  
               else if (isNumber(instruction.operand1)) {
                   createDataWord(atoi(instruction.operand1));
            }
        
            else {
                operand1Code.ARE = getARE(instruction.operand1, symHead,entryExternHead);
                operand1Code.operand = getOperandData(instruction.operand1, symHead,entryExternHead).operand;
                operand1Code.data = (operand1Code.operand)<<2 | operand1Code.ARE; 
            } 
          
        }
        /*if the operand1 not exist*/
        else {
            operand1Code.data = 0; /* Set operand1Code data to 0 */
        }

        /* Check if the second operand is not empty and is a valid operand */
        if (strlen(instruction.operand2) > 0 && isValidString(instruction.operand2)) {
          writeExtFileEntry(instruction.operand1, tempIc + 1, entryExternHead, extFile);
            tempIc += 1;
            /* Check if the second operand is a register */
            if (isValidRegister(instruction.operand2)) {
                    
                operand2Code.ARE = 0;  /* No A, R, or E for registers */
                operand2Code.operand = getRegister(instruction.operand2)<<2 | operand2Code.ARE;
               
            }
            else if (isNumber(instruction.operand2)) {
                 
                operand2Code = createDataWord(instruction.operand2);
            }
            else {
                operand2Code.ARE = getARE(instruction.operand2, symHead,entryExternHead);
                operand2Code.operand = getOperandData(instruction.operand2, symHead,entryExternHead).operand;
               operand2Code.data = (operand2Code.operand << 2) | operand2Code.ARE;
            }    
               
        }
         /*operand2 not exist*/
        else {
            operand2Code.data = 0; /* Set operand2Code data to 0 */
        }

        /* Encode the machine code in base64 */
          
      if(isDataOrString(instruction.operation) ==CODE){
        encodeBase64(obFile, instruction, machineCode, operand1Code, operand2Code);
            }
        
    }/*end of while loop*/

    fclose(obFile);
  fclose(extFile);
  }
 
