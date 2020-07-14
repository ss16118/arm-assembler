#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "emulator.h"
#include "utility.h"
#include "ARM.h"

static void fetch(ARM *);
static void decode(ARM *);
static void execute(ARM *);

/**
 * Executes one cycle, in which the program fetches one instruction from the
 * memory according to the program counter, decodes the instruction fetched in
 * the previous cycle, and executes the decoded instruction stored in the
 * ARMMachine.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void cycle(ARM *ARMMachine) {
  // If the decoded instruction stored in ARMMachine is not empty,
  // Execute it
  if ((ARMMachine->decodedInstr.type != 0) && ARMMachine->decodedInstr.cond) {
    execute(ARMMachine);
  }

  // Decode the instruction if an instruction is to be decoded in this cycle
  if ((ARMMachine->decode)) {
    decode(ARMMachine);
  }

  // Fetch the next instruction
  fetch(ARMMachine);
}


/**
 * Fetches the 4-byte instruction from memory according to the current program
 * counter. The fetched instruction is stored in the field FETCHED.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void fetch(ARM *ARMMachine) {
  uint32_t PC = getProgramCounter(ARMMachine);
  ARMMachine->addressLastFetched = PC;

  fetchFromMemory(ARMMachine, ARMMachine->fetched, PC);

  // increments the program counter by 4
  setProgramCounter(ARMMachine, PC + 4);

  // sets DECODE to true so that the emulator knows the next instruction is to
  // be decoded
  ARMMachine->decode = true;
}

/**
 * Checks if the first four bits of the instruction (the condition code) which
 * identify under which conditions the operation should be executed.
 * The conditions refer to the state of the top four bits of the CPSR register.
 * If the condition is satisfied, the COND field in the ARMMachine is set to
 * true, otherwise false.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void checkCond(ARM *ARMMachine) {
  bool condBool[4];

  extractArraySection(28, 31, ARMMachine->fetched, condBool);
  // Converts the 4 digits to decimal and compares it to the decimal format
  // of the condition code
  unsigned int cond = binaryToUnsignedDecimal(condBool, 4);
  switch(cond) {
    case 0: // Code: 0000 eq | equal
      if (getFlagInCPSR(ARMMachine, Z)) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 1: // Code: 0001 ne | not equal
      if (!getFlagInCPSR(ARMMachine, Z)) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 10: // Code: 1010 ge | greater or equal
      if (getFlagInCPSR(ARMMachine, N) == getFlagInCPSR(ARMMachine, V)) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 11: // Code 1011 lt | less than
      if (getFlagInCPSR(ARMMachine, N) != getFlagInCPSR(ARMMachine, V)) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 12: // Code: 1100 gt | greater than
      if (!getFlagInCPSR(ARMMachine, Z) &&
      (getFlagInCPSR(ARMMachine, N) == getFlagInCPSR(ARMMachine, V))) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 13: // Code: 1101 le | less than or equal
      if (getFlagInCPSR(ARMMachine, Z) ||
      (getFlagInCPSR(ARMMachine, N) != getFlagInCPSR(ARMMachine, V)) ) {
        ARMMachine->decodedInstr.cond = true;
      } else {
        ARMMachine->decodedInstr.cond = false;
      }
      break;
    case 14: // Code: 1110 al | always
      ARMMachine->decodedInstr.cond = true;
      break;
  }
}

/**
 * Checks if the instruction stored in the FETCHED field is a data processing
 * instruction and updates the related fields in the DECODED struct.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void isDataProcessingInstr(ARM* ARMMachine) {
  // fetched[26,27]
  bool checkBits1[2];
  extractArraySection(26, 27, ARMMachine->fetched, checkBits1);

  // fetched[4..7]
  bool checkBits2[4];
  extractArraySection(4, 7, ARMMachine->fetched, checkBits2);


  // If the check bits match those of the data processing instruction format
  // update all the relevant the fields in decodedInstr
  if (binaryToUnsignedDecimal(checkBits1, 2) == 0 &&
      binaryToUnsignedDecimal(checkBits2, 4) != 9) {
    ARMMachine->decodedInstr.type = DATA_PROCESSING;
    ARMMachine->decodedInstr.I = ARMMachine->fetched[IMMEDIATE_POS];
    ARMMachine->decodedInstr.S = ARMMachine->fetched[SET_CONDITION_POS];
    arrayCopy(21, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.opcode);
    arrayCopy(16, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rn);
    arrayCopy(12, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rd);
    arrayCopy(0, 0, 12, ARMMachine->fetched, ARMMachine->decodedInstr.operand2);
  }


}

/**
 * Checks if the instruction stored in the FETCHED field is a multiply
 * instruction and updates the related fields in the DECODED struct.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void isMultiplyInstr(ARM* ARMMachine) {
  // fetched[22..27]
  bool checkBits1[6];
  extractArraySection(22, 27, ARMMachine->fetched, checkBits1);

  // fetched[4..7]
  bool checkBits2[4];
  extractArraySection(4, 7, ARMMachine->fetched, checkBits2);

  // If the check bits match those of the multiply instruction format
  // update all the relevant the fields in decodedInstr
  if (binaryToUnsignedDecimal(checkBits1, 6) == 0 &&
      binaryToUnsignedDecimal(checkBits2, 4) == 9) {
    ARMMachine->decodedInstr.type = MULTIPLY;
    ARMMachine->decodedInstr.A = ARMMachine->fetched[21];
    ARMMachine->decodedInstr.S = ARMMachine->fetched[SET_CONDITION_POS];
    arrayCopy(16, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rd);
    arrayCopy(12, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rn);
    arrayCopy(8, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rs);
    arrayCopy(0, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rm);
  }
}

/**
 * Checks if the instruction stored in the FETCHED field is a single data
 * transfer instruction and updates the related fields in the DECODED struct.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void isSingleDataTransferInstr(ARM* ARMMachine) {
  // fetched[27, 26]
  bool checkBits1[2];
  extractArraySection(26, 27, ARMMachine->fetched, checkBits1);
  // fetched[22, 21]
  bool checkBits2[2];
  extractArraySection(21, 22, ARMMachine->fetched, checkBits2);

  // If the check bits match those of the single data transfer format
  // update all the relevant the fields in decodedInstr
  if (binaryToUnsignedDecimal(checkBits1, 2) == 1
      && binaryToUnsignedDecimal(checkBits2, 2) == 0) {
    ARMMachine->decodedInstr.type = SINGLE_DATA_TRANSFER;
    ARMMachine->decodedInstr.I = ARMMachine->fetched[IMMEDIATE_POS];
    ARMMachine->decodedInstr.P = ARMMachine->fetched[PRE_INDEXING_POS];
    ARMMachine->decodedInstr.U = ARMMachine->fetched[UP_POS];
    ARMMachine->decodedInstr.L = ARMMachine->fetched[LOAD_POS];
    arrayCopy(16, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rn);
    arrayCopy(12, 0, 4, ARMMachine->fetched, ARMMachine->decodedInstr.Rd);
    arrayCopy(0, 0, 12, ARMMachine->fetched, ARMMachine->decodedInstr.offset);
  }
}

/**
 * Checks if the instruction stored in the FETCHED field is a branch
 * instruction and updates the related fields in the DECODED struct.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void isBranchInstr(ARM* ARMMachine) {
  // fetched[24..27]
  bool checkBits[4];
  extractArraySection(24, 27, ARMMachine->fetched, checkBits);

  // If the check bits match those of the branch instruction format
  // update all the relevant the fields in decodedInstr
  if (binaryToUnsignedDecimal(checkBits, 4) == 10) {
    ARMMachine->decodedInstr.type = BRANCH;
    arrayCopy(0, 0, 24, ARMMachine->fetched, ARMMachine->decodedInstr.offset);
  }
}

/**
 * Decodes the instruction stored at the FETCHED field, and store the decoded
 * instruction in the field DECODED.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void decode(ARM *ARMMachine) {
  // if the fetched instruction is not HALT, decode it, else set the stop flag
  // in the ARM struct to true
  char fetchedHex[BYTE_SIZE + 1];
  binaryToHex(ARMMachine->fetched, fetchedHex, INSTRUCTION_SIZE);
  if (binaryToUnsignedDecimal(ARMMachine->fetched, INSTRUCTION_SIZE) != 0) {
    ARMMachine->decodedInstr.address = getProgramCounter(ARMMachine) - 4;
    checkCond(ARMMachine);
    // If cond is true, decode which type of instruction it is
    if (ARMMachine->decodedInstr.cond) {
      isDataProcessingInstr(ARMMachine);
      isMultiplyInstr(ARMMachine);
      isSingleDataTransferInstr(ARMMachine);
      isBranchInstr(ARMMachine);
    }
  } else {
    ARMMachine->stop = true;
  }
}


/**
 * Executes the decoded data processing instruction
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 */
static void executeDataProcessingInstr(ARM *ARMMachine) {
  uint32_t operand2;
  // if operand2 is an immediate value
  bool carryOut;
  if (ARMMachine->decodedInstr.I) {
    bool extendedValue[INSTRUCTION_SIZE] = {0};

    bool immediateValue[BYTE_SIZE];
    extractArraySection(0, 7,
        ARMMachine->decodedInstr.operand2, immediateValue);
    // extends the immediate value to 32 bits
    arrayCopy(0,0, BYTE_SIZE, immediateValue, extendedValue);

    bool rotate[4];
    extractArraySection(8, 11, ARMMachine->decodedInstr.operand2, rotate);
    int rotateAmount = binaryToUnsignedDecimal(rotate, 4) * 2;

    carryOut = rotateRight(extendedValue, rotateAmount, INSTRUCTION_SIZE);
    operand2 = binaryToUnsignedDecimal(extendedValue, INSTRUCTION_SIZE);
  } else {
    // if operand2 is a register
    bool Rm[4];
    extractArraySection(0, 3, ARMMachine->decodedInstr.operand2, Rm);

    bool RmContent[INSTRUCTION_SIZE];
    getDataFromRegister(ARMMachine, RmContent, binaryToUnsignedDecimal(Rm, 4));

    bool shiftType[2];
    extractArraySection(5, 6, ARMMachine->decodedInstr.operand2, shiftType);

    // if the bit 4 of operand2 is 0, shift by a constant amount
    int shiftAmount;
    if (!ARMMachine->decodedInstr.operand2[4]) {
      bool shift[5];
      extractArraySection(7, 11, ARMMachine->decodedInstr.operand2, shift);
      shiftAmount = binaryToUnsignedDecimal(shift, 5);
    } else {
      // if bit 4 is 1, shift is specified by a register Rs
      bool Rs[4];
      extractArraySection(8, 11, ARMMachine->decodedInstr.operand2, Rs);

      // The bottom byte of Rs specifies the amount to be shifted
      bool RsContent[32];
      getDataFromRegister(ARMMachine, RsContent, binaryToUnsignedDecimal(Rs, 4));
      bool bottomByte[8];
      extractArraySection(0, 7, RsContent, bottomByte);
      shiftAmount = binaryToUnsignedDecimal(bottomByte, BYTE_SIZE);
    }

    // obtain the value of operand2 via the type of shift specified by
    // the shift type
    switch (binaryToUnsignedDecimal(shiftType, 2)) {
      case 0: // 00 logical left (lsl)
        carryOut = shiftLeft(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 1: // 01 logical right (lsr)
        carryOut = shiftRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 2: // 10 arithmetic right (asr)
        carryOut =
            arithmeticShiftRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 3: // 11 rotate right (ror)
        carryOut = rotateRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
    }
    operand2 = binaryToUnsignedDecimal(RmContent, INSTRUCTION_SIZE);
  }


  // checks which operation it is by first converting the opcode into a decimal
  // number and then comparing it to each operation
  int operation = binaryToUnsignedDecimal(ARMMachine->decodedInstr.opcode, 4);

  uint32_t resultDecimal;
  bool result[INSTRUCTION_SIZE];
  bool RnContent[INSTRUCTION_SIZE];
  getDataFromRegister(ARMMachine, RnContent,
                      binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rn, 4));
  uint32_t operand1 = binaryToUnsignedDecimal(RnContent, INSTRUCTION_SIZE);

  bool saveResultToRegister = false;

  switch (operation) {
    case 0: // 0000 and | Rn AND operand2
      resultDecimal = operand1 & operand2;
      saveResultToRegister = true;
      break;
    case 1: // 0001 eor | Rn EOR operand2
      resultDecimal = operand1 ^ operand2;
      saveResultToRegister = true;
      break;
    case 2: // 0010 sub | Rn - operand2
      resultDecimal = operand1 - operand2;
      saveResultToRegister = true;
      carryOut = (resultDecimal > operand1) ? 0 : 1;
      break;
    case 3: // 0011 rsb | operand2 - Rn
      resultDecimal = operand2 - operand1;
      saveResultToRegister = true;
      carryOut = (resultDecimal > operand2) ? 0 : 1;
      break;
    case 4: // 0100 add | Rn + operand2
      resultDecimal = operand1 + operand2;
      saveResultToRegister = true;
      carryOut = (resultDecimal > operand1) ? 0 : 1;
      break;
    case 8: // 1000 tst | as and but result not written
      resultDecimal = operand1 & operand2;
      break;
    case 9: // 1001 teq | as eor, but result is not written
      resultDecimal = operand1 ^ operand2;
      break;
    case 10: // 1010 cmp | as sub, but result is not written
      resultDecimal = operand1 - operand2;
      carryOut = (resultDecimal > operand1) ? 0 : 1;
      break;
    case 12: // 1100 orr | Rn OR operand2
      resultDecimal = operand1 | operand2;
      saveResultToRegister = true;
      break;
    case 13: // 1101 mov | operand2 (Rn is ignored)
      resultDecimal = operand2;
      saveResultToRegister = true;
      break;
  }
  decimalToBinary(resultDecimal, result, INSTRUCTION_SIZE);
  if (saveResultToRegister) {
    saveToRegister(ARMMachine, result,
                   binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd, 4));
  }

  // if the flag for set condition codes is true, set the values of the flags
  // in CPSR
  if (ARMMachine->decodedInstr.S) {
    setFlagInCPSR(ARMMachine, C, carryOut);
    setFlagInCPSR(ARMMachine, Z, (resultDecimal == 0) ? 1 : 0);
    setFlagInCPSR(ARMMachine, N, result[31]);
  }

}

