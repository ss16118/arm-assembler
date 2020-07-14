#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "ARM.h"

/**
 * Copies the content from the source array to the destination array
 * @param startIndexSource: the index of the first element in the source
 * array that will be copied into the dest array
 * @param startIndexDest: the index in the destination array to which the
 * content of the source array will be copied
 * @param length: the length of the source array to be copied
 * @param source: the source array
 * @param dest: the destination array
 */
void arrayCopy(int startIndexSource, int startIndexDest,
    int length, void *source, void *dest) {
  memcpy(dest + startIndexDest, source + startIndexSource, length);
}

/**
 * Checks if a boolean array contains only zeros. Returns true if it contains
 * only zeroes, false otherwise.
 * @param array: the array to be checked
 * @param sizeOfArray: size of the array
 * @return: a boolean indicating whether the input boolean array contains only
 * zeroes.
 */
bool isEmptyArray(bool *array, int sizeOfArray) {
  bool result = 0;
  // Performs a bit-wise OR on all the elements in the array, and checks if
  // the final result is 1
  for (int i = 0; i < sizeOfArray; i++) {
    result ^= array[i];
  }
  return result == 0;
}


/**
 * Extracts one section of a given boolean array, and store that section in
 * the destination array. The section extracted is denoted by the startIndex,
 * and endIndex of the source array. The section extracted can be represented
 * as source[startIndex..endIndex] (Note that endIndex is included).
 * @param startIndex: the index which denotes the index of the first element
 * that will be extracted in the source array
 * @param endIndex: the index of the last element to be extracted
 * @param source: the array from which the elements will be extracted
 * @param dest: the array that stores the result of the extraction
 */
void extractArraySection(int startIndex, int endIndex, bool source[],
    bool dest[]) {
  register int len = endIndex - startIndex + 1;
  memcpy(dest, source + startIndex, len);
}



/**
 * Converts an array of booleans (binary number) to an array of chars (string)
 * @param array: the array containing 1s and 0s
 * @param result: the char array which stores the string
 * @param sizeOfArray: size of the array
 */
void binaryToString(bool array[], char result[], int sizeOfArray) {
  for (int i = 0; i < sizeOfArray; i++) {
    result[i] = (array[i]) ? '1' : '0';
  }
  result[sizeOfArray] = '\0';
}



/**
 * Reverses the content of an array
 * @param array: the array itself
 * @param sizeOfArray: the size of the array
 */
void reverseArray(bool array[], int sizeOfArray) {
  bool temp;
  for (int i = 0; i < sizeOfArray / 2; i++) {
    temp = array[i];
    array[i] = array[sizeOfArray - 1 - i];
    array[sizeOfArray - 1 - i] = temp;
  }
}


/**
 * Converts an array of booleans to an unsigned integer.
 * @param binary: the array which contains the binary digits.
 * @param sizeOfArray: the size of the binary array.
 * @return: the unsigned integer representation of the binary number
 */
unsigned int binaryToUnsignedDecimal(bool binary[], int sizeOfArray) {
  register int result = 0;
  for (int i = sizeOfArray - 1; i >= 0; i--) {
    result <<= 1;
    result |= binary[i];
  }
  return result;
}

/**
 * Converts an array of booleans to a signed integer
 * @param binary: the array which contains the binary digits
 * @param sizeOfArray: the size of the binary array
 * @return: the signed integer representation of the binary number
 */
int binaryToSignedDecimal(bool binary[], int sizeOfArray) {
  return binaryToUnsignedDecimal(binary, sizeOfArray - 1) -
      (int) (binary[sizeOfArray - 1] << (sizeOfArray - 1));
}


/**
 * Converts a given integer to an array of booleans (the binary number) of
 * a given length.
 * @PRE: numberOfBits <= sizeof(decimalNumber)
 * @param decimalNumber: the decimal number to be converted
 * @param result: the binary number which is represented by an
 * array of booleans
 * @param numberOfBits: the size of the result array
 */
void decimalToBinary(int decimalNumber, bool result[],
    int numberOfBits) {
  register unsigned int mask = 1U << (numberOfBits - 1);
  for (int i = numberOfBits - 1; i >= 0; i--) {
    result[i] = (decimalNumber & mask);
    decimalNumber <<= 1;
  }
}

/**
 * Converts a hexadecimal digit to a boolean array of length 4
 * @param hexDigit: the hex digit given
 * @param nibble: the boolean array that stores the result after conversion
 */
