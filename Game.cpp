#include "Game.h"

Game *Game::singleton = nullptr;

Game *Game::getInstance()
{
	if (singleton == NULL) {
		singleton = new Game();
	}
	return singleton;
}

void Game::releaseInstance()
{
	delete singleton;
}

Game::Game()
{
	currentScreen = LOGO;
	caveDecoder = nullptr;
	mapUtils = nullptr;
	gameContext = GameContext::getInstance();
}

Game::~Game()
{
	printf("Game destruction\n");
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
	caveDecoder->DecodeCave(CaveDecoder::cave7);
	mapUtils = MapUtils::getInstance();
	mapUtils->cutTilesSheet(gameContext);
	mapUtils->convertCaveData(gameContext);
	gameContext->canExit = 0;
	mapUtils->preOut.isAnim = 0;
	gameContext->diamondsCount = 0;
	gameContext->won = 0;
	gameContext->canExitFrame = 0;
	gameContext->winFrame = 0;
	gameContext->pause = 0;
	currentScreen = GAMELOOP;
	initScrollVars();
	gameContext->startChrono = std::chrono::steady_clock::now();
	gameContext->lastChrono = std::chrono::steady_clock::now();
	gameContext->caveTime = caveDecoder->caveTime;
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
	if (IsKeyDown(KEY_LEFT_CONTROL) && (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN))) {
		gameContext->controlKey = 1;
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
		checkEnemyOnRockfordMove(gameContext->rockFordY, gameContext->rockFordX);
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
		checkEnemyOnRockfordMove(gameContext->rockFordY, gameContext->rockFordX);
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
		checkEnemyOnRockfordMove(gameContext->rockFordY, gameContext->rockFordX);
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
		checkEnemyOnRockfordMove(gameContext->rockFordY, gameContext->rockFordX);
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
		animateFireflies();
		animateButterflies();
		amoebaGrow();

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
	// Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	Rectangle frameDest = { (float)(x - gameContext->countX) * TILE_SIZE * ZOOM - gameContext->shiftX, (float)(y - gameContext->countY) * TILE_SIZE * ZOOM - gameContext->shiftY, (float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	DrawTexturePro(*(explosionSprite->bitmap), frameSource, frameDest, { 0, 0 }, 0, WHITE);
}

