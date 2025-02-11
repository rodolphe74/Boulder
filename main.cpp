#include "raylib.h"
#include "Globals.h"
#include <chrono>
#include <iostream>
#include "main.h"


void initScrollVars()
{
	countX = rockFordX - SCROLL_BORDER;
	if (countX < 0)
		countX = 0;
	if (countX > 19)
		countX = 19;
	countY = rockFordY - SCROLL_BORDER;
	if (countY < 0)
		countY = 0;
	if (countY > 7)
		countY = 7;
	visibleX = rockFordX - countX;
	visibleY = rockFordY - countY;
}

void init()
{
	caveDecoder = CaveDecoder::getInstance();
	caveDecoder->DecodeCave(CaveDecoder::cave1);
	mapUtils = MapUtils::getInstance();
	mapUtils->cutTilesSheet();
	mapUtils->convertCaveData();
	canExit = 0;
	mapUtils->preOut.isAnim = 0;
	diamondsCount = 0;
	won = 0;
	canExitFrame = 0;
	winFrame = 0;
	initScrollVars();
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

void doFalls() {
	mapUtils->unmarkBouldersAndDiamonds();
	for (int y = 0; y < MAP_HEIGHT - 1; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			mapUtils->scanBouldersAndDiamonds(x, y);
			mapUtils->updateFallingBouldersAndDiamonds(x, y);
		}
	}
}

void prepareScroll()
{
	if (currentDirection) {
		rockfordAnimFlag = 1;
		previousRockFordX = rockFordX;
		previousRockFordY = rockFordY;
		mapUtils->map[previousRockFordY][previousRockFordX].type = TRANSITIONAL_SPACE;
		//mapUtils->map[previousRockFordY][previousRockFordX + 1].type = SPACE;
		checkDiamond();

	}
	if (currentDirection == RIGHT) {
		rockFordX++;
		mapUtils->map[rockFordY][rockFordX].type = TRANSITIONAL_ROCKFORD;
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
		mapUtils->map[rockFordY][rockFordX].type = TRANSITIONAL_ROCKFORD;
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
		mapUtils->map[rockFordY][rockFordX].type = TRANSITIONAL_ROCKFORD;
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
		mapUtils->map[rockFordY][rockFordX].type = TRANSITIONAL_ROCKFORD;
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

void checkDiamond()
{
	// Check diamond
	switch (currentDirection)
	{
	case RIGHT:
		if (mapUtils->map[rockFordY][rockFordX + 1].type == DIAMOND) {
			diamondsCount++;
		}
		break;
	case LEFT:
		if (mapUtils->map[rockFordY][rockFordX - 1].type == DIAMOND) {
			diamondsCount++;
		}
		break;
	case DOWN:
		if (mapUtils->map[rockFordY + 1][rockFordX].type == DIAMOND) {
			diamondsCount++;
		}
		break;
	case UP:
		if (mapUtils->map[rockFordY - 1][rockFordX].type == DIAMOND) {
			diamondsCount++;
		}
		break;
	default:
		break;
	}

	if (diamondsCount == caveDecoder->diamondsNeeded) {
		canExit = 1;
		mapUtils->preOut.isAnim = 1;
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
	uint8_t animCount = mapUtils->matchAnimatedSprite[ROCKFORD].animCount;
	map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][countFrames % animCount];
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
		uint8_t animCount = mapUtils->matchAnimatedSprite[ROCKFORD].animCount;
		map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][countFrames % animCount];

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
		mapUtils->map[previousRockFordY][previousRockFordX].type = SPACE;
		mapUtils->map[rockFordY][rockFordX].type = ROCKFORD;
		rockfordAnimFlag = 0;
		rockfordShift = 0;
		keyFlag = 1;
		currentDirection = 0;
		scrollFlag = 0;
		countFalls = 1;	// if stationnary Rockford, next fall check at countFalls == 16

		// check exit
		if (canExit && rockFordX == exitX && rockFordY == exitY) {
			won = 1;
			winFrame = 0;
		}

		doFalls();

		mapUtils->drawMap();
		//printf("%d %d     %d %d      %d %d\n", rockFordX, rockFordY, countX, countY, visibleX, visibleY);
	}
	else {
		keyFlag = 0;
		rockfordAnimFlag = 1;
	}
}

