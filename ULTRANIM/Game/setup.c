#include <stdio.h>
#include "setup.h"

#define MAX_STACK_AMT 10
#define MAX_STACK_SIZE 10
#define AI_SWITCH 2

/*
 * function prompts the user to select gamemode
 * @param GameSet *setting    - provides pointer to where gamemode should be stored
 */
void startGame(GameSet *setting) {
    /* | 1 | 2 | 3 |
     * 3    - ULTRANIM
     * 2    - Misere
     * 1    - Classic
     */

    // Placeholder, to remove once GUI is implemented
    clearScreen();
    printLogo();
    printf("Select Your Gamemode:\n");
    printf("3\t- ULTRANIM\n2\t- Misere\n1\t- Classic\n");
    //-----------------------------------------------

    *setting = getButtonInput() - 1;
}

/*
 * function prompts user to config AI settings
 * @paramt int *vsAI      - pointer to setting for enable AI
 * @param int *AIpos      - pointer to setting for position of AI 1st or 2nd
 */
void AIConfig(int *vsAI, int *AIpos) {
  int AIon = 0;
  int button = 0;
  while (button != 3) {
    // Prints console prompt for AI enable ---
    clearScreen();
    printf("Would you like to play against AI?\nAI: %s\n", (AIon) ? "enabled" : "disabled");
    //---

    button = getButtonInput();
    if (button == 1) {
      AIon = decrement(AIon, AI_SWITCH - 1, true);
    } else if (button == 2) {
      AIon = increment(AIon, AI_SWITCH - 1, true);
    }
  }
  *vsAI = AIon;

  if (AIon) {
    int order = 1;
    button = 0;

    while (button != 3) {
      // Prints console promt for AI position ---
      clearScreen();
      printf("AI: Enabled\n");
      printf("Would you like to be Player1 or Player2\nPlayer%d\n", order);
      //---

      button = getButtonInput();
      if (button == 1) {
        order = decrement(order, AI_SWITCH, false);
      } else if (button == 2) {
        order = increment(order, AI_SWITCH, false);
      }
    }

    *AIpos = 3 - order;

    printf("AIon %d AIpos %d\n", *vsAI, *AIpos);
  }
}

/*
 * function prompts user to config game parameters, namely number of
 * stacks and max bound for stack size
 * @param int *stackAmt       - pointer to setting amount of stacks
 * @paramt int *stackSizeMax  - pointer to setting max bound of stack size
 */
void gameConfig(int *stackAmt, int *stackSizeMax) {
    int curr = 1;
    int button = 0;

    // set the number of stacks used
    while (button != 3) {
        // Prompt user for number of stacks ---
        clearScreen();
        printf("Please Select the Number of Stacks\n");
        printf("> %d\n", curr);
        // ---

        button = getButtonInput();
        if (button == 1) {
            curr = decrement(curr, MAX_STACK_AMT, false);
        } 
        else if (button == 2) {
            curr = increment(curr, MAX_STACK_AMT, false);
        }
    }
    *stackAmt = curr;


    // set the maximum size of a stack
    curr = 1;
    button = 0;
    while (button != 3) {

        // Prompt user for maximum stack size bound ---
        clearScreen();
        printf("Number of Stacks: %d\n", *stackAmt);
        printf("Please Select the Maximum Counter Limit\n");
        printf("> %d\n", curr);
        // ---

        button = getButtonInput();
        if (button == 1) {
            curr = decrement(curr, MAX_STACK_SIZE, false);
        } 
        else if (button == 2) {
            curr = increment(curr, MAX_STACK_SIZE, false);
        }      
    }
    *stackSizeMax = curr;
}
