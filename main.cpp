#include "raylib.h"
#include "Globals.h"
#include "main.h"
#include <chrono>
#include <iostream>

void init()
{
	caveDecoder.DecodeCave(CaveDecoder::cave1);
	mapUtils = MapUtils::getInstance();
	mapUtils->cutTilesSheet();
	mapUtils->convertCaveData();
}

void keyPressed()
{
	if (IsKeyDown(KEY_RIGHT)) {
		currentDirection = RIGHT;
		keyFlag = 0;
	}
	if (IsKeyDown(KEY_LEFT)) {
		currentDirection = LEFT;
		keyFlag = 0;
	}
	if (IsKeyDown(KEY_UP)) {
		currentDirection = UP;
		keyFlag = 0;
	}
	if (IsKeyDown(KEY_DOWN)) {
		currentDirection = DOWN;
		keyFlag = 0;
	}
}

void prepareScroll()
{
	if (currentDirection) {
		rockfordAnimFlag = 1;
		previousRockFordX = rockFordX;
		previousRockFordY = rockFordY;
		mapUtils->map[previousRockFordY][previousRockFordX].type = SPACE;
		//mapUtils->map[previousRockFordY][previousRockFordX + 1].type = SPACE;
	}
	if (currentDirection == RIGHT) {
		rockFordX++;
		if (visibleX > TILES_DISPLAY_WIDTH - SCROLL_BORDER) {
			if (countX < MAP_WIDTH - TILES_DISPLAY_WIDTH) {
				scrollFlag = 1;
			}
			else {
				scrollFlag = 0;
				visibleX++;
			}
		}
		else {
			scrollFlag = 0;
			visibleX++;
		}
	}
	else if (currentDirection == LEFT) {
		rockFordX--;
		if (visibleX < SCROLL_BORDER - 1) {
			if (countX) {
				scrollFlag = 1;
			}
			else {
				scrollFlag = 0;
				visibleX--;
			}
		}
		else {
			scrollFlag = 0;
			visibleX--;
		}
	}
	else if (currentDirection == UP) {
		rockFordY--;
		if (visibleY < SCROLL_BORDER - 1) {
			if (countY) {
				scrollFlag = 1;
			}
			else {
				scrollFlag = 0;
				visibleY--;
			}
		}
		else {
			scrollFlag = 0;
			visibleY--;
		}
	}
	else if (currentDirection == DOWN) {
		rockFordY++;
		if (visibleY > TILES_DISPLAY_HEIGHT - SCROLL_BORDER) {
			if (countY < MAP_HEIGHT - TILES_DISPLAY_HEIGHT) {
				scrollFlag = 1;
			}
			else {
				scrollFlag = 0;
				visibleY++;
			}
		}
		else {
			scrollFlag = 0;
			visibleY++;
		}
	}
}

void animate()
{
	if (currentDirection == RIGHT) {
		shiftX += ZOOM;
		if (shiftX == TILE_SIZE * ZOOM) {
			countX++;
			shiftX = 0;
			currentDirection = 0;
			keyFlag = 1;
		}
		rockfordAnimFlag = 1;
	}
	else if (currentDirection == LEFT) {
		shiftX -= ZOOM;
		//if (shiftX == 0) {
		if (shiftX == -TILE_SIZE * ZOOM) {
			countX--;
			shiftX = 0;
			currentDirection = 0;
			keyFlag = 1;
		}
		rockfordAnimFlag = 1;
	}
	else if (currentDirection == UP) {
		shiftY -= ZOOM;
		if (shiftY == -TILE_SIZE * ZOOM) {
			countY--;
			shiftY = 0;
			currentDirection = 0;
			keyFlag = 1;
		}
	}
	else if (currentDirection == DOWN) {
		shiftY += ZOOM;
		if (shiftY == TILE_SIZE * ZOOM) {
			countY++;
			shiftY = 0;
			currentDirection = 0;
			keyFlag = 1;
		}
	}
}

