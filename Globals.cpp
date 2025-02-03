#include "Globals.h"

uint8_t quit = 0;
uint8_t scrollFlag = 0;
uint8_t keyFlag = 1;
int8_t shiftY = 0, shiftX= 0;
uint8_t countX, countY = 0;
int8_t rockFordY, rockFordX, previousRockFordX, previousRockFordY;
uint8_t rockfordShift;
uint8_t rockfordAnimFlag = 0;
uint8_t currentDirection = 0;
uint8_t visibleX = 3;
uint8_t visibleY = 2;
uint32_t countFrames = 0;
uint8_t lastFrame = 0;
uint8_t lastMoveFrame = 0;
uint8_t push = 0;
int fall = 0;
int which = 0;
CaveDecoder caveDecoder;
MapUtils *mapUtils;