void Game::endRockfordAt(int x, int y, int c)
{
	map::Sprite *endSprite = mapUtils->matchAnimatedSprite[ROCKFORD].anim[6][c > 64 ? 0 : 7];
	Rectangle frameSource = { (float)endSprite->xsource, (float)endSprite->ysource, (float)endSprite->width, (float)endSprite->height };
	//Rectangle frameDest = { (float)x * TILE_SIZE * ZOOM, (float)y * TILE_SIZE * ZOOM,(float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
	Rectangle frameDest = { (float)(x - gameContext->countX) * TILE_SIZE * ZOOM - gameContext->shiftX, (float)(y - gameContext->countY) * TILE_SIZE * ZOOM - gameContext->shiftY, (float)(TILE_SIZE * ZOOM), (float)(TILE_SIZE * ZOOM) };
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
	int rockFordIsHere = checkRockfordInExplosionBox(e.y, e.x);
	if (rockFordIsHere) {
		printf("ROCKFORD IS HERE\n");
		e.type = ROCKFORD;
		e.count = 128;
		e.x = gameContext->rockFordX;
		e.y = gameContext->rockFordY;
	}

	if (e.type != BUTTERFLY) {
		mapUtils->map[e.y - 1][e.x - 1].type = SPACE;
		mapUtils->map[e.y - 1][e.x].type = SPACE;
		mapUtils->map[e.y - 1][e.x + 1].type = SPACE;

		mapUtils->map[e.y][e.x - 1].type = SPACE;
		mapUtils->map[e.y][e.x].type = SPACE;
		mapUtils->map[e.y][e.x + 1].type = SPACE;

		mapUtils->map[e.y + 1][e.x - 1].type = SPACE;
		mapUtils->map[e.y + 1][e.x].type = SPACE;
		mapUtils->map[e.y + 1][e.x + 1].type = SPACE;
	}
	else if (e.type == BUTTERFLY && e.count == 1) {
		mapUtils->map[e.y - 1][e.x - 1].type = DIAMOND;
		mapUtils->map[e.y - 1][e.x].type = DIAMOND;
		mapUtils->map[e.y - 1][e.x + 1].type = DIAMOND;

		mapUtils->map[e.y][e.x - 1].type = DIAMOND;
		mapUtils->map[e.y][e.x].type = DIAMOND;
		mapUtils->map[e.y][e.x + 1].type = DIAMOND;

		mapUtils->map[e.y + 1][e.x - 1].type = DIAMOND;
		mapUtils->map[e.y + 1][e.x].type = DIAMOND;
		mapUtils->map[e.y + 1][e.x + 1].type = DIAMOND;
	}

	explodeAt(e.x - 1, e.y - 1, e.count);
	explodeAt(e.x, e.y - 1, e.count);
	explodeAt(e.x + 1, e.y - 1, e.count);

	explodeAt(e.x - 1, e.y, e.count);
	explodeAt(e.x, e.y, e.count);
	explodeAt(e.x + 1, e.y, e.count);

	explodeAt(e.x - 1, e.y + 1, e.count);
	explodeAt(e.x, e.y + 1, e.count);
	explodeAt(e.x + 1, e.y + 1, e.count);


	if (e.type == ROCKFORD && e.count > 1) {
		endRockfordAt(e.x, e.y, e.count);
		gameContext->pause = 1;
	}
	else if (e.type == ROCKFORD && e.count == 1) {
		gameContext->gameOver = 1;
		gameContext->pause = 1;
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

	DrawText("TIME", TILE_SIZE * ZOOM * 8, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
	static char strTime[20];
	sprintf(strTime, "%d", gameContext->caveTime);
	DrawText(strTime, TILE_SIZE * ZOOM * 10, TILES_DISPLAY_HEIGHT * TILE_SIZE * ZOOM + ZOOM * 4, 20, DARKBLUE);
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
	caveDecoder->DecodeCave(CaveDecoder::cave4);
	mapUtils->convertCaveData(gameContext);
	mapUtils->preOut.isAnim = 0;
	gameContext->gameOver = 0;
	gameContext->countX = 0;
	gameContext->countY = 0;
	gameContext->shiftX = 0;
	gameContext->shiftY = 0;
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
	gameContext->pause = 0;
	gameContext->lastChrono = std::chrono::steady_clock::now();
	gameContext->startChrono = std::chrono::steady_clock::now();
	initScrollVars();
}


void Game::gameLoopScreen()
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
		if (!gameContext->pause)
			keyPressed();

		if (gameContext->currentDirection && !mapUtils->checkMove(gameContext)) {
			gameContext->keyFlag = 1;
		}
		else {
			if (!gameContext->controlKey) {
				prepareScroll();
			}
			else if (gameContext->controlKey == 1 && gameContext->countFrames % 4 == 0) {
				//printf("CONTROL :%d\n", gameContext->countFrames);
				if (mapUtils->checkMove(gameContext)) {
					checkDiamond();
					switch (gameContext->currentDirection)
					{
					case RIGHT:
						if (mapUtils->map[gameContext->rockFordY][gameContext->rockFordX + 1].type != BUTTERFLY && mapUtils->map[gameContext->rockFordY][gameContext->rockFordX + 1].type != FIREFLY) {
							mapUtils->map[gameContext->rockFordY][gameContext->rockFordX + 1].type = SPACE;
						}
						break;
					case LEFT:
						if (mapUtils->map[gameContext->rockFordY][gameContext->rockFordX - 1].type != BUTTERFLY && mapUtils->map[gameContext->rockFordY][gameContext->rockFordX - 1].type != FIREFLY) {
							mapUtils->map[gameContext->rockFordY][gameContext->rockFordX - 1].type = SPACE;
						}
						break;
					case UP:
						if (mapUtils->map[gameContext->rockFordY - 1][gameContext->rockFordX].type != BUTTERFLY && mapUtils->map[gameContext->rockFordY - 1][gameContext->rockFordX].type != FIREFLY) {
							mapUtils->map[gameContext->rockFordY - 1][gameContext->rockFordX].type = SPACE;
						}
						break;
					case DOWN:
						if (mapUtils->map[gameContext->rockFordY + 1][gameContext->rockFordX].type != BUTTERFLY && mapUtils->map[gameContext->rockFordY + 1][gameContext->rockFordX].type != FIREFLY) {
							mapUtils->map[gameContext->rockFordY + 1][gameContext->rockFordX].type = SPACE;
						}
						break;
					default:
						break;
					}
				}
				gameContext->controlKey = 0;
				gameContext->keyFlag = 1;
				gameContext->currentDirection = 0;
			}
			else {
				gameContext->controlKey = 0;
				gameContext->keyFlag = 1;
				gameContext->currentDirection = 0;
			}
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
		animateFireflies();
		animateButterflies();
		amoebaGrow();
	}

	if (gameContext->countFalls % 16 == 0) {
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::seconds>(now - gameContext->lastChrono).count() >= 1) {
			gameContext->caveTime--;
			gameContext->lastChrono = std::chrono::steady_clock::now();
		}
		if (gameContext->caveTime == 0) {
			// gameOver
			printf("NOT ENOUGH TIME ROCKFORD AT %d,%d\n", gameContext->rockFordY, gameContext->rockFordX);
			map::Explosion *e = new map::Explosion;
			*e = { (uint16_t)(gameContext->rockFordX), (uint16_t)(gameContext->rockFordY), ROCKFORD, 128 };
			mapUtils->explosions.insert(e);
		}
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

map::Object Game::checkLeft(int x, int y)
{
	map::Object o = mapUtils->map[y][x];
	uint8_t dir = LEFT_DIRECTION[o.direction];
	if (dir == LEFT && x > 0) {
		return mapUtils->map[y][x - 1];
	}
	else if (dir == RIGHT && x >= 0 && x < MAP_WIDTH - 1) {
		return mapUtils->map[y][x + 1];
	}
	else if (dir == UP && y > 0) {
		return mapUtils->map[y - 1][x];
	}
	else if (dir == DOWN && y >= 0 && y < MAP_HEIGHT - 1) {
		return mapUtils->map[y + 1][x];
	}
	return { UNKNOWN, 0, 0, 0 };
}


map::Object Game::checkRight(int x, int y)
{
	map::Object o = mapUtils->map[y][x];
	uint8_t dir = RIGHT_DIRECTION[o.direction];
	if (dir == LEFT && x > 0) {
		return mapUtils->map[y][x - 1];
	}
	else if (dir == RIGHT && x >= 0 && x < MAP_WIDTH - 1) {
		return mapUtils->map[y][x + 1];
	}
	else if (dir == UP && y > 0) {
		return mapUtils->map[y - 1][x];
	}
	else if (dir == DOWN && y >= 0 && y < MAP_HEIGHT - 1) {
		return mapUtils->map[y + 1][x];
	}
	return { UNKNOWN, 0, 0, 0 };
}

void Game::moveDirection(int x, int y)
{
	map::Object o = mapUtils->map[y][x];
	uint8_t dir = o.direction;
	if (dir == LEFT && x > 0) {
		checkRockford(y, x - 1);
		checkFalling(y, x - 1);
		mapUtils->map[y][x - 1].type = mapUtils->map[y][x].type;
		mapUtils->map[y][x - 1].direction = mapUtils->map[y][x].direction;
		mapUtils->map[y][x - 1].mark = 1;
		mapUtils->map[y][x].type = SPACE;
		mapUtils->map[y][x].direction = STATIONARY;
	}
	else if (dir == RIGHT && x >= 0 && x < MAP_WIDTH - 1) {
		checkRockford(y, x + 1);
		checkFalling(y, x + 1);
		mapUtils->map[y][x + 1].type = mapUtils->map[y][x].type;
		mapUtils->map[y][x + 1].direction = mapUtils->map[y][x].direction;
		mapUtils->map[y][x + 1].mark = 1;
		mapUtils->map[y][x].type = SPACE;
		mapUtils->map[y][x].direction = STATIONARY;
	}
	else if (dir == UP && y > 0) {
		checkRockford(y - 1, x);
		checkFalling(y - 1, x);
		mapUtils->map[y - 1][x].type = mapUtils->map[y][x].type;
		mapUtils->map[y - 1][x].direction = mapUtils->map[y][x].direction;
		mapUtils->map[y - 1][x].mark = 1;
		mapUtils->map[y][x].type = SPACE;
		mapUtils->map[y][x].direction = STATIONARY;
	}
	else if (dir == DOWN && y >= 0 && y < MAP_HEIGHT - 1) {
		checkRockford(y + 1, x);
		checkFalling(y + 1, x);
		mapUtils->map[y + 1][x].type = mapUtils->map[y][x].type;
		mapUtils->map[y + 1][x].direction = mapUtils->map[y][x].direction;
		mapUtils->map[y + 1][x].mark = 1;
		mapUtils->map[y][x].type = SPACE;
		mapUtils->map[y][x].direction = STATIONARY;
	}
}

void Game::checkRockford(int y, int x)
{
	if (mapUtils->map[y][x].type == ROCKFORD || mapUtils->map[y][x].type == TRANSITIONAL_ROCKFORD) {
		printf("HIT ROCKFORD AT %d,%d\n", y, x);
		map::Explosion *e = new map::Explosion;
		// *e = { (uint16_t)(x - gameContext->countX), (uint16_t)(y - gameContext->countY), ROCKFORD, 128 };
		*e = { (uint16_t)(x), (uint16_t)(y), mapUtils->map[y][x].type, 128 };
		mapUtils->explosions.insert(e);
	}
}

void Game::checkEnemyOnRockfordMove(int y, int x)
{
	if (mapUtils->map[y][x].type == FIREFLY || mapUtils->map[y][x].type == BUTTERFLY) {
		printf("HIT FIREFLY OR BUTTERFLY AT %d,%d\n", y, x);
		map::Explosion *e = new map::Explosion;
		*e = { (uint16_t)(x), (uint16_t)(y), /*mapUtils->map[y][x].type*/ROCKFORD, 128 };
		mapUtils->explosions.insert(e);
	}
}

void Game::checkFalling(int y, int x)
{
	if ((mapUtils->map[y][x].type == DIAMOND || mapUtils->map[y][x].type == ROCK) /*&& mapUtils->map[y][x].falling == FALL*/) {
		printf("HIT ROCK OR DIAMOND AT %d,%d\n", y, x);
		map::Explosion *e = new map::Explosion;
		*e = { (uint16_t)(x), (uint16_t)(y), mapUtils->map[y][x].type, 128 };
		mapUtils->explosions.insert(e);
	}
}

int Game::checkRockfordInExplosionBox(int y, int x)
{
	if (mapUtils->map[y - 1][x - 1].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y - 1][x].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y - 1][x + 1].type == ROCKFORD) {
		return 1;
	}

	if (mapUtils->map[y][x - 1].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y][x].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y][x + 1].type == ROCKFORD) {
		return 1;
	}

	if (mapUtils->map[y + 1][x - 1].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y + 1][x].type == ROCKFORD) {
		return 1;
	}
	if (mapUtils->map[y + 1][x + 1].type == ROCKFORD) {
		return 1;
	}

	return 0;
}

