#pragma once
#include <stdint.h>
#include <memory>
#include "CaveDecoder.h"
#include "MapUtils.h"
#include "GameContext.h"

#define FRAMES_BEFORE_RESTLESS 64
#define TILES_DISPLAY_BUFFER 1

typedef enum GameScreen { LOGO = 0, TITLE, GAMELOOP, GAMEOVER, GAMEWAIT, GAMEWIN } GameScreen;

class Game
{
private:
	static std::unique_ptr<Game> singleton;

public:

	static Game *getInstance();
	Game();
	~Game();

	GameScreen currentScreen;
	CaveDecoder *caveDecoder;
	MapUtils *mapUtils;
	GameContext *gameContext;

	void initScrollVars();
	void init();
	void keyPressed();
	void doFalls();
	void prepareScroll();
	void checkDiamond();
	void animate();
	void setRockfordSprite();
	void animateRockfordPushing();
	void animateRockford();
	void explodeAt(int x, int y, int c);
	void endRockfordAt(int x, int y, int c);
	void winRockfordAt(int x, int y, int c);
	void exitRockfordAt(int x, int y, int c);
	void doExplosion(map::Explosion &e);
	void iterateExplosions();
	void showExit();
	void drawGameStats();
	void gameOverScreen();
	void gameWinScreen();
	void initGame();
	void gameLoopScreen(int pause = 0);
	uint8_t checkLeft(int x, int y);
	void moveDirection(int x, int y);
	void checkRockford(int y, int x);
	void checkEnnemy(int y, int x);
	void animateFireflies();
};

