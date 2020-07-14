#ifndef ASSEMBLER_ASSEMBERIO_H
#define ASSEMBLER_ASSEMBERIO_H
#include <stdbool.h>
#include "symbolTable.h"
#include "assembler.h"
#include "structs_and_constants.h"

InstrLines *readArmFile(char fileName[]);

void wToBinFile(char fileName[], Output *instr);

#endif //ASSEMBLER_ASSEMBERIO_H
