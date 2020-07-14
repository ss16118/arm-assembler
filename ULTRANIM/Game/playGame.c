#include "playGame.h"

//
// Created by rh4618 on 11/06/19.
//

/*
 * function initialises a given array of stacks by allocating each with a
 * random int value within bound
 * @param int stackArray[]        - array of stacks to be set
 * @param int length              - length of stackArray
 * @param int maxSize             - bound for max value of random int generation
 */
static void setStacks(int stackArray[], int length, int maxSize) {
  for (int i = 0; i < length; ++i) {
    stackArray[i] = randint(maxSize);
  }
}

/*
 * function gets opponent of/next player from current player
 * @param Player currentPlayer    - player of current turn
 */
static Player nextPlayer(Player currentPlayer) {
  return (currentPlayer == PLAYERONE) ? PLAYERTWO : PLAYERONE;
}

/*
 * function checks if all stacks are empty, used to determine if game ended
 * @param int *stackArray         - array of stack values
 * @param int length              - length of stackArray
 */
static bool emptyStacks(int *stackArray, int length) {
  for (int i = 0; i < length; ++i) {
    if (stackArray[i] > 0) {
      return false;
    }
  }
  return true;
}

// Terminal Based GUI --------------------------------------------

// prints stacks to terminal with given offset from left of screen
static void printStacks(int *stackArray, int length, int offset) {
  int maxStackVal = 0;

  for (int i = 0; i < length; ++i) {
    maxStackVal = max(maxStackVal, stackArray[i]);
  }


  for (int j = 0; j < maxStackVal; ++j) {
    printf(" ");
    for (int i = 0; i < offset; ++i) {
      printf(" ");
    }
    for (int i = 0; i < length; ++i) {
        printf("%c ", (stackArray[i] >= (maxStackVal - j)) ? '*' : ' ');
    }
    printf("\n");
  }

  printf("%*s|", offset, "");
  for (int i = 0; i < length; ++i) {
    printf("%d|", i + 1);
  }
  printf("\n");
}

// prints the selector icon ^ used during stack selection, offset from left of screen
static void printStackPointer(int *stackArray, int length, int stackNo, int offset) {
  printf("%*s", offset + 1, "");
  for (int i = 1; i <= length; ++i) {
    printf("%s ", (stackNo == i) ? "^" : " ");
  }
  if (stackNo == 0) {
    printf(" all");
  }
  printf("\n");
}

// calculates necessary offset to center element of itemWidth on screen
static int getPrintOffset(int screenWidth, int itemWidth) {
  return (screenWidth - itemWidth) / 2;
}
//------------


/*
 * function is the main Game Loop
 * @param GameSet setting       - Gamemode setting
 * @param int stackAmt          - amount of stacks for this round
 * @param int stackSizeMax      - bound maximum stacksize used in setting stack values
 * @param int AI                - denotes whether AI is enabled
 * @param int AIpos             - AI plays first or second, does not matter if AI is not enabled
 */
void playGame(GameSet setting, int stackAmt, int stackSizeMax, int AI, int AIpos) {
  int printOffset = getPrintOffset(getScreenWidth(), stackAmt * 2 + 1);

  // seed setting, required for random generation of stackSize
  srand(time(NULL));

  bool winCond = (setting == MISERE);
  bool multiRmv = (setting == ULTRANIM);

  // create stack array
  int *stackArray = calloc(stackAmt, sizeof(int));
  setStacks(stackArray, stackAmt, stackSizeMax);

  Player currentPlayer = PLAYERONE;

  bool gameEnded = false;
  int prevStack = 0;
  int prevAmt = 0;

  // each iteration of loop equivalent to one turn
  while (!gameEnded) {
    int stackNo = 1;
    int removeAmt = 1;

    // If AI enabled in appropriate gamemode, calculate move
    if ((setting != ULTRANIM) && AI && currentPlayer == (AIpos - 1)) {
      Move *compMove = getMove(stackArray, stackAmt, winCond);
      stackNo = compMove->stack;
      removeAmt = compMove->amount;

      // -- Internal print, delete once GUI has been implemented
      printf("AI removed %d from stack %d\n", removeAmt, stackNo);
      //---

      free(compMove);

    } else {
      //prompt user for a move

      /*
       * |No DOWN | No UP | SELECT |
       *
       * <Pile Select>
       * SELECT      - Enters Selection
       * No UP       - Reduces current selection, cycles;
       * No DOWN     - Increments current selection, cycle 1, 2... All, 1...
       */

      // user select stack to remove from
      while (true) {

        // -- Print select stack screen
        clearScreen();
        if (AI && (prevStack > 0 || prevAmt > 0)) {
          printf("AI removed %d from stack %d\n", prevAmt, prevStack);
        }
        printStacks(stackArray, stackAmt, printOffset);
        printStackPointer(stackArray, stackAmt, stackNo, printOffset);
        printf("%*splayer%d's turn\n", getPrintOffset(getScreenWidth(), 15), "",
               currentPlayer + 1);
        // ---

        int input = getButtonInput();
        if (input == 1) {
          // decrement stackNo, cycle
          stackNo = decrement(stackNo, stackAmt, multiRmv);
        } else if (input == 2) {
          // increment stackNo, cycle
          stackNo = increment(stackNo, stackAmt, multiRmv);
        } else {
          // Selects current option
          if ((multiRmv && stackNo == 0) || stackArray[stackNo - 1] > 0) {
            break;
          }
        }
      }

      // user select amount to remove from stack
      int maxRemoveAmt = 999;
      if (stackNo == 0) {
        for (int i = 0; i < stackAmt; ++i) {
          if (stackArray[i] > 0) {
            maxRemoveAmt = min(maxRemoveAmt, stackArray[i]);
          }
        }
      } else {
        maxRemoveAmt = stackArray[stackNo - 1];
      }
      while (true) {

        // -- Print select amount to remove screen
        clearScreen();
        printStacks(stackArray, stackAmt, printOffset);
        printStackPointer(stackArray, stackAmt, stackNo, printOffset);
        printf("%*splayer%d's turn\n", getPrintOffset(getScreenWidth(), 15), "",
                currentPlayer + 1);
        printf("max removable amount: %d\n", maxRemoveAmt);
        printf("amount from stack%d: %d\n", stackNo, removeAmt);
        // ---

        int input = getButtonInput();
        if (input == 1) {
          // decrement stackNo, cycle
          removeAmt = decrement(removeAmt, maxRemoveAmt, false);
        } else if (input == 2) {
          // increment stackNo, cycle
          removeAmt = increment(removeAmt, maxRemoveAmt, false);
        } else {
          // Selects current option
          break;
        }
      }
    }

    // Remove counter(s) from stacks
    if (multiRmv && stackNo == 0) {
      for (int i = 0; i < stackAmt; ++i) {
        if (stackArray[i] > 0) {
          stackArray[i] -= removeAmt;
        }
      }
    } else {
      stackArray[stackNo - 1] -= removeAmt;
    }

    // Checks if game has ended
    if (emptyStacks(stackArray, stackAmt)) {
      if (winCond) {
        // Switches winner if gamemode is misere
        currentPlayer = nextPlayer(currentPlayer);
      }
      gameEnded = true;
    } else {
      currentPlayer = nextPlayer(currentPlayer);
    }

    // Stores previous move
    if (AI) {
      prevStack = stackNo;
      prevAmt = removeAmt;
    }
  }

  // -- Print endgame screen
  clearScreen();
  printf("\n*** Player %d wins ***\n\n", currentPlayer + 1);
  getButtonInput();
  // ---

  free(stackArray);
}
