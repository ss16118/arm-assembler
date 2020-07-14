#ifndef GAME_SETUP_H
#define GAME_SETUP_H

#include <stdio.h>
#include <stdbool.h>

#include "io.h"
#include "contantsAndStructs.h"
#include "utility.h"

void startGame(GameSet *setting);

void gameConfig(int *stackAmt, int *stackSizeMax);

void AIConfig(int *VsAI, int *AIpos);

#endif //GAME_SETUP_H