void setRockfordSprite()
{
	if (!currentDirection && lastMoveFrame > FRAMES_BEFORE_RESTLESS) {
		lastMoveFrame = FRAMES_BEFORE_RESTLESS + 1;
		mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 0;
	}
	else {
		if (!currentDirection)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 5;
		else if (currentDirection == UP)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 1;
		else if (currentDirection == DOWN)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 2;
		else if (currentDirection == LEFT)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 3;
		else if (currentDirection == RIGHT)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 4;
	}
}

void animateRockfordPushing()
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim;
	map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][countFrames % 12];
	if (currentDirection == LEFT || currentDirection == RIGHT) {
		// stationary Rockford pushing his stone
		Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
		Rectangle frameDest = { (float)visibleX * TILE_SIZE * ZOOM, (float)visibleY * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
		DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
	}
}

void animateRockford()
{
	uint16_t sx, sy;

	if (rockfordAnimFlag && rockfordShift < TILE_SIZE) {
		rockfordShift += SPEED;
		int8_t shift = (currentDirection == LEFT || currentDirection == UP ? -rockfordShift : rockfordShift);
		sx = (previousRockFordX - countX) * TILE_SIZE;
		sy = (previousRockFordY - countY) * TILE_SIZE;
		// get current Rockford animated sprite
		uint8_t currentAnim = mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim;
		map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][countFrames % 12];

		if (currentDirection == LEFT || currentDirection == RIGHT) {
			if (scrollFlag) {
				// stationary Rockford & scroll
				Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
				Rectangle frameDest = { (float)sx * ZOOM, (float)sy * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
				DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
			}
			else {
				// move only Rockford
				Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
				Rectangle frameDest = { (float)(sx + shift) * ZOOM, (float)sy * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
				DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
			}
		}
		else if (currentDirection == UP || currentDirection == DOWN) {
			if (scrollFlag) {
				// stationary Rockford & scroll
				Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
				Rectangle frameDest = { (float)sx * ZOOM, (float)sy * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
				DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
			}
			else {
				// move only Rockford
				Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
				Rectangle frameDest = { (float)sx * ZOOM, (float)(sy + shift) * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
				DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
			}
		}
	}

	if (rockfordShift == TILE_SIZE) {
		mapUtils->map[rockFordY][rockFordX].type = ROCKFORD;
		rockfordAnimFlag = 0;
		rockfordShift = 0;
		keyFlag = 1;
		currentDirection = 0;
		scrollFlag = 0;
		mapUtils->drawMap();
	}
	else {
		keyFlag = 0;
		rockfordAnimFlag = 1;
	}
}

int main(void)
{
	const int screenWidth = 1008;
	const int screenHeight = 768;

	InitWindow(screenWidth, screenHeight, "Boulder Dash");

	//int refreshRate = GetMonitorRefreshRate(0);

	SetTargetFPS(75);
	init();

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		setRockfordSprite();

		if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) && (currentDirection == LEFT || currentDirection == RIGHT)) {
			uint8_t pushIt = mapUtils->checkPush();
			if (pushIt && push && push == FRAMES_BEFORE_PUSH) {
				mapUtils->moveStone();
				push = 0;
			}
		}
		else {
			push = 0;
		}

		// Update
		if (keyFlag) {
			keyPressed();

			if (currentDirection && !mapUtils->checkMove()) {
				keyFlag = 1;
			}
			else {
				prepareScroll();
			}
		}

		if (!keyFlag && currentDirection) {
			if (scrollFlag)
				animate();
		}

		//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		BeginDrawing();

		ClearBackground(RAYWHITE);

		mapUtils->drawMap();

		if (push && !rockfordAnimFlag) {
			animateRockfordPushing();
		}

		if (rockfordAnimFlag) {
			animateRockford();
		}

		// Game informations
		DrawRectangle(0, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM, screenWidth, TILE_SIZE * ZOOM, RAYWHITE);

		EndDrawing();
		//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [millis]" << std::endl;


		countFrames++;
		if (countFrames == 1024)
			countFrames = 0;
	}

	CloseWindow();        // Close window and OpenGL context
	return 0;
}






