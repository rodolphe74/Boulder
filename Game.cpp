#include "Game.h"

std::unique_ptr<Game> Game::singleton = NULL;

Game *Game::getInstance()
{
	if (singleton.get() == NULL) {
		singleton = std::make_unique<Game>();
	}
	return singleton.get();
}

Game::Game()
{
	currentScreen = LOGO;
	caveDecoder = nullptr;
	mapUtils = nullptr;
	gameContext = GameContext::getInstance();
}

void Game::initScrollVars()
{
	gameContext->countX = gameContext->rockFordX - SCROLL_BORDER;
	if (gameContext->countX < 0)
		gameContext->countX = 0;
	if (gameContext->countX > 19)
		gameContext->countX = 19;
	gameContext->countY = gameContext->rockFordY - SCROLL_BORDER;
	if (gameContext->countY < 0)
		gameContext->countY = 0;
	if (gameContext->countY > 7)
		gameContext->countY = 7;
	gameContext->visibleX = gameContext->rockFordX - gameContext->countX;
	gameContext->visibleY = gameContext->rockFordY - gameContext->countY;
}

void Game::init()
{
	caveDecoder = CaveDecoder::getInstance();
	caveDecoder->DecodeCave(CaveDecoder::cave1);
	mapUtils = MapUtils::getInstance();
	mapUtils->cutTilesSheet(gameContext);
	mapUtils->convertCaveData(gameContext);
	gameContext->canExit = 0;
	mapUtils->preOut.isAnim = 0;
	gameContext->diamondsCount = 0;
	gameContext->won = 0;
	gameContext->canExitFrame = 0;
	gameContext->winFrame = 0;
	currentScreen = GAMELOOP;
	initScrollVars();
}



void Game::keyPressed()
{
	if (IsKeyDown(KEY_RIGHT)) {
		gameContext->currentDirection = RIGHT;
		gameContext->keyFlag = 0;
	}
	if (IsKeyDown(KEY_LEFT)) {
		gameContext->currentDirection = LEFT;
		gameContext->keyFlag = 0;
	}
	if (IsKeyDown(KEY_UP)) {
		gameContext->currentDirection = UP;
		gameContext->keyFlag = 0;
	}
	if (IsKeyDown(KEY_DOWN)) {
		gameContext->currentDirection = DOWN;
		gameContext->keyFlag = 0;
	}
}

void Game::doFalls() {
	mapUtils->unmarkBouldersAndDiamonds();
	for (int y = 0; y < MAP_HEIGHT - 1; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			mapUtils->scanBouldersAndDiamonds(x, y);
			mapUtils->updateFallingBouldersAndDiamonds(x, y, gameContext);
		}
	}
}

