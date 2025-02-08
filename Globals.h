#pragma once
#include <stdint.h>
#include "CaveDecoder.h"
#include "MapUtils.h"

#define FRAMES_BEFORE_RESTLESS 64
#define TILES_DISPLAY_BUFFER 1

typedef enum GameScreen { LOGO = 0, TITLE, GAMELOOP, GAMEOVER, GAMEWAIT } GameScreen;
extern GameScreen currentScreen;

extern uint8_t quit;
extern uint8_t scrollFlag;
extern uint8_t keyFlag;
extern int8_t shiftY, shiftX;
extern uint8_t countX, countY;
extern int8_t rockFordY, rockFordX, previousRockFordX, previousRockFordY;
extern uint8_t rockfordShift;
extern uint8_t rockfordAnimFlag;
extern uint8_t currentDirection;
extern uint8_t visibleX;
extern uint8_t visibleY;
extern uint32_t countFrames;
extern uint32_t countFalls;
extern uint8_t lastFrame;
extern uint8_t lastMoveFrame;
extern uint8_t push;
extern uint8_t gameOver;
extern int fall;
extern int which;
extern CaveDecoder caveDecoder;
extern MapUtils *mapUtils;