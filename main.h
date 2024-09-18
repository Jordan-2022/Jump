#ifndef MAIN_H
#define MAIN_H

#include "gba.h"

// TODO: Create any necessary structs

/*
* For example, for a Snake game, one could be:
*
* struct snake {
*   int heading;
*   int length;
*   int row;
*   int col;
* };
*
* Example of a struct to hold state machine data:
*
* struct state {
*   int currentState;
*   int nextState;
* };
*
*/

#define titlex 20
#define titley 140
#define titlexv 3
#define titleyv 0
#define playx 117
#define playy 145 

extern int direction;

struct slime {
    int x;
    int y;
    int xv;
    int yv;
    int screen;
    int timer;
    int falling;
    int phase;
    int ophase;
    int sliding;
};

void changepos(struct slime* s);

void changedifficulty(void);

void collide(struct slime* s, const int (*size)[2][3][2]);



#endif
