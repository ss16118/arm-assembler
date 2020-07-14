#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>

#include "structs_and_constants.h"
#include "utility.h"
#include "emulatorIO.h"


// takes in a struct and a filename
// stores the content in the file into designated location inside
// the struct (ARMMachine)
void readFile(ARM *ARMMachine, char *fileName) {
    FILE *ptr;
    ptr = fopen(fileName, "rb");

    struct stat st;
    stat(fileName, &st);
    int size = st.st_size;
    // obtain size of file
    

    unsigned char buffer[size];

    fread(buffer, size, 1, ptr);
    //load the data in the file into a buffer
    
    int address = 0;
    bool currentByte[8];
    for (int i = 0; i < size; i++) {
        //use mask to determine the value of each bit
	    for (int j = 0; j < BYTE_SIZE; j++) {
            currentByte[j] = (((int)buffer[i]) & (int) 1 << (7 - j)) != 0;
        }
        
        // store the result into the memory
        saveToMemory(ARMMachine, currentByte, address, 1);
        address++;
    }
    fclose(ptr);
}

// helper function for output
// we need to know the length of the denary value 
// in order to know how many spaces should be added
static int numberOfDigits(int n) {
  int count = n < 0 ? 1 : 0;
  if (n == 0) {
    count = 1;
  } else {
    while (n != 0 ) {
      n = n / 10;
      count++;
    }
  }
  return count;
}


// ouput the state of the emulated ARMMachine using standard output
void outputMachineState(ARM *ARMMachine) {
  printf("Registers:\n");
  char hex[BYTE_SIZE + 1];
  for (int i = 0; i < 13; i++){
    printf("$%d", i);
    printf(i < 10 ? "  :" : " :");
    int decValue = binaryToUnsignedDecimal(ARMMachine->registers[i],
        INSTRUCTION_SIZE);
    printf("%*c", (11 - numberOfDigits(decValue)), ' ');
    printf("%d", decValue);
    reverseArray(ARMMachine->registers[i], INSTRUCTION_SIZE);
    binaryToHex(ARMMachine->registers[i], hex, INSTRUCTION_SIZE);
    printf(" (0x%s)\n", hex);
  }

  printf("PC  :%*c", (11 - numberOfDigits(getProgramCounter(ARMMachine))), ' ');
  printf("%d", getProgramCounter(ARMMachine));
  printf(" (0x%08" PRIx32, getProgramCounter(ARMMachine));
  printf(")\n");
  printf("CPSR:%*c",(11 - numberOfDigits(binaryToUnsignedDecimal
     (ARMMachine->registers[CPSR_NUM], INSTRUCTION_SIZE))),
      ' ');
  printf("%i", binaryToUnsignedDecimal(ARMMachine->registers[CPSR_NUM],
      INSTRUCTION_SIZE));
  reverseArray(ARMMachine->registers[CPSR_NUM], INSTRUCTION_SIZE);
  binaryToHex(ARMMachine->registers[CPSR_NUM], hex, INSTRUCTION_SIZE);
  printf(" (0x%s)\n", hex);

  printf("Non-zero memory:\n");

  char memoryContentHex[BYTE_SIZE + 1];
  bool memoryContent[INSTRUCTION_SIZE];
  for (int i = 0; i < MEMORY_SIZE; i += 4) {
    getDataFromMemory(ARMMachine, memoryContent, i);
    if (binaryToUnsignedDecimal(memoryContent, INSTRUCTION_SIZE) != 0) {
      printf("0x%08x", i);
      binaryToHex(memoryContent, memoryContentHex, INSTRUCTION_SIZE);
      printf(": 0x%s\n", memoryContentHex);
    }
  }
}
