#pragma once
#include <stdint.h>
#include "CaveDecoder.h"
#include "MapUtils.h"

#define FRAMES_BEFORE_RESTLESS 64
#define TILES_DISPLAY_BUFFER 1

//static uint8_t quit = 0;
//static uint8_t scrollFlag = 0;
//static uint8_t keyFlag = 1;
//static int8_t shiftY, shiftX;
//static uint8_t countX, countY = 0;
//static int8_t rockFordY, rockFordX, previousRockFordX, previousRockFordY;
//static uint8_t rockFordShift;
//static uint8_t rockFordAnimFlag = 0;
//static uint8_t currentDirection = 0;
//static uint8_t visibleX = 3;
//static uint8_t visibleY = 2;
//static int16_t countFrames = 0;
//static uint8_t lastFrame = 0;
//static uint8_t lastMoveFrame = 0;
//static uint8_t push = 0;
//static int fall = 0;
//static int which = 0;

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
extern int16_t countFrames;
extern uint8_t lastFrame;
extern uint8_t lastMoveFrame;
extern uint8_t push;
extern int fall;
extern int which;
extern CaveDecoder caveDecoder;
extern MapUtils *mapUtils;