#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "emulator.h"
#include "utility.h"
#include "emulatorIO.h"

int main(int argc, char **argv) {

  ARM *ARMMachine = (ARM*) malloc(sizeof(ARM));
  if (ARMMachine == NULL) {
    printf("Failed to allocate memory for the struct ARM!\n");
    return EXIT_FAILURE;
  }
  // Initializes the struct by setting the memory which it occupies to zero
  memset(ARMMachine, 0, sizeof(*ARMMachine));
  
  readFile(ARMMachine, argv[1]);

  executeProgram(ARMMachine);

  outputMachineState(ARMMachine);
  
  return EXIT_SUCCESS;
}
