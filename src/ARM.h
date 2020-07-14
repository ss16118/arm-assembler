#ifndef EMULATOR_ARM_H
#define EMULATOR_ARM_H
#include <stdbool.h>
#include <stdint.h>
#include "structs_and_constants.h"

void clearDecodedInstr(ARM *);

bool getFlagInCPSR(ARM *, int flag);

void setFlagInCPSR(ARM *, int flag, bool value);

uint32_t getProgramCounter(ARM *);

void setProgramCounter(ARM *, uint32_t value);

void saveToRegister(ARM *, bool value[], int registerNum);

void getDataFromRegister(ARM *, bool *result, int registerNum);

void saveToMemory(ARM * ARMMachine, bool value[], uint16_t address,
    int numOfByte);

void saveDataToMemory(ARM * ARMMachine, bool value[], uint16_t address,
                  int numOfByte);

void getDataFromMemory(ARM * ARMMachine, bool result[],
    uint16_t address);

void fetchFromMemory(ARM *ARMMachine, bool *result, uint16_t address);


#endif //EMULATOR_ARM_H
