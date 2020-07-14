#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "symbolTable.h"
#include "assembler.h"
#include "utility.h"


// these functions are only used in this file
// so they are not put into the header file
static void writeOperand(bool *output, char tokens[][10], int tokenCount);
static int split(char dst[][10], char *line, char *delimiters);
static char *getFirstWord(char *line);
static InstructionType getInstructionType(char *mnemonic);

//takes in an instruction as a char and converts it into 32 bits of binary value
static void parseDataProcessingInstr(bool *output, char *instruction) {
  output[31] = output[30] = output[29] = 1;
  output[28] = output[27] = output[26] = 0;
  char tokens[MAX_TOKEN_NUM][MAX_CHAR_PER_TOKEN];
  int tokenCount = split(tokens, instruction, " ,:");

  //write the opcode of the instruction
  if (strcmp(tokens[0], "and") == 0) {
    output[24] = 0;
    output[23] = 0;
    output[22] = 0;
    output[21] = 0;
  } else if (strcmp(tokens[0], "eor") == 0) {
    output[24] = 0;
    output[23] = 0;
    output[22] = 0;
    output[21] = 1;
  } else if (strcmp(tokens[0], "sub") == 0) {
    output[24] = 0;
    output[23] = 0;
    output[22] = 1;
    output[21] = 0;
  } else if (strcmp(tokens[0], "rsb") == 0) {
    output[24] = 0;
    output[23] = 0;
    output[22] = 1;
    output[21] = 1;
  } else if (strcmp(tokens[0], "add") == 0) {
    output[24] = 0;
    output[23] = 1;
    output[22] = 0;
    output[21] = 0;
  } else if (strcmp(tokens[0], "tst") == 0) {
    output[24] = 1;
    output[23] = 0;
    output[22] = 0;
    output[21] = 0;
  } else if (strcmp(tokens[0], "teq") == 0) {
    output[24] = 1;
    output[23] = 0;
    output[22] = 0;
    output[21] = 1;
  } else if (strcmp(tokens[0], "cmp") == 0) {
    output[24] = 1;
    output[23] = 0;
    output[22] = 1;
    output[21] = 0;
  } else if (strcmp(tokens[0], "orr") == 0) {
    output[24] = 1;
    output[23] = 1;
    output[22] = 0;
    output[21] = 0;
  } else if (strcmp(tokens[0], "mov") == 0) {
    output[24] = 1;
    output[23] = 1;
    output[22] = 0;
    output[21] = 1;
  } else if (strcmp(tokens[0], "sub") == 0) {
    output[24] = 0;
    output[23] = 0;
    output[22] = 1;
    output[21] = 0;
  }
  //write Rn of the instruction 
  if (strcmp(tokens[0], "mov") == 0) {
    output[19] = 0;
    output[18] = 0;
    output[17] = 0;
    output[16] = 0;
  } else {
    int num;
    int rnIndex = 2;
    if ((strcmp(tokens[0], "teq")) == 0 || (strcmp(tokens[0], "cmp")) == 0
        || (strcmp(tokens[0], "tst")) == 0) {
      rnIndex = 1;
    }
    num = atoi(tokens[rnIndex] + 1);
    for (int i = 16; i <= 19; i++) {
      output[i] = ((num % 2) == 1);
      num = num / 2;
    }
  }
  if (strcmp(tokens[0], "tst") == 0 || strcmp(tokens[0], "teq") == 0
      || strcmp(tokens[0], "cmp") == 0) {
    output[SET_CONDITION_POS] = 1;
  } else {
    output[SET_CONDITION_POS] = 0;
  }

  //write Rd of the instruction
  if ((strcmp(tokens[0], "teq")) == 0 || (strcmp(tokens[0], "cmp")) == 0
      || (strcmp(tokens[0], "tst")) == 0) {
    for (int i = 12; i <= 15; i++) {
      output[i] = 0;
    }
  } else {
    int num = atoi(tokens[1] + 1);
    for (int i = 12; i <= 15; i++) {
      output[i] = (num % 2) == 1;
      num = num / 2;
    }
  }
  //write I of the instruction
  int rnPosition;
  if (strcmp(tokens[0], "mov") == 0) {
    rnPosition = 2;
  } else {
    rnPosition = 3;
  }
  if ((strcmp(tokens[0], "teq")) == 0 || (strcmp(tokens[0], "cmp")) == 0
      || (strcmp(tokens[0], "tst")) == 0) {
    rnPosition--;
  }
  if (tokenCount > rnPosition) {
    output[IMMEDIATE_POS] = (tokens[rnPosition][0] == '#');
  } else {
    output[IMMEDIATE_POS] = 0;
  }
  //the operand part is complicated so a function is written to handle it
  writeOperand(output, tokens, tokenCount);
}

