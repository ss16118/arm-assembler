#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symbolTable.h"
#include "utility.h"
#include "assembler.h"


/**
 * Given a list of lines of assembly code, generates the corresponding symbol
 * table
 * @param: assembly: list of lines of assembly code
 * @param: assemblySize: number of lines of assembly
 * @param: map: the symbol table to be generated
 */
void generateSymbolTable(InstrLines *instrLines, SymbolTable *symbolTable) {
  int tupleElements = 0; //the number of tuples in the map
  int memoryAddress = 0; //the memory address that the label corresponds to
  for (int i = 0; i < instrLines->numOfLines; i++) {
    for (int j = 0; j < MAX_LABEL_LEN; j++) {
      // check for a label
      if (instrLines->sourceCode[i][j] == ':') {
        arrayCopy(0, 0, j, instrLines->sourceCode[i],
            symbolTable->map[tupleElements].label);
        symbolTable->map[tupleElements].label[j] = '\0';
        symbolTable->map[tupleElements].memoryAddress = memoryAddress;
        tupleElements++;

      // check for empty space
      } else if (instrLines->sourceCode[i][j] == ' ') {
          memoryAddress += 4;
          break;
      }
    }
  }
  symbolTable->size = tupleElements;
}

/**
 * Retrieves the corresponding address of a given label in the symbol table
 * @PRE: the label entered has to be in the symbol table
 * @param label: the specified label
 * @param symbolTable: the symbol table which contains the label and its
 * corresponding address
 * @return: the address which should replace the label
 */
uint16_t getAddress(char *label,  SymbolTable *symbolTable) {
  for (int i = 0; i < symbolTable->size; i++) {
    if (strcmp(label, symbolTable->map[i].label) == 0) {
      return symbolTable->map[i].memoryAddress;
    }
  }
  return 0;
}
