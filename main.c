#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "gba.h"


#include "images/mountaint.h"
#include "images/mountainm.h"
#include "images/mountainb.h"
#include "images/left.h"
#include "images/right.h"
#include "images/leftjump.h"
#include "images/rightjump.h"
#include "images/squish.h"
#include "images/leftcharge.h"
#include "images/rightcharge.h"
#include "images/mm.h"
#include "images/difficulty.h"
#include "images/end.h"

#include "screens.h"

/* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"

/* TODO: */
// Add any additional states you need for your app. You are not requried to use
// these specific provided states.
enum gba_state {
  START,
  PLAY,
  END
};

int direction;
int speed = 3;
int fly = -10;
int gravity = 1;


int main(void) {
  /* TODO: */
  // Manipulate REG_DISPCNT here to set Mode 3. //

  REG_DISPCNT = MODE3 | BG2_ENABLE;

  // Save current and previous state of button input.
  u32 previousButtons = BUTTONS;
  u32 currentButtons = BUTTONS;

  // Load initial application state
  enum gba_state state = START;

  struct slime player;
  struct slime obj;

  int reset = 0;
  int waittime = 180;

  const unsigned short *images[2][3] = {{left, leftcharge, leftjump}, 
  {right, rightcharge, rightjump}};

  const int sizes[2][3][2] = {{{LEFT_WIDTH, LEFT_HEIGHT}, {LEFTCHARGE_WIDTH, LEFTCHARGE_HEIGHT}, 
  {LEFTJUMP_WIDTH, LEFTJUMP_HEIGHT}}, {{RIGHT_WIDTH, RIGHT_HEIGHT}, {RIGHTCHARGE_WIDTH, RIGHTCHARGE_HEIGHT}, 
  {RIGHTJUMP_WIDTH, RIGHTJUMP_HEIGHT}}};

  const unsigned short *mountains[5] = {mm, mountainbot, mountainmid, mountaintop, end};

  direction = 1;

  while (1) {
    currentButtons = BUTTONS; // Load the current state of the buttons

    /* TODO: */
    // Manipulate the state machine below as needed //
    // NOTE: Call waitForVBlank() before you draw

    switch (state) {
      case START:
        if (reset == 0) {
          obj.x = titlex;
          obj.y = titley;
          obj.xv = titlexv;
          obj.yv = titleyv;
          obj.screen = 0;
          obj.timer = 0;
          obj.falling = 0;
          obj.phase = 0;
          direction = 1;
          waitForVBlank();
          drawFullScreenImageDMA(mm);
          drawImageDMA(obj.y, obj.x, sizes[direction][0][0], sizes[direction][0][1], images[direction][0]);
          reset = 1;
        }

        if (KEY_RELEASED(BUTTON_START, currentButtons, previousButtons)) {
          reset = 0;
          state = PLAY;
        }
        if (KEY_RELEASED(BUTTON_SELECT, currentButtons, previousButtons)) {
          reset = 0;
        }

        if (KEY_RELEASED(BUTTON_A, currentButtons, previousButtons)) {
          changedifficulty();
          waitForVBlank();
          drawFullScreenImageDMA(mm);
          drawImageDMA(obj.y, obj.x, sizes[direction][0][0], sizes[direction][0][1], images[direction][0]);
        }
        
        waitForVBlank();
        if ((int) vBlankCounter == waittime) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][0][0], sizes[direction][0][1], mm);
          drawImageDMA(obj.y, obj.x, sizes[direction][2][0], sizes[direction][2][1], images[direction][2]);
        } else if ((int) vBlankCounter == waittime + 80) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][2][0], sizes[direction][2][1], mm);
          drawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], images[direction][1]);
        } else if ((int) vBlankCounter > waittime + 80 && vBlankCounter % 20 == 0) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], mm);
          obj.phase = 1;
          collide(&obj, &sizes);
          changepos(&obj);
          drawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], images[direction][1]);
          obj.timer += 1;
          if (obj.timer == 10) {
            obj.timer = 0;
            vBlankCounter = 0;
            waittime = 480;
          }
        }
        break;
      case PLAY:
        if (reset == 0) {
          player.x = playx;
          player.y = playy;
          player.falling = 0;
          player.screen = 1;
          player.timer = 10;
          player.xv = 0;
          player.yv = 0;
          player.ophase = 0;
          player.phase = 0;
          player.sliding = 0;
          direction = 1;
          waitForVBlank();
          drawFullScreenImageDMA(mountainbot);
          drawImageDMA(player.y, player.x, sizes[direction][0][0], sizes[direction][0][1], images[direction][player.phase]);
          reset = 1;
        }

        if (KEY_RELEASED(BUTTON_SELECT, currentButtons, previousButtons)) {
          reset = 0;
          state = START;
          break;
        }

        if (KEY_DOWN(BUTTON_RIGHT, currentButtons)) {
          player.xv = speed;
          player.ophase = player.phase;
          player.phase = 1;
          direction = 1;
        } else if (KEY_DOWN(BUTTON_LEFT, currentButtons)) {
          player.xv = -speed;
          player.ophase = player.phase;
          player.phase = 1;
          direction = 0;
        }
        if (KEY_RELEASED(BUTTON_UP, currentButtons, previousButtons) && (player.falling == 0)) {
          player.yv = fly;
          player.ophase = player.phase;
          player.phase = 2;
          player.falling = 1;
        }

        if (gravity == 0 && KEY_DOWN(BUTTON_UP, currentButtons)) {
          player.yv = fly;
          player.ophase = player.phase;
          player.phase = 2;
          player.falling = 1;
        }

        if (KEY_RELEASED(BUTTON_RIGHT, currentButtons, previousButtons) || KEY_RELEASED(BUTTON_LEFT, currentButtons, previousButtons)) {
          player.xv = 0;
          player.ophase = player.phase;
          player.phase = 0;
        }

        waitForVBlank();
        undrawImageDMA(player.y, player.x, sizes[direction][player.ophase][0], sizes[direction][player.ophase][1], mountains[player.screen]);
        int sc = player.screen;
        collide(&player, &sizes);
        undrawImageDMA(player.y, player.x, 10, 10, mountains[player.screen]);
        if (sc != player.screen) {
          if (player.screen == 4) {
            state = END;
            reset = 0;
            break;
          }
          waitForVBlank();
          drawFullScreenImageDMA(mountains[player.screen]);
        }
        changepos(&player);
        drawImageDMA(player.y, player.x, sizes[direction][player.phase][0], sizes[direction][player.phase][1], images[direction][player.phase]);
        char progress[20];
        snprintf(progress, 20, "%d.%d%%", ((((160-player.y) + ((player.screen - 1) * 160)) * 10000) / 480) / 100, ((((160-player.y) + ((player.screen - 1) * 160)) * 10000) / 480) % 100);
        undrawImageDMA(2, 204, 40, 8, mountains[player.screen]);
        drawString(2, 204, progress, MAGENTA);


        if (gravity == 0) {
          player.yv = 0;
        }
        break;
      case END:
        if (reset == 0) {
          obj.x = titlex;
          obj.y = titley;
          obj.xv = titlexv;
          obj.yv = titleyv;
          obj.screen = 0;
          obj.timer = 0;
          obj.falling = 0;
          obj.phase = 0;
          direction = 1;
          waitForVBlank();
          drawFullScreenImageDMA(mountains[4]);
          drawImageDMA(obj.y, obj.x, sizes[direction][0][0], sizes[direction][0][1], images[direction][0]);
          reset = 1;
        }

        if (KEY_RELEASED(BUTTON_SELECT, currentButtons, previousButtons)) {
          reset = 0;
          state = START;
        }
        
        waitForVBlank();
        if ((int) vBlankCounter == waittime) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][0][0], sizes[direction][0][1], mountains[4]);
          drawImageDMA(obj.y, obj.x, sizes[direction][2][0], sizes[direction][2][1], images[direction][2]);
        } else if ((int) vBlankCounter == waittime + 80) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][2][0], sizes[direction][2][1], mountains[4]);
          drawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], images[direction][1]);
        } else if ((int) vBlankCounter > waittime + 80 && vBlankCounter % 20 == 0) {
          undrawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], mountains[4]);
          obj.phase = 1;
          collide(&obj, &sizes);
          changepos(&obj);
          drawImageDMA(obj.y, obj.x, sizes[direction][1][0], sizes[direction][1][1], images[direction][1]);
          obj.timer += 1;
          if (obj.timer == 10) {
            obj.timer = 0;
            vBlankCounter = 0;
            waittime = 480;
          }
        }
        break;
    }

    previousButtons = currentButtons; // Store the current state of the buttons
  }
  return 0;
}