void explodeAt(int x, int y, int c)
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[EXPLODE].currentAnim;
	uint8_t animCount = mapUtils->matchAnimatedSprite[EXPLODE].animCount;
	map::Sprite *explosionSprite = mapUtils->matchAnimatedSprite[EXPLODE].anim[currentAnim][c % animCount];
	Rectangle frameSource = { (float)explosionSprite->xsource, (float)explosionSprite->ysource, (float)explosionSprite->width, (float)explosionSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(explosionSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}

void endRockfordAt(int x, int y, int c)
{
	map::Sprite *endSprite = mapUtils->matchAnimatedSprite[ROCKFORD].anim[6][c > 64 ? 0 : 7];
	Rectangle frameSource = { (float)endSprite->xsource, (float)endSprite->ysource, (float)endSprite->width, (float)endSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(endSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}


void winRockfordAt(int x, int y, int c)
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].currentAnim;
	uint8_t animCount = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].animCount;
	map::Sprite *endSprite = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].anim[currentAnim][c % animCount];
	Rectangle frameSource = { (float)endSprite->xsource, (float)endSprite->ysource, (float)endSprite->width, (float)endSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(endSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}

void exitRockfordAt(int x, int y, int c) {
	DrawText("EXIT", x * TILE_SIZE * ZOOM + TILE_SIZE, y * TILE_SIZE * ZOOM - c, 12 * ZOOM, GREEN);
}


void doExplosion(map::Explosion &e)
{
	explodeAt(e.x - 1, e.y - 1, e.count);
	explodeAt(e.x, e.y - 1, e.count);
	explodeAt(e.x + 1, e.y - 1, e.count);

	explodeAt(e.x - 1, e.y, e.count);
	explodeAt(e.x, e.y, e.count);
	explodeAt(e.x + 1, e.y, e.count);

	explodeAt(e.x - 1, e.y + 1, e.count);
	explodeAt(e.x, e.y + 1, e.count);
	explodeAt(e.x + 1, e.y + 1, e.count);

	mapUtils->map[e.y + countY - 1][e.x + countX - 1].type = SPACE;
	mapUtils->map[e.y + countY - 1][e.x + countX].type = SPACE;
	mapUtils->map[e.y + countY - 1][e.x + countX + 1].type = SPACE;

	mapUtils->map[e.y + countY][e.x + countX - 1].type = SPACE;
	mapUtils->map[e.y + countY][e.x + countX].type = SPACE;
	mapUtils->map[e.y + countY][e.x + countX + 1].type = SPACE;

	mapUtils->map[e.y + countY + 1][e.x + countX - 1].type = SPACE;
	mapUtils->map[e.y + countY + 1][e.x + countX].type = SPACE;
	mapUtils->map[e.y + countY + 1][e.x + countX + 1].type = SPACE;

	if (e.type == ROCKFORD && e.count > 1) {
		endRockfordAt(e.x, e.y, e.count);
	}
	else if (e.type == ROCKFORD && e.count == 1) {
		gameOver = 1;
	}

	e.count--;
}

void iterateExplosions()
{
	std::set<map::Explosion *>::iterator it;
	for (it = mapUtils->explosions.begin(); it != mapUtils->explosions.end();) {
		map::Explosion *e = *it;
		if (e->count <= 0) {
			delete(*it);
			mapUtils->explosions.erase(it++);
		}
		else {
			doExplosion(*e);
			++it;
		}
	}
}

void showExit()
{
	static int8_t exitX = 0, exitY = 0;
	if (canExitFrame == 0) {
		exitX = rockFordX - countX;
		exitY = rockFordY - countY;
	}
	if (canExitFrame < 128) {
		exitRockfordAt(exitX, exitY, canExitFrame);
	}
	canExitFrame += ZOOM;
}

void drawGameStats()
{
	DrawRectangle(0, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM, SCREEN_WIDTH, TILE_SIZE * ZOOM, RAYWHITE);
	DrawText("DIAMONDS", TILE_SIZE * ZOOM, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
	static char strDiamondsCount[20];
	sprintf(strDiamondsCount, "%d/%d", diamondsCount, caveDecoder->diamondsNeeded);
	DrawText(strDiamondsCount, TILE_SIZE * ZOOM * 4, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
}

void gameOverScreen()
{
	BeginDrawing();
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
	DrawText("GAME OVER", 20, 20, 40, DARKBLUE);
	DrawText("PRESS ENTER to RESTART", 120, 220, 20, DARKBLUE);
	EndDrawing();
}

void gameWinScreen()
{
	BeginDrawing();
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
	DrawText("YOU WON", 20, 20, 40, DARKBLUE);
	DrawText("PRESS ENTER to RESTART", 120, 220, 20, DARKBLUE);
	EndDrawing();
}


void initGame()
{
	caveDecoder->DecodeCave(CaveDecoder::cave1);
	mapUtils->convertCaveData();
	mapUtils->preOut.isAnim = 0;
	gameOver = 0;
	countX = 0;
	countY = 0;
	shiftX = 0;
	shiftY = 0;
	//rockFordX = 3;
	//rockFordY = 2;
	rockfordShift = 0;
	previousRockFordX = rockFordX;
	previousRockFordY = rockFordY;
	visibleX = 3;
	visibleY = 2;
	canExit = 0;
	diamondsCount = 0;
	won = 0;
	canExitFrame = 0;
	winFrame = 0;
	initScrollVars();
}


void gameLoopScreen(int pause = 0)
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
		if (!pause)
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

	if (!rockfordAnimFlag && (countFalls % 16 == 0)) {
		doFalls();
	}

	if (mapUtils->explosions.size()) {
		iterateExplosions();
	}

	if (canExit) {
		showExit();
	}

	if (won) {
		winRockfordAt(exitX - countX, exitY - countY, winFrame);
		if (winFrame == 128) {
			currentScreen = GAMEWIN;
		}
		winFrame++;
	}

	// Game informations
	drawGameStats();

	EndDrawing();
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [millis]" << std::endl;

	countFrames++;
	if (countFrames == 1024)
		countFrames = 0;

	countFalls++;
	if (countFrames == 1024)
		countFrames = 0;
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
		switch (currentScreen) {
		case GAMELOOP:
			gameLoopScreen();
			if (gameOver) {
				currentScreen = GAMEOVER;
				// currentScreen = GAMEWAIT;
			}
			break;
		case GAMEOVER:
			gameOverScreen();
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
			{
				initGame();
				currentScreen = GAMELOOP;
			}
			break;
		case GAMEWAIT:
			gameLoopScreen(1);
			break;
		case GAMEWIN:
			gameWinScreen();
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
			{
				initGame();
				currentScreen = GAMELOOP;
			}
			break;
		}


	}

	CloseWindow();        // Close window and OpenGL context
	return 0;
}