static void checkGPIOState(ARM *ARMMachine, uint32_t address) {
  if (address > MEMORY_SIZE) {
    bool storeToReg = true;
    switch (address) {
      case 0x20200000:
        printf("One GPIO pin from 0 to 9 has been accessed\n");
        break;
      case 0x20200004:
        printf("One GPIO pin from 10 to 19 has been accessed\n");
        break;
      case 0x20200008:
        printf("One GPIO pin from 20 to 29 has been accessed\n");
        break;
      case 0x20200028:
        printf("PIN OFF\n");
        break;
      case 0x2020001c:
        printf("PIN ON\n");
        break;
      default:
        printf("Error: Out of bounds memory access at address 0x%08x\n",
               address);
        storeToReg = false;
        break;
    }


    if (storeToReg && ARMMachine->decodedInstr.L) {
      bool gpioData[INSTRUCTION_SIZE];
      decimalToBinary(address, gpioData, INSTRUCTION_SIZE);
      saveToRegister(ARMMachine, gpioData,
                     binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd, 4));
    }
  }
}


/**
 * Executes the decoded single data transfer instruction
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 */
static void executeSingleDataTransferInstr(ARM *ARMMachine) {
  uint32_t offset;
  // if the I flag is set, offset is interpreted as a shifted register
  if (ARMMachine->decodedInstr.I) {
    // if operand2 is a register
    bool Rm[4];
    extractArraySection(0, 3, ARMMachine->decodedInstr.offset, Rm);

    bool RmContent[INSTRUCTION_SIZE];
    getDataFromRegister(ARMMachine, RmContent, binaryToUnsignedDecimal(Rm, 4));

    bool shiftType[2];
    extractArraySection(5, 6, ARMMachine->decodedInstr.offset, shiftType);

    // if the bit 4 of operand2 is 0, shift by a constant amount
    int shiftAmount;
    if (!ARMMachine->decodedInstr.offset[4]) {
      bool shift[4];
      extractArraySection(7, 11, ARMMachine->decodedInstr.offset, shift);
      shiftAmount = binaryToUnsignedDecimal(shift, 4);
    } else {
      // if bit 4 is 1, shift is specified by a register Rs
      bool Rs[4];
      extractArraySection(8, 11, ARMMachine->decodedInstr.offset, Rs);

      // The bottom byte of Rs specifies the amount to be shifted
      bool RsContent[32];
      getDataFromRegister(ARMMachine, RsContent, binaryToUnsignedDecimal(Rs, 4));
      bool bottomByte[8];
      extractArraySection(0, 7, RsContent, bottomByte);
      shiftAmount = binaryToUnsignedDecimal(bottomByte, BYTE_SIZE);
    }
    // obtain the value of operand2 via the type of shift specified by
    // the shift type
    switch (binaryToUnsignedDecimal(shiftType, 2)) {
      case 0: // 00 logical left (lsl)
        shiftLeft(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 1: // 01 logical right (lsr)
        shiftRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 2: // 10 arithmetic right (asr)
        arithmeticShiftRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
      case 3: // 11 rotate right (ror)
        rotateRight(RmContent, shiftAmount, INSTRUCTION_SIZE);
        break;
    }
    offset = binaryToUnsignedDecimal(RmContent, INSTRUCTION_SIZE);

  } else {
  // if the flag is not set, the offset is interpreted as an unsigned 12 bit
  // immediate offset
    offset = binaryToUnsignedDecimal(ARMMachine->decodedInstr.offset, 12);
  }

  bool RnContent[INSTRUCTION_SIZE];
  getDataFromRegister(ARMMachine, RnContent,
                      binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rn, 4));
  uint32_t address = binaryToUnsignedDecimal(RnContent, INSTRUCTION_SIZE);
  // if the up bit is set, the offset is added to the base register, otherwise
  // it is subtracted from the base register
  uint32_t newAddress = ARMMachine->decodedInstr.U ? address + offset :
      address - offset;

  bool dataFromMemory[INSTRUCTION_SIZE];
  bool dataFromRegister[INSTRUCTION_SIZE];
  // if the P flag is set (pre-indexing), the offset is added/subtracted to
  // the base register before transferring the data
  if (ARMMachine->decodedInstr.P) {
    // if the L bit is set, the word is loaded from memory to the Rd register
    checkGPIOState(ARMMachine, newAddress);
    // If the memory access is out of bounds, print an error message
    if (newAddress > MEMORY_SIZE) {

    } else {
      if (ARMMachine->decodedInstr.L) {
        fetchFromMemory(ARMMachine, dataFromMemory, newAddress);
        saveToRegister(ARMMachine, dataFromMemory,
                       binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd, 4));
      } else {
        // if the L bit is not set, the word is stored into memory
        getDataFromRegister(ARMMachine, dataFromRegister,
                            binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd,
                                                    4));
        saveDataToMemory(ARMMachine, dataFromRegister, newAddress, 4);
      }
    }

  } else {
  // if the flag is not set (post-indexing), the offset is added/ subtracted to
  // the base register after transferring
    // if the L bit is set, the word is loaded from memory to the Rd register

    // If the memory access is out of bounds, print an error message
    if (address > MEMORY_SIZE) {
      checkGPIOState(ARMMachine, address);
    } else {
      if (ARMMachine->decodedInstr.L) {
        fetchFromMemory(ARMMachine, dataFromMemory, address);
        saveToRegister(ARMMachine, dataFromMemory,
                       binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd, 4));
      } else {
        // if the L bit is not set, the word is stored into memory
        getDataFromRegister(ARMMachine, dataFromRegister,
                            binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd,
                                                    4));
        saveDataToMemory(ARMMachine, dataFromRegister, address, 4);
      }
      // Sets the value in the Rn register to be that of the new address in binary
      bool newAddressBinary[INSTRUCTION_SIZE];
      decimalToBinary(newAddress, newAddressBinary, INSTRUCTION_SIZE);
      saveToRegister(ARMMachine, newAddressBinary,
                     binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rn, 4));
    }
  }
}