static void parseMultiplyInstr(bool output[], char *instruction) {
  // sets the check bits for multiply instruction
  // instr[27..22] = [0..0]
  bool checkBits1[6] = {0, 0, 0, 0, 0, 0};
  arrayCopy(0, 22, 6, checkBits1, output);
  // instr[7..4] = [1,0,0,1]
  bool checkBits2[4] = {1, 0, 0, 1};
  arrayCopy(0, 4, 4, checkBits2, output);

  // sets cond to 1110
  bool cond[4] = {0, 1, 1, 1};
  arrayCopy(0, 28, 4, cond, output);

  // sets S to 0
  output[SET_CONDITION_POS] = 0;

  char tokens[MAX_TOKEN_NUM][MAX_CHAR_PER_TOKEN];
  split(tokens, instruction, " ,");

  int Rd, Rn, Rs, Rm;
  Rd = atoi(&tokens[1][1]);
  Rm = atoi(&tokens[2][1]);
  Rs = atoi(&tokens[3][1]);

  bool RdContent[4];
  decimalToBinary(Rd, RdContent, 4);
  arrayCopy(0, 16, 4, RdContent, output);

  bool RmContent[4];
  decimalToBinary(Rm, RmContent, 4);
  arrayCopy(0, 0, 4, RmContent, output);

  bool RsContent[4];
  decimalToBinary(Rs, RsContent, 4);
  arrayCopy(0, 8, 4, RsContent, output);

  // if the mnemonic is "mla"
  if (strcmp(tokens[0], "mla") == 0) {
    // sets the A bit to 1
    output[ACCUMULATE_POS] = 1;

    Rn = atoi(&tokens[4][1]);
    bool RnContent[4];
    decimalToBinary(Rn, RnContent, 4);
    arrayCopy(0, 12, 4, RnContent, output);

  } else {
    // if the mnemonic is "mul"
    // sets the A bit to 0
    output[ACCUMULATE_POS] = 0;
  }
}

/**
 * Converts a constant in the form of a string (array of chars) into binary
 * format (array of booleans) given the length of the desired binary number.
 * @param constant: the constant to be parsed
 * @param binary: the output array which stores the result after conversion
 * @param lenOfBinary: length of the array binary
 */
static void parseConstant(char *constant, bool binary[], int lenOfBinary) {
  // if the constant is in hexadecimal form
  if (constant[1] == 'x') {
    hexToBinary(constant + 2, binary, lenOfBinary);
  } else {
    // if the constant is in decimal form
    decimalToBinary(atoi(constant), binary, lenOfBinary);
  }
}

/**
 * Converts one line of ARM assembly source code into binary after it is
 * categorized as a single data transfer instruction
 * @param output: the output struct defined in the main method
 * @param instruction: the source code as a string
 */
