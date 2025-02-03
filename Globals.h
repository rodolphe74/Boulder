#pragma once
#include <stdint.h>
#include "CaveDecoder.h"
#include "MapUtils.h"

#define FRAMES_BEFORE_RESTLESS 64
#define TILES_DISPLAY_BUFFER 1

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
extern uint8_t lastFrame;
extern uint8_t lastMoveFrame;
extern uint8_t push;
extern int fall;
extern int which;
extern CaveDecoder caveDecoder;
extern MapUtils *mapUtils;