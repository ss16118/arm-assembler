//
// Created by rh4618 on 12/06/19.
//

#ifndef GAME_UTILITY_H
#define GAME_UTILITY_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

int increment(int value, int maxValue, bool incZero);

int decrement(int value, int maxValue, bool incZero);

int randint(int maxVal);

int min(int val1, int val2);

int max(int val1, int val2);

#endif //GAME_UTILITY_H