void Game::prepareScroll()
{
	if (gameContext->currentDirection) {
		gameContext->rockfordAnimFlag = 1;
		gameContext->previousRockFordX = gameContext->rockFordX;
		gameContext->previousRockFordY = gameContext->rockFordY;
		mapUtils->map[gameContext->previousRockFordY][gameContext->previousRockFordX].type = TRANSITIONAL_SPACE;
		//mapUtils->map[previousRockFordY][previousRockFordX + 1].type = SPACE;
		checkDiamond();

	}
	if (gameContext->currentDirection == RIGHT) {
		gameContext->rockFordX++;
		mapUtils->map[gameContext->rockFordY][gameContext->rockFordX].type = TRANSITIONAL_ROCKFORD;
		if (gameContext->visibleX > TILES_DISPLAY_WIDTH - SCROLL_BORDER) {
			if (gameContext->countX < MAP_WIDTH - TILES_DISPLAY_WIDTH) {
				gameContext->scrollFlag = 1;
			}
			else {
				gameContext->scrollFlag = 0;
				gameContext->visibleX++;
			}
		}
		else {
			gameContext->scrollFlag = 0;
			gameContext->visibleX++;
		}
	}
	else if (gameContext->currentDirection == LEFT) {
		gameContext->rockFordX--;
		mapUtils->map[gameContext->rockFordY][gameContext->rockFordX].type = TRANSITIONAL_ROCKFORD;
		if (gameContext->visibleX < SCROLL_BORDER - 1) {
			if (gameContext->countX) {
				gameContext->scrollFlag = 1;
			}
			else {
				gameContext->scrollFlag = 0;
				gameContext->visibleX--;
			}
		}
		else {
			gameContext->scrollFlag = 0;
			gameContext->visibleX--;
		}
	}
	else if (gameContext->currentDirection == UP) {
		gameContext->rockFordY--;
		mapUtils->map[gameContext->rockFordY][gameContext->rockFordX].type = TRANSITIONAL_ROCKFORD;
		if (gameContext->visibleY < SCROLL_BORDER - 1) {
			if (gameContext->countY) {
				gameContext->scrollFlag = 1;
			}
			else {
				gameContext->scrollFlag = 0;
				gameContext->visibleY--;
			}
		}
		else {
			gameContext->scrollFlag = 0;
			gameContext->visibleY--;
		}
	}
	else if (gameContext->currentDirection == DOWN) {
		gameContext->rockFordY++;
		mapUtils->map[gameContext->rockFordY][gameContext->rockFordX].type = TRANSITIONAL_ROCKFORD;
		if (gameContext->visibleY > TILES_DISPLAY_HEIGHT - SCROLL_BORDER) {
			if (gameContext->countY < MAP_HEIGHT - TILES_DISPLAY_HEIGHT) {
				gameContext->scrollFlag = 1;
			}
			else {
				gameContext->scrollFlag = 0;
				gameContext->visibleY++;
			}
		}
		else {
			gameContext->scrollFlag = 0;
			gameContext->visibleY++;
		}
	}
}

void Game::checkDiamond()
{
	// Check diamond
	switch (gameContext->currentDirection)
	{
	case RIGHT:
		if (mapUtils->map[gameContext->rockFordY][gameContext->rockFordX + 1].type == DIAMOND) {
			gameContext->diamondsCount++;
		}
		break;
	case LEFT:
		if (mapUtils->map[gameContext->rockFordY][gameContext->rockFordX - 1].type == DIAMOND) {
			gameContext->diamondsCount++;
		}
		break;
	case DOWN:
		if (mapUtils->map[gameContext->rockFordY + 1][gameContext->rockFordX].type == DIAMOND) {
			gameContext->diamondsCount++;
		}
		break;
	case UP:
		if (mapUtils->map[gameContext->rockFordY - 1][gameContext->rockFordX].type == DIAMOND) {
			gameContext->diamondsCount++;
		}
		break;
	default:
		break;
	}

	if (gameContext->diamondsCount == caveDecoder->diamondsNeeded) {
		gameContext->canExit = 1;
		mapUtils->preOut.isAnim = 1;
	}
}

void Game::animate()
{
	if (gameContext->currentDirection == RIGHT) {
		gameContext->shiftX += ZOOM;
		if (gameContext->shiftX == TILE_SIZE * ZOOM) {
			gameContext->countX++;
			gameContext->shiftX = 0;
			gameContext->currentDirection = 0;
			gameContext->keyFlag = 1;
		}
	}
	else if (gameContext->currentDirection == LEFT) {
		gameContext->shiftX -= ZOOM;
		//if (shiftX == 0) {
		if (gameContext->shiftX == -TILE_SIZE * ZOOM) {
			gameContext->countX--;
			gameContext->shiftX = 0;
			gameContext->currentDirection = 0;
			gameContext->keyFlag = 1;
		}
	}
	else if (gameContext->currentDirection == UP) {
		gameContext->shiftY -= ZOOM;
		if (gameContext->shiftY == -TILE_SIZE * ZOOM) {
			gameContext->countY--;
			gameContext->shiftY = 0;
			gameContext->currentDirection = 0;
			gameContext->keyFlag = 1;
		}
	}
	else if (gameContext->currentDirection == DOWN) {
		gameContext->shiftY += ZOOM;
		if (gameContext->shiftY == TILE_SIZE * ZOOM) {
			gameContext->countY++;
			gameContext->shiftY = 0;
			gameContext->currentDirection = 0;
			gameContext->keyFlag = 1;
		}
	}
}