/**
 * Executes the decoded multiply instruction
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 */
static void executeMultiplyInstr(ARM * ARMMachine) {

  bool RmContent[INSTRUCTION_SIZE];
  getDataFromRegister(ARMMachine, RmContent,
                      binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rm, 4));
  int32_t operand1 = binaryToUnsignedDecimal(RmContent, INSTRUCTION_SIZE);

  bool RsContent[INSTRUCTION_SIZE];
  getDataFromRegister(ARMMachine, RsContent,
                      binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rs, 4));
  int32_t operand2 = binaryToUnsignedDecimal(RsContent, INSTRUCTION_SIZE);

  int32_t resultDecimal;

  // if the bit A is set, the instruction performs a multiply and accumulate
  // Rd := Rm * Rs + Rn
  if (ARMMachine->decodedInstr.A) {
    bool RnContent[INSTRUCTION_SIZE];
    getDataFromRegister(ARMMachine, RnContent,
                        binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rn, 4));
    resultDecimal = operand1 * operand2 +
        binaryToUnsignedDecimal(RnContent, INSTRUCTION_SIZE);
  } else {
  // if the bit is not set, it performs multiply only
  // Rd := Rm * Rs
    resultDecimal = operand1 * operand2;
  }
  bool result[INSTRUCTION_SIZE];
  decimalToBinary(resultDecimal, result, INSTRUCTION_SIZE);
  saveToRegister(ARMMachine, result,
                 binaryToUnsignedDecimal(ARMMachine->decodedInstr.Rd, 4));


  // if the flag for set condition codes is true, set the values of the flags
  // in CPSR
  if (ARMMachine->decodedInstr.S) {
    setFlagInCPSR(ARMMachine, N, result[31]);
    setFlagInCPSR(ARMMachine, Z, (resultDecimal == 0) ? 1 : 0);
  }

}


