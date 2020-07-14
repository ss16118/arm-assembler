#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "utility.h"
#include "symbolTable.h"
#include "assemblerIO.h"
#include "structs_and_constants.h"

int main(int argc, char **argv) {

  InstrLines *instrLines = readArmFile(argv[1]);
  if (instrLines == NULL) {
    return EXIT_FAILURE;
  }


  SymbolTable *symbolTable = firstPass(instrLines);
  if (symbolTable == NULL) {
    return EXIT_FAILURE;
  }


  Output *output = (Output *) calloc(1, sizeof(Output));
  if (output == NULL) {
    printf("Failed to allocate memory for the struct Output!\n");
    return EXIT_FAILURE;
  }

  output->outputSize = instrLines->numOfLines - symbolTable->size;

  parseSourceCode(output, instrLines, symbolTable);

  wToBinFile(argv[2], output);

  free(instrLines);
  free(symbolTable);
  free(output);

  return EXIT_SUCCESS;
}