void Game::setRockfordSprite()
{
	if (!gameContext->currentDirection && gameContext->lastMoveFrame > FRAMES_BEFORE_RESTLESS) {
		gameContext->lastMoveFrame = FRAMES_BEFORE_RESTLESS + 1;
		mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 0;
	}
	else {
		if (!gameContext->currentDirection)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 5;
		else if (gameContext->currentDirection == UP)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 1;
		else if (gameContext->currentDirection == DOWN)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 2;
		else if (gameContext->currentDirection == LEFT)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 3;
		else if (gameContext->currentDirection == RIGHT)
			mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim = 4;
	}
}

void Game::animateRockfordPushing()
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim;
	uint8_t animCount = mapUtils->matchAnimatedSprite[ROCKFORD].animCount;
	map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][gameContext->countFrames % animCount];
	if (gameContext->currentDirection == LEFT || gameContext->currentDirection == RIGHT) {
		// stationary Rockford pushing his stone
		Rectangle frameSource = { (float)animatedRockFord->xsource, (float)animatedRockFord->ysource, (float)animatedRockFord->width, (float)animatedRockFord->height };
		Rectangle frameDest = { (float)gameContext->visibleX * TILE_SIZE * ZOOM, (float)gameContext->visibleY * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
		DrawTexturePro(*(animatedRockFord->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
	}
}

void Game::animateRockford()
{
	uint16_t sx, sy;

	if (gameContext->rockfordAnimFlag && gameContext->rockfordShift < TILE_SIZE) {
		gameContext->rockfordShift += SPEED;
		int8_t shift = (gameContext->currentDirection == LEFT || gameContext->currentDirection == UP ? -gameContext->rockfordShift : gameContext->rockfordShift);
		sx = (gameContext->previousRockFordX - gameContext->countX) * TILE_SIZE;
		sy = (gameContext->previousRockFordY - gameContext->countY) * TILE_SIZE;
		// get current Rockford animated sprite
		uint8_t currentAnim = mapUtils->matchAnimatedSprite[ROCKFORD].currentAnim;
		uint8_t animCount = mapUtils->matchAnimatedSprite[ROCKFORD].animCount;
		map::Sprite *animatedRockFord = mapUtils->matchAnimatedSprite[ROCKFORD].anim[currentAnim][gameContext->countFrames % animCount];

		if (gameContext->currentDirection == LEFT || gameContext->currentDirection == RIGHT) {
			if (gameContext->scrollFlag) {
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
		else if (gameContext->currentDirection == UP || gameContext->currentDirection == DOWN) {
			if (gameContext->scrollFlag) {
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

	if (gameContext->rockfordShift == TILE_SIZE) {
		mapUtils->map[gameContext->previousRockFordY][gameContext->previousRockFordX].type = SPACE;
		mapUtils->map[gameContext->rockFordY][gameContext->rockFordX].type = ROCKFORD;
		gameContext->rockfordAnimFlag = 0;
		gameContext->rockfordShift = 0;
		gameContext->keyFlag = 1;
		gameContext->currentDirection = 0;
		gameContext->scrollFlag = 0;
		gameContext->countFalls = 1;	// if stationnary Rockford, next fall check at countFalls == 16

		// check exit
		if (gameContext->canExit && gameContext->rockFordX == gameContext->exitX && gameContext->rockFordY == gameContext->exitY) {
			gameContext->won = 1;
			gameContext->winFrame = 0;
		}

		doFalls();

		mapUtils->drawMap(gameContext);
		//printf("%d %d     %d %d      %d %d\n", rockFordX, rockFordY, countX, countY, visibleX, visibleY);
	}
	else {
		gameContext->keyFlag = 0;
		gameContext->rockfordAnimFlag = 1;
	}
}

void Game::explodeAt(int x, int y, int c)
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[EXPLODE].currentAnim;
	uint8_t animCount = mapUtils->matchAnimatedSprite[EXPLODE].animCount;
	map::Sprite *explosionSprite = mapUtils->matchAnimatedSprite[EXPLODE].anim[currentAnim][c % animCount];
	Rectangle frameSource = { (float)explosionSprite->xsource, (float)explosionSprite->ysource, (float)explosionSprite->width, (float)explosionSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(explosionSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}

void Game::endRockfordAt(int x, int y, int c)
{
	map::Sprite *endSprite = mapUtils->matchAnimatedSprite[ROCKFORD].anim[6][c > 64 ? 0 : 7];
	Rectangle frameSource = { (float)endSprite->xsource, (float)endSprite->ysource, (float)endSprite->width, (float)endSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(endSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}


void Game::winRockfordAt(int x, int y, int c)
{
	uint8_t currentAnim = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].currentAnim;
	uint8_t animCount = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].animCount;
	map::Sprite *endSprite = mapUtils->matchAnimatedSprite[WIN_ROCKFORD].anim[currentAnim][c % animCount];
	Rectangle frameSource = { (float)endSprite->xsource, (float)endSprite->ysource, (float)endSprite->width, (float)endSprite->height };
	Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(endSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}

void Game::exitRockfordAt(int x, int y, int c) {
	DrawText("EXIT", x * TILE_SIZE * ZOOM + TILE_SIZE, y * TILE_SIZE * ZOOM - c, 12 * ZOOM, GREEN);
}


void Game::doExplosion(map::Explosion &e)
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

	mapUtils->map[e.y + gameContext->countY - 1][e.x + gameContext->countX - 1].type = SPACE;
	mapUtils->map[e.y + gameContext->countY - 1][e.x + gameContext->countX].type = SPACE;
	mapUtils->map[e.y + gameContext->countY - 1][e.x + gameContext->countX + 1].type = SPACE;

	mapUtils->map[e.y + gameContext->countY][e.x + gameContext->countX - 1].type = SPACE;
	mapUtils->map[e.y + gameContext->countY][e.x + gameContext->countX].type = SPACE;
	mapUtils->map[e.y + gameContext->countY][e.x + gameContext->countX + 1].type = SPACE;

	mapUtils->map[e.y + gameContext->countY + 1][e.x + gameContext->countX - 1].type = SPACE;
	mapUtils->map[e.y + gameContext->countY + 1][e.x + gameContext->countX].type = SPACE;
	mapUtils->map[e.y + gameContext->countY + 1][e.x + gameContext->countX + 1].type = SPACE;

	if (e.type == ROCKFORD && e.count > 1) {
		endRockfordAt(e.x, e.y, e.count);
	}
	else if (e.type == ROCKFORD && e.count == 1) {
		gameContext->gameOver = 1;
	}

	e.count--;
}

void Game::iterateExplosions()
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

void Game::showExit()
{
	static int8_t exitX = 0, exitY = 0;
	if (gameContext->canExitFrame == 0) {
		exitX = gameContext->rockFordX - gameContext->countX;
		exitY = gameContext->rockFordY - gameContext->countY;
	}
	if (gameContext->canExitFrame < 128) {
		exitRockfordAt(exitX, exitY, gameContext->canExitFrame);
	}
	gameContext->canExitFrame += ZOOM;
}

void Game::drawGameStats()
{
	DrawRectangle(0, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM, SCREEN_WIDTH, TILE_SIZE * ZOOM, RAYWHITE);
	DrawText("DIAMONDS", TILE_SIZE * ZOOM, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
	static char strDiamondsCount[20];
	sprintf(strDiamondsCount, "%d/%d", gameContext->diamondsCount, caveDecoder->diamondsNeeded);
	DrawText(strDiamondsCount, TILE_SIZE * ZOOM * 4, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
}

void Game::gameOverScreen()
{
	BeginDrawing();
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
	DrawText("GAME OVER", 20, 20, 40, DARKBLUE);
	DrawText("PRESS ENTER to RESTART", 120, 220, 20, DARKBLUE);
	EndDrawing();
}

void Game::gameWinScreen()
{
	BeginDrawing();
	DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLUE);
	DrawText("YOU WON", 20, 20, 40, DARKBLUE);
	DrawText("PRESS ENTER to RESTART", 120, 220, 20, DARKBLUE);
	EndDrawing();
}


void Game::initGame()
{
	caveDecoder->DecodeCave(CaveDecoder::cave1);
	mapUtils->convertCaveData(gameContext);
	mapUtils->preOut.isAnim = 0;
	gameContext->gameOver = 0;
	gameContext->countX = 0;
	gameContext->countY = 0;
	gameContext->shiftX = 0;
	gameContext->shiftY = 0;
	//rockFordX = 3;
	//rockFordY = 2;
	gameContext->rockfordShift = 0;
	gameContext->previousRockFordX = gameContext->rockFordX;
	gameContext->previousRockFordY = gameContext->rockFordY;
	gameContext->visibleX = 3;
	gameContext->visibleY = 2;
	gameContext->canExit = 0;
	gameContext->diamondsCount = 0;
	gameContext->won = 0;
	gameContext->canExitFrame = 0;
	gameContext->winFrame = 0;
	initScrollVars();
}


void Game::gameLoopScreen(int pause)
{
	setRockfordSprite();

	if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT)) && (gameContext->currentDirection == LEFT || gameContext->currentDirection == RIGHT)) {
		uint8_t pushIt = mapUtils->checkPush(gameContext);
		if (pushIt && gameContext->push && gameContext->push == FRAMES_BEFORE_PUSH) {
			mapUtils->moveStone(gameContext);
			gameContext->push = 0;
		}
	}
	else {
		gameContext->push = 0;
	}

	// Update
	if (gameContext->keyFlag) {
		if (!pause)
			keyPressed();

		if (gameContext->currentDirection && !mapUtils->checkMove(gameContext)) {
			gameContext->keyFlag = 1;
		}
		else {
			prepareScroll();
		}
	}

	if (!gameContext->keyFlag && gameContext->currentDirection) {
		if (gameContext->scrollFlag)
			animate();
	}

	//std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	BeginDrawing();

	ClearBackground(RAYWHITE);

	mapUtils->drawMap(gameContext);

	if (gameContext->push && !gameContext->rockfordAnimFlag) {
		animateRockfordPushing();
	}

	if (gameContext->rockfordAnimFlag) {
		animateRockford();
	}

	if (!gameContext->rockfordAnimFlag && (gameContext->countFalls % 16 == 0)) {
		doFalls();
	}

	if (mapUtils->explosions.size()) {
		iterateExplosions();
	}

	if (gameContext->canExit) {
		showExit();
	}

	if (gameContext->won) {
		winRockfordAt(gameContext->exitX - gameContext->countX, gameContext->exitY - gameContext->countY, gameContext->winFrame);
		if (gameContext->winFrame == 128) {
			currentScreen = GAMEWIN;
		}
		gameContext->winFrame++;
	}

	// Game informations
	drawGameStats();

	EndDrawing();
	//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	//std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << " [millis]" << std::endl;

	gameContext->countFrames++;
	if (gameContext->countFrames == 1024)
		gameContext->countFrames = 0;

	gameContext->countFalls++;
	if (gameContext->countFrames == 1024)
		gameContext->countFrames = 0;
}