#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "assembler.h"
#include "assemblerIO.h"
/*
* Function takes 32 system bits (boolean array) at once and outputs
* a 4 int array with each int representing an 8 bit segment of input
* @param: sysBin[] a boolean array of length 8
*/
static int *sysbinToInt(bool sysBin[INSTRUCTION_SIZE]) {

	int *outputBytes = (int *) malloc(BYTE_PER_INSTRUCTION);
	if (outputBytes == NULL) {
		printf("A memory allocation error occurred!\n");
		return NULL;
	}

	for (int cInstr = 0; cInstr < (INSTRUCTION_SIZE / BYTE_SIZE); cInstr++) {
		int sum = 0;
		for (int i = 0; i < BYTE_SIZE; i++) {
			sum += ((int) sysBin[cInstr * BYTE_SIZE + i]) * ((int) pow(2, i));
		}
		outputBytes[cInstr] = sum;
	}
	return outputBytes;
}

/*
* Writes the content of the given struct to be written to a binary
* file, writes values in the order it is given
* @param: String fileName, an Output STRUCT instr that contains
* the relevant instructions in bool array (sysBin) format
*/
void wToBinFile(char fileName[], Output *instr) {
	FILE *newFile = fopen(fileName, "wb");
	if (newFile != NULL) {
		for (int i = 0; i < instr->outputSize; i++) {
			int *instrLine = sysbinToInt(instr->outputBinary[i]);
			for (int lineSeg = 0; lineSeg < BYTE_PER_INSTRUCTION; lineSeg++) {
				fwrite(&instrLine[lineSeg], 1, 1, newFile);
			}
			free(instrLine);
		}
	} else {
		printf("Could not Open File\n");
	}
}

/**
 * Checks if a line is empty (filled with white space characters).
 * @param line: the line to be checked
 * @return: true if the line is empty, false otherwise
 */
static bool isEmptyLine(const char *line) {
	int index = 0;
	while (line[index] != '\0') {
		if (!isspace(line[index])) {
			return false;
		}
		index++;
	}
  return true;
}



/*
* Takes a string filename, calls function to manipulate instruction
* Produces (1) array of characters from loaded instruction file
* (2) array of string dividers to assist in iterating through file
* @param: String fileName
*/
InstrLines *readArmFile(char fileName[]) {
	FILE *assemFile = fopen(fileName, "r");
	if (assemFile != NULL) {
		InstrLines *ARMFile = (InstrLines *) malloc(sizeof(InstrLines));
		if (ARMFile == NULL) {
			printf("Failed to allocate memory for the struct InstrLines!\n");
			return NULL;
		}

		int linePointer = 0;
		char line[MAX_LINE_LEN];
		while (fgets(line, MAX_LINE_LEN, assemFile)) {
			if (!isEmptyLine(line)) {
        for (int i = 0; i < MAX_LINE_LEN; i++) {
          if (line[i] != '\n') {
            ARMFile->sourceCode[linePointer][i] = line[i];
          } else {
            ARMFile->sourceCode[linePointer][i] = '\0';
            break;
          }
        }
        linePointer++;
			}
		}
		ARMFile->numOfLines = linePointer;
		fclose(assemFile);
		return ARMFile;
	} else {
		printf("Failed to open file\n");
		return NULL;
	}
}