void changepos(struct slime* s) {
  s->x += s->xv;
  s->y += s->yv;
  if (s->screen != 0 && s->yv < 2 * gravity) {
    s->yv += gravity;
  }
}

void changedifficulty(void) {
  u32 pb2 = BUTTONS;
  u32 cb2 = BUTTONS;
  int i = 1;
  waitForVBlank();
  drawFullScreenImageDMA(difficulty);
  while(1) {
    cb2 = BUTTONS;
    if (i == 0) {
      waitForVBlank();
      undrawImageDMA(40, 50, 150, 120, difficulty);
      drawRectDMA(45, 55, 130, 2, RED);
      drawRectDMA(45, 55, 2, 23, RED);
      drawRectDMA(68, 55, 130, 2, RED);
      drawRectDMA(45, 183, 2, 24, RED);
      speed = 2;
      fly = -6;
      gravity = 0;
    } else if (i == 1) {
      waitForVBlank();
      undrawImageDMA(40, 50, 150, 120, difficulty);
      drawRectDMA(70, 55, 130, 2, RED);
      drawRectDMA(70, 55, 2, 25, RED);
      drawRectDMA(95, 55, 130, 2, RED);
      drawRectDMA(70, 183, 2, 25, RED); 
      speed = 3;
      fly = -10;
      gravity = 1;
    } else if (i == 2) {
      waitForVBlank();
      undrawImageDMA(40, 50, 150, 120, difficulty);
      drawRectDMA(97, 55, 130, 2, RED);
      drawRectDMA(97, 55, 2, 25, RED);
      drawRectDMA(122, 55, 130, 2, RED);
      drawRectDMA(97, 183, 2, 25, RED);
      speed = 3;
      fly = -13;
      gravity = 2;
    } else if (i == 3) {
      waitForVBlank();
      undrawImageDMA(40, 50, 150, 120, difficulty);
      drawRectDMA(124, 55, 130, 2, RED);
      drawRectDMA(124, 55, 2, 25, RED);
      drawRectDMA(149, 55, 130, 2, RED);
      drawRectDMA(124, 183, 2, 25, RED);
      speed = 5;
      fly = -13;
      gravity = 3;
    }
    if(KEY_RELEASED(BUTTON_DOWN, cb2, pb2)) {
        if (i < 3) {
          i++;
        }
    }
    if (KEY_RELEASED(BUTTON_UP, cb2, pb2)) {
      if (i > 0) {
        i--;
      }
    }

    if (KEY_RELEASED(BUTTON_START, cb2, pb2)) {
      return;
    }

    pb2 = cb2;
  }
}