static void parseSingleDataTransferInstr(Output *output, char *instruction) {
  int currentLine = output->currentLine;
  // sets the check bits
  // instr[27, 26] = [0, 1]
  output->outputBinary[currentLine][27] = 0;
  output->outputBinary[currentLine][26] = 1;

  // instr[22, 21] = [0, 0]
  output->outputBinary[currentLine][22] = 0;
  output->outputBinary[currentLine][21] = 0;
  // sets the cond bits to 1110
  bool cond[] = {0, 1, 1, 1};
  arrayCopy(0, 28, 4, cond, output->outputBinary[currentLine]);

  char tokens[MAX_TOKEN_NUM][MAX_CHAR_PER_TOKEN];
  int numOfTokens = split(tokens, instruction, " ,");
  // if it is a "ldr" instruction

  // sets the bits for Rd
  int Rd = atoi(&tokens[1][1]);
  bool RdContent[4];
  decimalToBinary(Rd, RdContent, 4);
  arrayCopy(0, 12, 4, RdContent, output->outputBinary[currentLine]);

  if (strcmp(tokens[0], "ldr") == 0) {
    // sets the L bit to 1
    output->outputBinary[currentLine][LOAD_POS] = 1;

    // if the third token is a constant
    if (tokens[2][0] == '=') {
      bool *constant = (bool *) calloc(INSTRUCTION_SIZE, sizeof(bool));
      if (constant == NULL) {
        printf("A memory allocation error occurred!\n");
      }

      parseConstant(&(tokens[2][1]), constant, INSTRUCTION_SIZE);
      uint32_t constDecimal =
          binaryToUnsignedDecimal(constant, INSTRUCTION_SIZE);
      // if the constant to be loaded is smaller than 0xFF, the instruction will
      // be converted to a "mov" instruction
      if (constDecimal < 0xFF) {
        char newInstr[50];
        tokens[2][0] = '#';
        snprintf(newInstr, sizeof(newInstr), "mov %s, %s",
                 tokens[1], tokens[2]);
        parseDataProcessingInstr(output->outputBinary[currentLine], newInstr);
        return;
      } else {
        // if the constant is larger than 0xFF, direct the assembler to reserve
        // memory with the constant in it
        // sets the Up bit to 1, and immediate offset to 0
        // and the P bit to 1
        output->outputBinary[currentLine][UP_POS] = 1; // U
        output->outputBinary[currentLine][PRE_INDEXING_POS] = 1; // P
        output->outputBinary[currentLine][IMMEDIATE_POS] = 0; // I


        // sets the base register to PC
        bool programCounterBin[] = {1, 1, 1, 1};
        arrayCopy(0, 16, 4, programCounterBin,
                  output->outputBinary[currentLine]);


        // sets offset to be the difference between the address of the current
        // instruction and that of the constant to be loaded
        int offset = (output->outputSize - output->currentLine - 2) * 4;
        bool offsetBin[12];
        decimalToBinary(offset, offsetBin, 12);
        arrayCopy(0, 0, 12, offsetBin, output->outputBinary[currentLine]);

        // copy the constant in binary form into the reserved memory
        arrayCopy(0, 0, INSTRUCTION_SIZE, constant,
                  output->outputBinary[output->outputSize]);
        output->outputSize++;
        free(constant);
        return;
      }
    }
  } else {
    // if it is a "str" instruction
    // sets the L bit to 0
    output->outputBinary[currentLine][LOAD_POS] = 0;
  }

  // if it is pre-indexing
  if (tokens[numOfTokens - 1][strlen(tokens[numOfTokens - 1]) - 1] == ']') {
    // sets the P bit to 1
    output->outputBinary[currentLine][PRE_INDEXING_POS] = 1; // P

    //tokens[numOfTokens - 1][strlen(tokens[numOfTokens - 1]) - 1] == '\0';
    // sets the last character to the sentinel chararacter
    if (numOfTokens > 3) {
      memset(&(tokens[numOfTokens - 1][strlen(tokens[numOfTokens - 1]) - 1]),
             0, 1);
    }
  } else {
    // if it is post-indexing
    // sets the P bit to 0
    output->outputBinary[currentLine][PRE_INDEXING_POS] = 0; // P
  }


  // if the instruction has the format "ldr/str Rd, [Rn] {...}"
  if (tokens[2][0] == '[' && tokens[2][strlen(tokens[2]) - 1] == ']') {
    // sets Up bit to 1, and the immediate bit to 0
    output->outputBinary[currentLine][IMMEDIATE_POS] = 0; // I
    output->outputBinary[currentLine][UP_POS] = 1; // U
    // sets offset to 0
    memset(&output->outputBinary[currentLine][0], 0, 12);

    // sets the Rn bits
    tokens[2][strlen(tokens[2]) - 1] = '\0';
  }
  // sets the Rn bits
  int Rn = atoi(&tokens[2][2]);

  bool RnContent[4];
  decimalToBinary(Rn, RnContent, 4);
  arrayCopy(0, 16, 4, RnContent, output->outputBinary[currentLine]);

  // if the instruction has the format "ldr/str Rd, [Rn, <#expression>]" or
  // "ldr/str Rd, [Rn], <#expression>"
  if (tokens[3][0] == '#') {
    // sets the I bit to 0
    output->outputBinary[currentLine][IMMEDIATE_POS] = 0; // I

    // sets the offset bits
    bool *offset = (bool *) calloc(INSTRUCTION_SIZE, sizeof(bool));
    if (offset == NULL) {
      printf("A memory allocation error occurred!\n");
    }

    // if the constant is negative
    if (tokens[3][1] == '-') {
      // sets the Up bit to 0
      output->outputBinary[currentLine][UP_POS] = 0; // U
      parseConstant(&(tokens[3][2]), offset, 12);
    } else {
      // sets the Up bit to 1
      output->outputBinary[currentLine][UP_POS] = 1; // U
      parseConstant(&(tokens[3][1]), offset, 12);
    }

    arrayCopy(0, 0, 12, offset, output->outputBinary[currentLine]);
    free(offset);
  } else if (numOfTokens == 6 || numOfTokens == 4) {
    // if the instruction has the format "ldr/str Rd, [Rn, {+/-}Rm{, <shift>}]"
    // or "ldr/str Rd, [Rn], {+/-}Rm,{, <shift>}"

    int Rm;
    // sets the immediate bit to 1 and the Up bit to 1
    output->outputBinary[currentLine][IMMEDIATE_POS] = 1; // I

    output->outputBinary[currentLine][4] = 0;
    // checks which type of shit it is and sets the bits corresponding to it

    if (strcmp(tokens[4], "lsl") == 0) { // logical left | 00
      output->outputBinary[currentLine][6] = 0;
      output->outputBinary[currentLine][5] = 0;
    } else if (strcmp(tokens[4], "lsr") == 0) { // logical right | 01
      output->outputBinary[currentLine][6] = 0;
      output->outputBinary[currentLine][5] = 1;
    } else if (strcmp(tokens[4], "asr") == 0) { // arithmetic right | 10
      output->outputBinary[currentLine][6] = 1;
      output->outputBinary[currentLine][5] = 0;
    } else if (strcmp(tokens[4], "ror") == 0) { // rotate right | 11
      output->outputBinary[currentLine][6] = 1;
      output->outputBinary[currentLine][5] = 1;
    }

    // sets the bits for the shift constant
    bool *shiftAmount = (bool *) calloc(INSTRUCTION_SIZE, sizeof(bool));
    if (shiftAmount == NULL) {
      printf("A memory allocation error occurred!\n");
    }
    parseConstant(&(tokens[5][1]), shiftAmount, 5);
    arrayCopy(0, 7, 5, shiftAmount, output->outputBinary[currentLine]);

    free(shiftAmount);

    // sets the bits for Rm
    if (tokens[3][0] == '-') {
      // sets Up bit to 0
      output->outputBinary[currentLine][UP_POS] = 0; // U
      Rm = atoi(&(tokens[3][2]));

    } else if (tokens[3][0] == '+') {

      // sets Up bit to 1
      output->outputBinary[currentLine][UP_POS] = 1; // U
      Rm = atoi(&(tokens[3][2]));

    } else {
      // sets Up bit to 1
      output->outputBinary[currentLine][UP_POS] = 1; // U
      Rm = atoi(&(tokens[3][1]));
    }
    bool RmContent[4];
    decimalToBinary(Rm, RmContent, 4);
    arrayCopy(0, 0, 4, RmContent, output->outputBinary[currentLine]);
  }
}

