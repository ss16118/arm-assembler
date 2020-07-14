#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "ARM.h"
#include "emulator.h"
#include "utility.h"



/**
 * Sets the type of the decoded instruction in the ARM struct to 0, so that
 * it becomes invalid in the next cycle
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 */
void clearDecodedInstr(ARM *ARMMachine) {
  ARMMachine->decodedInstr.type = 0;
}


/**
 * Retrieves the value of one of the four flags in the register CPSR
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param flag: the index of the flag N, Z, C or V
 * @return: the value of the specified flag
 */
bool getFlagInCPSR(ARM * ARMMachine, int flag) {
  return ARMMachine->registers[CPSR_NUM][flag];
}

/**
 * Sets the specified flag in the CPSR register to the given boolean value
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param flag: the index of the flag N, Z, C or V
 * @param value: the new boolean value to be set
 */
void setFlagInCPSR(ARM * ARMMachine, int flag, bool value) {
  ARMMachine->registers[CPSR_NUM][flag] = value;
}



/**
 * Returns the current value in the program counter as an unsigned
 * 32 bit integer
 * @param ARMMachine: the pointer to the ARM struct created int he main method
 * @return: the current value in the program counter
 */
uint32_t getProgramCounter(ARM *ARMMachine) {
  bool PC[INSTRUCTION_SIZE];
  getDataFromRegister(ARMMachine, PC, PROGRAM_COUNTER_NUM);
  return binaryToUnsignedDecimal(PC, INSTRUCTION_SIZE);
}

/**
 * Sets the program counter to the 32 bit integer specified in the input
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param value: the new value in decimal form that is to be stored in the
 * program counter
 */
void setProgramCounter(ARM *ARMMachine, uint32_t value) {
  bool newValue[INSTRUCTION_SIZE];
  decimalToBinary(value, newValue, INSTRUCTION_SIZE);
  saveToRegister(ARMMachine, newValue, PROGRAM_COUNTER_NUM);
}


/**
 * Sets the content of a register to the given value.
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param value: the new content of the register
 * @param registerNum: the number of the register
 */
void saveToRegister(ARM *ARMMachine, bool *value, int registerNum) {
  arrayCopy(0, 0, INSTRUCTION_SIZE, value,
      ARMMachine->registers[registerNum]);
}


/**
 * Retrieves the data stored in the register specified
 * by registerNum
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param result: the array which contains the data retrieved from the register
 * @param registerNum: the number of the register
 */
void getDataFromRegister(ARM *ARMMachine, bool *result,
                         int registerNum) {
  arrayCopy(0, 0, INSTRUCTION_SIZE,
      ARMMachine->registers[registerNum], result);
}


/**
 * Saves a given value to the address specified in the memory. The number of
 * bytes to be stored is specified by the parameter numOfByte.
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param value: the bits that are to be saved into the memory
 * @param address: the address of the memory
 * @param numOfBytes: the number of byte to be stored into the memory
 */
void saveToMemory(ARM * ARMMachine, bool value[], uint16_t address,
    int numOfBytes) {
  for (int i = 0; i < numOfBytes; i++) {
    arrayCopy(i * BYTE_SIZE, 0, BYTE_SIZE, value,
        ARMMachine->memory[address + i]);
  }
}

void saveDataToMemory(ARM * ARMMachine, bool value[], uint16_t address,
    int numOfBytes) {
  bool byte[BYTE_SIZE];
  for (int i = 0; i < numOfBytes; i++) {
    arrayCopy(i * BYTE_SIZE, 0, BYTE_SIZE, value, byte);
    reverseArray(byte, BYTE_SIZE);
    arrayCopy(0, 0, BYTE_SIZE, byte,
              ARMMachine->memory[address + i]);
  }

}


/**
 * Retrieves the data stored in a specified address of the memory.
 * Note that the data retrieved is usually 32 bit. However, each memory address
 * only contains 8 bits. Therefore, the retrieved result should cover 4
 * consecutive memory addresses. Since the file is stored in little endian
 * format, each byte will be reversed before it is stored into the result array.
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 * @param result: the array which contains the fetched data
 * @param address; the address of the memory
 */
void fetchFromMemory(ARM *ARMMachine, bool *result,
                                uint16_t address) {
  bool temp[BYTE_SIZE];
  for (int i = 0; i < 4; i++) {
    arrayCopy(0, 0, BYTE_SIZE, ARMMachine->memory[address + i], temp);
    reverseArray(temp, BYTE_SIZE);
    arrayCopy(0, i * BYTE_SIZE, BYTE_SIZE, temp, result);
  }
}


void getDataFromMemory(ARM *ARMMachine, bool *result, uint16_t address) {
  for (int i = 0; i< 4; i++) {
    arrayCopy(0, i * BYTE_SIZE, BYTE_SIZE, ARMMachine->memory[address + i],
        result);
  }
}
