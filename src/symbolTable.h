#ifndef EMULATOR_SYMBOLTABLE_H
#define EMULATOR_SYMBOLTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include "structs_and_constants.h"


void generateSymbolTable(InstrLines *instrLines, SymbolTable *symbolTable);

uint16_t getAddress(char *label,  SymbolTable *symbolTable);

#endif