static void parseSpecialInstr(bool *output, char *instruction) {
  char tokens[MAX_TOKEN_NUM][MAX_CHAR_PER_TOKEN];
  split(tokens, instruction, " ,");

  // if the mnemonic is "lsl"
  if (strcmp(tokens[0], "lsl") == 0) {
    char newInstr[50];
    snprintf(newInstr, sizeof(newInstr), "mov %s, %s,lsl, %s",
             tokens[1], tokens[1], tokens[2]);
    parseDataProcessingInstr(output, newInstr);
  } else {
    // if the mnemonic is "andeq"
    // sets output to 0
    memset(output, 0, INSTRUCTION_SIZE);
  }

}

/**
 * Converts one line of ARM assembly source code into binary after it is
 * categorized as a branch instruction
 * @param output: the output struct defined in the main method
 * @param instruction: the source code as a string
 * @param symbolTable: the symbol table generated earlier
 */
static void parseBranchInstr(Output *output,
                      char *instruction,
                      struct SymbolTable *symbolTable) {
  char tokens[MAX_TOKEN_NUM][MAX_CHAR_PER_TOKEN];
  split(tokens, instruction, " ");
  int currentLine = output->currentLine;

  //set the condition bits
  if (strcmp(tokens[0], "beq") == 0) {
    // 0000
    output->outputBinary[currentLine][31] = 0;
    output->outputBinary[currentLine][30] = 0;
    output->outputBinary[currentLine][29] = 0;
    output->outputBinary[currentLine][28] = 0;
  } else if (strcmp(tokens[0], "bne") == 0) {
    // 0001
    output->outputBinary[currentLine][31] = 0;
    output->outputBinary[currentLine][30] = 0;
    output->outputBinary[currentLine][29] = 0;
    output->outputBinary[currentLine][28] = 1;

  } else if (strcmp(tokens[0], "bge") == 0) {
    // 1010
    output->outputBinary[currentLine][31] = 1;
    output->outputBinary[currentLine][30] = 0;
    output->outputBinary[currentLine][29] = 1;
    output->outputBinary[currentLine][28] = 0;
  } else if (strcmp(tokens[0], "blt") == 0) {
    // 1011
    output->outputBinary[currentLine][31] = 1;
    output->outputBinary[currentLine][30] = 0;
    output->outputBinary[currentLine][29] = 1;
    output->outputBinary[currentLine][28] = 1;
  } else if (strcmp(tokens[0], "bgt") == 0) {
    // 1100
    output->outputBinary[currentLine][31] = 1;
    output->outputBinary[currentLine][30] = 1;
    output->outputBinary[currentLine][29] = 0;
    output->outputBinary[currentLine][28] = 0;
  } else if (strcmp(tokens[0], "ble") == 0) {
    // 1101
    output->outputBinary[currentLine][31] = 1;
    output->outputBinary[currentLine][30] = 1;
    output->outputBinary[currentLine][29] = 0;
    output->outputBinary[currentLine][28] = 1;
  } else {
    // 1110
    output->outputBinary[currentLine][31] = 1;
    output->outputBinary[currentLine][30] = 1;
    output->outputBinary[currentLine][29] = 1;
    output->outputBinary[currentLine][28] = 0;
  }

  //set the next four constant bits
  bool nextFourBits[4] = {0, 1, 0, 1};
  arrayCopy(0, 24, 4, nextFourBits, output->outputBinary[currentLine]);


  //set the offset bits
  int labelAddress = getAddress(tokens[1], symbolTable);
  int currentAddress = output->currentLine * 4;
  int offset = labelAddress - currentAddress - 8;
  bool binaryOffset[26];
  decimalToBinary(offset, binaryOffset, 26);

  //shifts right by 2 and then takes the lower 24 bits
  shiftRight(binaryOffset, 2, 26);
  arrayCopy(0, 0, 24, binaryOffset, output->outputBinary[currentLine]);
}


