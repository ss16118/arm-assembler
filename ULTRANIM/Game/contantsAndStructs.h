//
// Created by rh4618 on 11/06/19.
//

#ifndef GAME_CONTANTSANDSTRUCTS_H
#define GAME_CONTANTSANDSTRUCTS_H

typedef enum {
    CLASSIC,
    MISERE,
    ULTRANIM
} GameSet;

typedef enum {
    PLAYERONE,
    PLAYERTWO
} Player;

typedef struct {
    int stack, amount;
} Move;

#endif //GAME_CONTANTSANDSTRUCTS_H