void Game::animateFireflies()
{
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == FIREFLY) {
				mapUtils->map[i][j].mark = 0;
			}
		}
	}
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == FIREFLY && mapUtils->map[i][j].mark == 0) {
				//printf("found firefly at %d,%d -> %d - %d\n", j, i, mapUtils->map[i][j].direction, checkLeft(j, i));
				if (j > 0 && mapUtils->map[i][j].direction == LEFT) {
					if (checkLeft(j, i).type == SPACE || checkLeft(j, i).type == ROCKFORD || checkLeft(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i][j - 1].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (j < MAP_WIDTH - 1 && mapUtils->map[i][j].direction == RIGHT) {
					if (checkLeft(j, i).type == SPACE || checkLeft(j, i).type == ROCKFORD || checkLeft(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i][j + 1].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (i > 0 && mapUtils->map[i][j].direction == UP) {
					if (checkLeft(j, i).type == SPACE || checkLeft(j, i).type == ROCKFORD || checkLeft(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i - 1][j].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (i < MAP_HEIGHT - 1 && mapUtils->map[i][j].direction == DOWN) {
					if (checkLeft(j, i).type == SPACE || checkLeft(j, i).type == ROCKFORD || checkLeft(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i + 1][j].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
			}
		}
	}
}

void Game::animateButterflies()
{
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == BUTTERFLY) {
				mapUtils->map[i][j].mark = 0;
			}
		}
	}
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == BUTTERFLY && mapUtils->map[i][j].mark == 0) {
				//printf("found firefly at %d,%d -> %d - %d\n", j, i, mapUtils->map[i][j].direction, checkLeft(j, i));
				if (j > 0 && mapUtils->map[i][j].direction == LEFT) {
					if (checkRight(j, i).type == SPACE || checkRight(j, i).type == ROCKFORD || checkRight(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i][j - 1].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (j < MAP_WIDTH - 1 && mapUtils->map[i][j].direction == RIGHT) {
					if (checkRight(j, i).type == SPACE || checkRight(j, i).type == ROCKFORD || checkRight(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i][j + 1].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (i > 0 && mapUtils->map[i][j].direction == UP) {
					if (checkRight(j, i).type == SPACE || checkRight(j, i).type == ROCKFORD || checkRight(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i - 1][j].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
				else if (i < MAP_HEIGHT - 1 && mapUtils->map[i][j].direction == DOWN) {
					if (checkRight(j, i).type == SPACE || checkRight(j, i).type == ROCKFORD || checkRight(j, i).type == TRANSITIONAL_ROCKFORD) {
						mapUtils->map[i][j].direction = RIGHT_DIRECTION[mapUtils->map[i][j].direction];
						moveDirection(j, i);
					}
					else if (mapUtils->map[i + 1][j].type == SPACE) {
						moveDirection(j, i);
					}
					else {
						mapUtils->map[i][j].direction = LEFT_DIRECTION[mapUtils->map[i][j].direction];
					}
				}
			}
		}
	}
}

int Game::countAmoebas()
{
	uint16_t amoebasCount = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_HEIGHT; j++) {
			if (mapUtils->map[i][j].type == AMOEBA) {
				amoebasCount++;
			}
		}
	}
	printf("amoebas count:%d\n", amoebasCount);
	if (amoebasCount > MAX_AMOEBAS) {
		// transforms to rocks
		transformsAmoebas(ROCK);
		return 1;
	}
	return 0;
}

int Game::checkAmoebaGrow()
{
	uint16_t countGrows = 0;
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == AMOEBA) {
				if (j > 0 && (mapUtils->map[i][j - 1].type == GRASS || mapUtils->map[i][j - 1].type == SPACE)) {
					return 1;
				}
				else if (j < MAP_WIDTH - 1 && (mapUtils->map[i][j + 1].type == GRASS || mapUtils->map[i][j + 1].type == SPACE)) {
					return 1;
				}
				else if (i > 0 && (mapUtils->map[i - 1][j].type == GRASS || mapUtils->map[i - 1][j].type == SPACE)) {
					return 1;
				}
				else if (i < MAP_HEIGHT - 1 && (mapUtils->map[i + 1][j].type == GRASS || mapUtils->map[i + 1][j].type == SPACE)) {
					return 1;
				}
			}
		}
	}
	transformsAmoebas(DIAMOND);
	return 0;
}

