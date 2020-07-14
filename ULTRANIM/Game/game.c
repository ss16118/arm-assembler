#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "game.h"

/*
 * Function for main program loop, calls setup functions before running
 * game loop (playGame), also responsible for freeing settings variable
 * once game loop ends.
 */
void game() {
    while (true) {

        // allocate settings variables ---------------------
        GameSet *setting = malloc(sizeof(GameSet));
        int *stackAmt = malloc(sizeof(int));
        int *stackMaxSize = malloc(sizeof(int));

        int *AISwitch = malloc(sizeof(int));
        int *AIpos = malloc(sizeof(int));

        // initiate game settings config -------------------
        startGame(setting);
        if (*setting != ULTRANIM) {
          AIConfig(AISwitch, AIpos);
        } else {
          *AISwitch = 0;
        }
        gameConfig(stackAmt, stackMaxSize);
        playGame(*setting, *stackAmt, *stackMaxSize, *AISwitch, *AIpos);

        // free all settings variables ---------------------
        free(setting);
        free(stackAmt);
        free(stackMaxSize);
        free(AISwitch);
        free(AIpos);
    }
}

int main() {
    game();
}