/**
 * Passes through the source code for the first time and generates the symbol
 * table from it. Returns the pointer to the created SymbolTable .
 * @param instrLines: the  which contains all the instructions in the
 * source code as a two dimensional array
 * @return: the pointer to the generated SymbolTable
 */
SymbolTable *firstPass(InstrLines *instrLines) {
  // make the size of the symbol table half of the number of lines in the
  // source code by default
  SymbolTable *symbolTable = (SymbolTable *) calloc(1, sizeof(SymbolTable));
  if (symbolTable == NULL) {
    printf("Failed to allocate memory for the struct SymbolTable!\n");
    return NULL;
  }

  generateSymbolTable(instrLines, symbolTable);

  return symbolTable;
}

/**
 * Fills in the output array by parsing each line in the source code into
 * a 32 bit instruction (or data) with the given  InstrLines and
 * SymbolTable
 * @param output: the Output struct created in the main method
 * @param instrLines: the string (array of chars) representation of the
 * source code
 * @param symbolTable: a map-like struct that associates each label with an
 * address in memory
 */
void parseSourceCode(Output *output, InstrLines *instrLines,
                     SymbolTable *symbolTable) {
  output->currentLine = 0;
  int currentLine;
  for (int i = 0; i < instrLines->numOfLines; i++) {
    currentLine = output->currentLine;
    char *firstWord = getFirstWord(instrLines->sourceCode[i]);
    switch (getInstructionType(firstWord)) {
      case DATA_PROCESSING:
        parseDataProcessingInstr(output->outputBinary[currentLine],
                                 instrLines->sourceCode[i]);
        break;
      case MULTIPLY:
        parseMultiplyInstr(output->outputBinary[currentLine],
                           instrLines->sourceCode[i]);
        break;
      case SINGLE_DATA_TRANSFER:
        parseSingleDataTransferInstr(output,
                                     instrLines->sourceCode[i]);
        break;
      case BRANCH:
        parseBranchInstr(output,
                         instrLines->sourceCode[i],
                         symbolTable);
        break;
      case SPECIAL:
        parseSpecialInstr(output->outputBinary[currentLine],
                          instrLines->sourceCode[i]);
        break;
      default:
        // if the current line of the source code is a label, reverse the change
        // in currentLine as a label does not need to be parsed as
        // an instruction
        output->currentLine--;
        break;
    }
    output->currentLine++;
    free(firstWord);
  }
}

