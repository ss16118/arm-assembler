//
// Created by rh4618 on 12/06/19.
//
#include "utility.h"

/*
 * function decrements value by 1, cycles if min bound exceeded
 * @param int value       - value to be manipulated
 * @paramt int maxValue   - max bound for value, cycles value
 *                          to max bound if min bound is exceeded
 * @param bool incZero    - true if min bound is zero, else min bound is 1
 */
int decrement(int value, int maxValue, bool incZero) {
  if (incZero) {
    return (value + maxValue) % (maxValue + 1);
  } else {
    return (value - 2 + maxValue) % maxValue + 1;
  }
}

/*
 * function increments value by 1, cycles if max bound exceeded
 * @param int value       - value to be manipulated
 * @paramt int maxValue   - max bound for value, cycles value
 *                          to min bound if max bound is exceeded
 * @param bool incZero    - true if min bound is zero, else min bound is 1
 */
int increment(int value, int maxValue, bool incZero) {
  if (incZero) {
    return (value + 1) % (maxValue + 1);
  } else {
    return value % maxValue + 1;
  }
}

/*
 * function generates a pseudorandom int within a bound,
 * requires prior seeding
 * @param int maxVal        - max bound for random int
 */
int randint(int maxVal) {
  return (rand() % maxVal + 1);
}

/*
 * function returns min value between two int values
 */
int min(int val1, int val2) {
  return (val1 < val2) ? val1 : val2;
}

/*
 * function returns max value between two int values
 */
int max(int val1, int val2) {
  return (val1 > val2) ? val1 : val2;
}