void hexToFourBits(char hexDigit, bool nibble[]) {
  switch (hexDigit) {
    case 'A':
      nibble[3] = nibble[1] = 1;
      nibble[2] = nibble[0] = 0;
      break;
    case 'B':
      nibble[3] = nibble[1] = nibble[0] = 1;
      nibble[2] = 0;
      break;
    case 'C':
      nibble[3] = nibble[2] = 1;
      nibble[1] = nibble[0] = 0;
      break;
    case 'D':
      nibble[3] = nibble[2] = nibble[0] = 1;
      nibble[1] = 0;
      break;
    case 'E':
      nibble[3] = nibble[2] = nibble[1] = 1;
      nibble[0] = 0;
      break;
    case 'F':
      nibble[3] = nibble[2] = nibble[1] = nibble[0] = 1;
      break;
    default:
      decimalToBinary(atoi(&hexDigit), nibble, 4);
      break;
  }
}

/**
 * Converts a hexadecimal number in the form of a string to a binary number in
 * the form of a boolean array.
 * @PRE: strlen(hex) = numberOfBits / 4
 * @param hex: the hexadecimal number
 * @param result: the binary representation of the hex number
 * @param numberOfBits: the length of the result array
 */
void hexToBinary(char *hex, bool result[], int numberOfBits) {
  int len = strlen(hex);
  bool nibble[4];
  memset(nibble, 0, 4);
  for (int i = 0; i < len; i++) {
    hexToFourBits(hex[i], nibble);
    arrayCopy(0, (len - i - 1) * 4, 4, nibble, result);
  }
}



/**
 * Helper function that converts 4 binary bits to a hex char.
 * @param binary: the array which contains the binary digits.
 * @param sizeOfArray: the size of the binary array.
 * @return: the hex representation of the 4 bits as a char
 */
char fourBitsToHex(bool binary[]) {
  reverseArray(binary, 4);
  int decimal = binaryToUnsignedDecimal(binary, 4);
  if (decimal < 10) {
    return (char) (decimal + '0');
  } else {
    return (char) (decimal + 'W');
  }
}


/**
 * Converts an array of booleans to the hex representation as a list of chars.
 * PRE: number of bits in binary must be multiple of 4.
 * @param binary: the array which contains the binary digits.
 * @param sizeOfArray: the size of the binary array.
 * @return: the list of char representation of the hex
 */
void binaryToHex(bool binary[], char hex[], int sizeOfArray) {
  for (int i = 0; i < sizeOfArray; i += 4) {
    bool fourBits[4];
    arrayCopy(i, 0, 4, binary, fourBits);
    hex[i / 4] = fourBitsToHex(fourBits);
  }
  hex[sizeOfArray / 4 + 1] = '\0';
}




/**
 * Shifts all the bits left by specified amount.
 * @param shiftedArray: the array after the shifts.
 * @param shiftAmount: the amount to shift each bit.
 * @param arraySize: size of the array
 * @return: the carry out
 */
bool shiftLeft(bool shiftedArray[], int shiftAmount, int arraySize) {

  bool carryOut = shiftedArray[arraySize - shiftAmount];
  memcpy(shiftedArray + shiftAmount, shiftedArray, arraySize - shiftAmount);
  memset(shiftedArray, 0, sizeof(bool) * shiftAmount);
  return carryOut;
}


/**
 * Shifts all the bits right by specified amount.
 * @param shiftedArray: the array after the shifts.
 * @param shiftAmount: the amount to shift each bit.
 * @param arraySize: size of the array
 * @return: the carry out
 */
bool shiftRight(bool shiftedArray[], int shiftAmount, int arraySize) {
  bool carryOut = shiftedArray[shiftAmount - 1];
  memcpy(shiftedArray, shiftedArray + shiftAmount, arraySize - shiftAmount);
  memset(shiftedArray + arraySize - shiftAmount, 0,
      sizeof(bool) * shiftAmount);
  return carryOut;
}


/**
 * Carries out an arithmetic right shift on the bits.
 * @param shiftedArray: the array after the shifts.
 * @param shiftAmount: the amount to shift each bit.
 * @param arraySize: size of the array
 * @return: the carry out
 */
bool arithmeticShiftRight(bool shiftedArray[], int shiftAmount, int arraySize) {
  int arithmeticBit = shiftedArray[arraySize - 1];

  //shifts all of the bits right, setting leftmost bits to 0
  bool carryOut = shiftRight(shiftedArray, shiftAmount, arraySize);
  memset(shiftedArray + arraySize - shiftAmount,
    arithmeticBit, sizeof(bool) * shiftAmount);
  return carryOut;
}


/**
 * Rotates all the bits right by specified amount.
 * @param rotatedArray: the array after the rotation.
 * @param rotateAmount: the amount to rotate each bit.
 * @param arraySize: size of the array
 * @return: the carry out
 */
bool rotateRight(bool rotatedArray[], int rotateAmount, int arraySize) {

  bool carryOut = rotatedArray[rotateAmount - 1];
  bool copiedArray[arraySize];
  memcpy(copiedArray, rotatedArray + rotateAmount, arraySize - rotateAmount);
  memcpy(copiedArray + arraySize - rotateAmount, rotatedArray, rotateAmount);
  memcpy(rotatedArray, copiedArray, arraySize);

  return carryOut;
}