void collide(struct slime* s, const int (*size)[2][3][2]) {
  // title screen detection
  if ((s->screen == 0 || s->screen == 4) && s->xv + s->x + *(*(*((*size) + direction) + s->phase) + 0) > 240) {
    s->xv = -s->xv;
    direction = 0;
  } else if ((s->screen == 0 || s->screen == 4)  && s->x + s->xv < 0) {
    s->xv = -s->xv;
    direction = 1;
  }
  //right left detection
  if (direction == 0) {
    if (s->x + s->xv < 0) {
      s->xv = 0;
    } else if (screen[s->screen][(s->y)/10 + 1][((s->x + s->xv)/10) + 1] != 0 || screen[s->screen][((s->y) + *(*(*((*size) + direction) + s->phase) + 1) - 2)/10 + 1][((s->x + s->xv)/10) + 1] != 0) {
      if (screen[s->screen][(s->y)/10 + 1][((s->x + s->xv)/10) + 1] == 2 || screen[s->screen][((s->y) + *(*(*((*size) + direction) + s->phase) + 1) - 2)/10 + 1][((s->x + s->xv)/10) + 1] == 2) {
        s->xv = 1;
        s->falling = 1;
      } else {
        s->xv = 0;
        s->falling = 1;
      }
    }
  } else {
    if (screen[s->screen][(s->y)/10 + 1][((s->x + s->xv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1] != 0 || screen[s->screen][((s->y) + *(*(*((*size) + direction) + s->phase) + 1) - 2)/10 + 1][((s->x + s->xv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1] != 0) {
      if (screen[s->screen][(s->y)/10 + 1][((s->x + s->xv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1] == 2 || screen[s->screen][((s->y) + *(*(*((*size) + direction) + s->phase) + 1) - 2)/10 + 1][((s->x + s->xv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1] == 2) { 
        s->xv = -1;
        s->falling = 1;
      } else {
        s->xv = 0;
        s->falling = 1;
      }
    }
  }

  // up down detection
  if (s->y + s->yv < 0) {
    s->screen += 1;
    s->y = 155;
    return;
  } 

  if ((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0)) >= 160) {
    if(s->screen == 1) {
      s->yv = 0;
    } else {
      s->screen -= 1;
      s->y = 0;
    }
    return;
  }
  
  if (screen[s->screen][(s->y + s->yv)/10 + 1][(s->x)/10 + 1] != 0 || screen[s->screen][(s->y + s->yv)/10 + 1][(s->x + *(*(*((*size) + direction) + s->phase) + 0))/10 + 1] != 0){
    s->yv = 0;
    s->xv /= 2;
  }

  if (screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x) /10 + 1] != 0 || screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x + *(*(*((*size) + direction) + s->phase) + 0)) /10 + 1] != 0) {
    if (screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x) /10 + 1] == 2 || screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x + *(*(*((*size) + direction) + s->phase) + 0)) /10 + 1] == 2) {
      s->xv = 2;
      s->yv = 0;
      s->falling = 0;
      s->ophase = s->phase;
      s->phase = 0;
      s->sliding = 1;
    } else if (screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x) /10 + 1] == 3 || screen[s->screen][((s->y + s->yv + *(*(*((*size) + direction) + s->phase) + 0))/10) + 1][(s->x + *(*(*((*size) + direction) + s->phase) + 0)) /10 + 1] == 3) {
      s->xv = -2;
      s->yv = 0;
      s->falling = 0;
      s->ophase = s->phase;
      s->phase = 0;
      s->sliding = 1;
    } else {
      if (s->sliding == 1) {
        s->xv = 0;
        s->sliding = 0;
      }
      s->yv = 0;
      s->falling = 0;
      s->ophase = s->phase;
      s->phase = 0;
    }
  }

  // waitForVBlank();
  // char test[100];
  // char test2[100];
  // char test3[100];
  // char test4[100];
  // snprintf(test, 100, "screen: %d", s->screen);
  // snprintf(test2, 100, "yindex: %d",  (s->y + s->yv)/10 + 1);
  // snprintf(test3, 100, "xindex: %d", (s->x)/10 + 1);
  // snprintf(test4, 100, "screen[]: %d", screen[s->screen][(s->y + s->yv)/10 + 1][(s->x)/10 + 1]);
  // drawRectDMA(20, 20, 80, 80, BLACK);
  // drawString(20, 20, test, MAGENTA);
  // drawString(40, 20, test2, MAGENTA);
  // drawString(60, 20, test3, MAGENTA);
  // drawString(80, 20, test4, MAGENTA);
}