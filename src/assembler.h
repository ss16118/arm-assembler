#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H
#include "symbolTable.h"
#include "ARM.h"
#include "structs_and_constants.h"

SymbolTable * firstPass(InstrLines *instrLines);

void parseSourceCode(Output *output, InstrLines *instrLines,
    SymbolTable *symbolTable);

#endif //ASSEMBLER_ASSEMBLER_H