void Game::transformsAmoebas(int which)
{
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == AMOEBA) {
				mapUtils->map[i][j].type = which;
				mapUtils->map[i][j].falling = STATIONARY;
			}
		}
	}
}

void Game::amoebaGrow()
{
	if (countAmoebas()) {
		// transformed to rocks
		return;
	}
	if (!checkAmoebaGrow()) {
		// transformed to diamonds
		return;
	}

	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			if (mapUtils->map[i][j].type == AMOEBA) {
				int growChance = rand() % 100 + 1;
				int chrono = (int) std::chrono::duration_cast<std::chrono::seconds>(gameContext->lastChrono - gameContext->startChrono).count();
				if (growChance <= (chrono < caveDecoder->amoebaTime ? 3 : 25)) {
					int dir = rand() % 4 + 1;
					printf("dir:%d\n", dir);
					if (j > 0 && dir == LEFT && (mapUtils->map[i][j - 1].type == GRASS || mapUtils->map[i][j - 1].type == SPACE)) {
						mapUtils->map[i][j - 1].type = AMOEBA;
					}
					else if (j < MAP_WIDTH - 1 && dir == RIGHT && (mapUtils->map[i][j + 1].type == GRASS || mapUtils->map[i][j + 1].type == SPACE)) {
						mapUtils->map[i][j + 1].type = AMOEBA;
					}
					else if (i > 0 && dir == UP && (mapUtils->map[i - 1][j].type == GRASS || mapUtils->map[i - 1][j].type == SPACE)) {
						mapUtils->map[i - 1][j].type = AMOEBA;
					}
					else if (i < MAP_HEIGHT - 1 && dir == DOWN && (mapUtils->map[i + 1][j].type == GRASS || mapUtils->map[i + 1][j].type == SPACE)) {
						mapUtils->map[i + 1][j].type = AMOEBA;
					}
				}
			}
		}
	}
}