/**
 * Executes the decoded branch instruction
 * @param ARMMachine: the pointer to the ARM struct created in the main method
 */
static void executeBranchInstr(ARM * ARMMachine) {

  shiftLeft(ARMMachine->decodedInstr.offset, 2, 24);

  int32_t offset = binaryToSignedDecimal(ARMMachine->decodedInstr.offset, 24);
  
  setProgramCounter(ARMMachine, getProgramCounter(ARMMachine) + offset);
  // Sets DECODE to false so that the code fetched in the previous cycle is
  // not to be decoded
  ARMMachine->decode = false;
  ARMMachine->decodedInstr.cond = 0;
}


/**
 * Executes the decoded instruction stored in the ARMMachine
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
static void execute(ARM *ARMMachine) {
  switch (ARMMachine->decodedInstr.type) {
    case DATA_PROCESSING:
      executeDataProcessingInstr(ARMMachine);
      break;
    case MULTIPLY:
      executeMultiplyInstr(ARMMachine);
      break;
    case SINGLE_DATA_TRANSFER:
      executeSingleDataTransferInstr(ARMMachine);
      break;
    case BRANCH:
      executeBranchInstr(ARMMachine);
      break;
  }
}

/**
 * Executes the binary instructions stored in the memory.
 * @param ARMMachine: pointer to the ARM struct created in the main method
 */
void executeProgram(ARM *ARMMachine) {
  while (!ARMMachine->stop) {
    cycle(ARMMachine);
  }
}
