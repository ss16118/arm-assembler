#ifndef EMULATOR_UTILITY_H
#define EMULATOR_UTILITY_H
#include <stdbool.h>
#include "emulator.h"

void arrayCopy(int startIndexSource, int startIndexDest,
    int length, void *source, void *dest);

void reverseArray(bool array[], int sizeOfArray);

void extractArraySection(int startIndex, int endIndex, bool source[],
    bool dest[]);

void binaryToString(bool binary[], char result[], int sizeOfArray);

bool isEmptyArray(bool *array, int sizeOfArray);

void extendValue(bool oldValue[], bool newValue[], int oldArraySize,
    int newArraySize);

void clearArray(bool array[], int sizeOfArray);

// Number Conversion
unsigned int binaryToUnsignedDecimal(bool binary[], int sizeOfArray);

int binaryToSignedDecimal(bool binary[], int sizeOfArray);

void decimalToBinary(int decimalNumber, bool result[],
    int numberOfBits);

void binaryToHex(bool binary[], char hex[], int sizeOfArray);

void hexToBinary(char *hex, bool binary[], int numOfBits);
// Shift and Rotation
bool shiftLeft(bool shiftedArray[], int shiftAmount, int arraySize);

bool shiftRight(bool shiftedArray[], int shiftAmount, int arraySize);

bool arithmeticShiftRight(bool shiftedArray[], int shiftAmount, int arraySize);

bool rotateRight(bool rotatedArray[], int rotateAmount, int arraySize);

#endif //EMULATOR_UTILITY_H