/**
 * Takes in one line of the source code and identifies what the first word
 * (the mnemonic or the label) is. Outputs the result as an array of chars.
 * @param line: pointer to one line of source code
 * @return: the mnemonic or the label
 */
static char *getFirstWord(char *line) {
  int index = 0;
  while ((*(line + index)) != ' ' && (*(line + index)) != ':'
      && (*(line + index) != '\n')) {
    index += 1;
  }
  char *first = (char *) malloc(index + 1);
  if (first == NULL) {
    printf(" A memory allocation error occurred!\n");
    return NULL;
  }
  strcpy(first, line);
  first[index] = '\0';
  return first;
}

/**
 * Returns the type of instruction as the predefined enum given the mnemonic
 * as an array of chars. If the word inputted matches none of the mnemonics
 * for the instructions, it is then a label, and 0 is returned in that case.
 * @param mnemonic: the mnemonic of the instruction
 * @return: the type of instruction
 */
static InstructionType getInstructionType(char *mnemonic) {
  const char dataProcessing[10][4] =
      {"add\0", "sub\0", "rsb\0", "and\0", "eor\0", "orr\0",
       "mov\0", "tst\0", "teq\0", "cmp\0"};
  const char multiply[2][4] =
      {"mul\0", "mla\0"};
  const char singleDataTransfer[2][4] =
      {"ldr\0", "str\0"};
  const char branch[7][4] =
      {"beq\0", "bne\0", "bge\0", "blt\0", "bgt\0", "ble\0", "b\0"};
  const char special[2][6] =
      {"lsl\0", "andeq\0"};
  for (int i = 0; i < 10; i++) {
    if (strcmp(mnemonic, dataProcessing[i]) == 0) {
      return DATA_PROCESSING;
    }
  }
  for (int i = 0; i < 2; i++) {
    if (strcmp(mnemonic, multiply[i]) == 0) {
      return MULTIPLY;
    }
  }
  for (int i = 0; i < 2; i++) {
    if (strcmp(mnemonic, singleDataTransfer[i]) == 0) {
      return SINGLE_DATA_TRANSFER;
    }
  }
  for (int i = 0; i < 7; i++) {
    if (strcmp(mnemonic, branch[i]) == 0) {
      return BRANCH;
    }
  }
  for (int i = 0; i < 2; i++) {
    if (strcmp(mnemonic, special[i]) == 0) {
      return SPECIAL;
    }
  }
  return 0;
}

