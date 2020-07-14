//
// Created by rh4618 on 12/06/19.
//
#include "nimatron.h"

/*
 * allocate space for a move variable
 */
static Move *initMove() {
  Move *result = malloc(sizeof(Move));
  if (!result) {
    printf("allocation of move failed");
    exit(EXIT_FAILURE);
  }
  result->stack = 0;
  result->amount = 0;
  return result;
}

/*
 * Function checks whether current stack array is "near endgame"
 * near endgame is defined as there exists at most one stack greater than 1
 * @param int *stackArray     - current stack array stores num of counters in each stack
 * @param int length          - length of stackArray, used for iteration
 */
static bool nearEndGame(int *stackArray, int length) {
  bool nearEnd = false;
  for (int i = 0; i < length; ++i) {
    if (stackArray[i] > 1) {
      if (!nearEnd) {
        nearEnd = true;
      } else {
        return false;
      }
    }
  }
  return true;
}

/*
 * Function calculates move AI should take given the stack states and gamemode
 * mvoes are calculated in adherence to game strategy outlined in report
 * @param int *stackArray     - current stack array represents stack state
 * @param int length          - length of stackArray
 * @return Move *result       - struct containing index of stack to remove from and
 *                              amount of remove from said stack
 */
Move *getMove(int *stackArray, int length, bool isMisere) {
  // Print stackArray ----------------------
  printf("stack : ");
  for (int i = 0; i < length; ++i) {
    printf("%d ", stackArray[i]);
  }
  printf("\n");
  //----------------------------------------

  Move *result = initMove();

  int nimSum = 0;
  for (int i = 0; i < length; ++i) {
    nimSum = nimSum ^ stackArray[i];
  }

  printf("X: %d\n", nimSum);

  if (nimSum == 0) {
    // If current gamemode is not misere
    if (!isMisere) {
      // remove a random amount from a random stack
      while (result->amount == 0) {
        result->stack = randint(length);
        if (stackArray[result->stack - 1] > 0) {
          result->amount = randint(stackArray[result->stack - 1]);
        }
      }
    } else {
      // remove 1 counter from max stack
      int maxInd = 0;
      int maxVal = 0;
      for (int i = 0; i < length; ++i) {
        if (stackArray[i] > maxVal) {
          maxVal = stackArray[i];
          maxInd = i;
        }
      }

      result->stack = maxInd + 1;
      result->amount = 1;
    }
  } else {

    //Classic AI aim to remove last element
    if (!isMisere || !nearEndGame(stackArray, length)) {
      for (int i = 0; i < length; ++i) {
        int minSize = nimSum ^ stackArray[i];
        if (minSize < stackArray[i]) {
          result->stack = i + 1;
          result->amount = stackArray[i] - minSize;
          break;
        }
      }
    } else {
      // Misere AI when nearing endgame
      int maxVal = 0;
      int numOnes = 0;
      for (int i = 0; i < length; ++i) {
        maxVal = max(maxVal, stackArray[i]);
        if (stackArray[i] == 1) {
          numOnes++;
        }
      }

      bool isOdd = (numOnes % 2 == 1);
      bool hasMax = (maxVal > 1);
      for (int i = 0; i < length; ++i) {
        if (stackArray[i] == maxVal) {
          result->stack = i + 1;
          if (isOdd) {  // Take entire stack of max stack
            result->amount = stackArray[i];
          } else if (hasMax) {  // Take all but 1 element from max stack
            result->amount = stackArray[i] - 1;
          } else {
            result->amount = 1;
          }
          break;
        }
      }
    }
  }
  printf("stack: %d amount %d\n", result->stack, result->amount);

  return result;
}
