#pragma once
#include <stdio.h>
#include <stdint.h>
#include <memory>
#include "CaveDecoder.h"


#define FRAMES_BEFORE_RESTLESS 64
#define TILES_DISPLAY_BUFFER 1

class GameContext
{
private:
	GameContext() {};
	~GameContext() {
		printf("GameContext destruction\n");
	}
public:
	//static std::unique_ptr<GameContext> singleton;
	static GameContext *singleton;
	static GameContext *getInstance();
	static void releaseInstance();

	uint8_t quit = 0;
	uint8_t scrollFlag = 0;
	uint8_t keyFlag = 1;
	int8_t shiftY = 0, shiftX = 0;
	int8_t countX = 0, countY = 0;
	int8_t rockFordY = 0, rockFordX = 0, previousRockFordX = 0, previousRockFordY = 0;
	uint8_t rockfordShift = 0;
	uint8_t rockfordAnimFlag = 0;
	uint8_t currentDirection = 0;
	uint8_t visibleX = 3;
	uint8_t visibleY = 2;
	uint32_t countFrames = 0;
	uint32_t countFalls = 0;
	uint32_t winFrame = 0;
	uint32_t canExitFrame = 0;
	uint8_t lastFrame = 0;
	uint8_t lastMoveFrame = 0;
	uint8_t push = 0;
	uint8_t gameOver = 0;
	uint8_t diamondsCount = 0;
	uint8_t canExit = 0;
	uint8_t exitX = 0, exitY = 0;
	uint8_t won = 0;
	uint8_t pause = 0;
};