/**
 * Splits a string (array of chars) into an array of strings that contains
 * separate words that can be obtained by dividing the input according to the
 * given delimiters. This function returns the length of the array of strings,
 * in other words, the number of words obtained after splitting the string.
 * @param dst: the array that is used to store the words after splitting
 * @param line: the string to be split
 * @param delimiters: an array of chars used to split the string
 * @return: number of tokens
 */
static int split(char dst[][10], char *line, char *delimiters) {
  int tokenCount = 0;
  char *result = NULL;
  result = strtok(line, delimiters);
  while (result != NULL) {
    strcpy(dst[tokenCount++], result);
    result = strtok(NULL, delimiters);
  }
  return tokenCount;
}

/*
 * Given the source code as an array of tokens, and write the bits corresponding
 * to operand2 for data processing instructions.
 */
static void writeOperand(bool *output, char tokens[][10], int tokenCount) {
  int operandIndex;
  if (strcmp(tokens[0], "mov") == 0) {
    operandIndex = 2;
  } else {
    operandIndex = 3;
  }
  if ((strcmp(tokens[0], "teq")) == 0 || (strcmp(tokens[0], "cmp")) == 0
      || (strcmp(tokens[0], "tst")) == 0) {
    operandIndex--;
  }

  if (tokens[operandIndex][0] == '#') {
    //operand 2 is an immediate value
    int rotateLeft = 0;
    int num;
    if (strlen(tokens[operandIndex]) > 3) {
      if (tokens[operandIndex][1] == '0' && tokens[operandIndex][2] == 'x') {
        //in this case, the immdediate value is a hexdecimal number
        num = strtol(&tokens[operandIndex][3], NULL, 16);
      } else {
        num = atoi(&tokens[operandIndex][1]);
      }
    } else {
      num = atoi(&tokens[operandIndex][1]);
    }

    //if num is greater than 2**8-1, the value needs to be shifted
    if (num <= (255)) {
      //255 is 2**8-1
      output[8] = output[9] = output[10] = output[11] = 0;
      //no rotation needed
    } else {
      while (num % 4 == 0) {
        num = num / 4;
        rotateLeft += 2;
      }
      int rotate = (32 - rotateLeft) / 2;
      for (int i = 8; i <= 11; i++) {
        output[i] = rotate % 2;
        rotate = rotate / 2;
      }
    }

    for (int i = 0; i <= 7; i++) {
      output[i] = ((num % 2) == 1);
      num = num / 2;
    }
  } else {
    //operand 2 is a register

    //write register numbers
    int registerNum = atoi(&tokens[operandIndex][1]);
    for (int i = 0; i <= 3; i++) {
      output[i] = registerNum % 2;
      registerNum = registerNum / 2;
    }

    operandIndex++;
    if (operandIndex >= tokenCount) {
      //not shifted
      for (int i = 4; i <= 11; i++) {
        output[i] = 0;
      }

    } else {
      if (strcmp(tokens[operandIndex], "lsr") == 0) {
        output[5] = 1;
        output[6] = 0;
        //shift right
      } else if (strcmp(tokens[operandIndex], "lsl") == 0) {
        output[6] = output[5] = 0;
        //shift left
      } else {
        printf("invalid shift type");
      }
      operandIndex += 1;
      if (tokens[operandIndex][0] == '#') {
        //shifted by a constant amount
        output[4] = 0;
        int num;
        if (strlen(tokens[operandIndex]) > 3) {
          if (tokens[operandIndex][1] == '0'
              && tokens[operandIndex][2] == 'x') {
            // constant value is a hexadecimal
            num = strtol(&tokens[operandIndex][3], NULL, 16);
          } else {
            num = atoi(&tokens[operandIndex][1]);
          }
        } else {
          num = atoi(&tokens[operandIndex][1]);
        }
        for (int i = 7; i <= 11; i++) {
          output[i] = num % 2;
          num = num / 2;
        }

      } else {
        //shifted by a value inside a register
        output[4] = 1;
        output[7] = 0;
        registerNum = atoi(&tokens[operandIndex][1]);
        for (int i = 8; i <= 11; i++) {
          output[i] = registerNum % 2;
          registerNum = registerNum / 2;
        }
      }
    }
  }
}
