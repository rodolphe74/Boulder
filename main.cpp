#include "Game.h"
#include "raylib.h"
#include <chrono>
#include <iostream>

int main(void)
{
	const int screenWidth = 1008;
	const int screenHeight = 768;
	

	InitWindow(screenWidth, screenHeight, "Boulder Dash");

	GameContext *gameContext = GameContext::getInstance();
	MapUtils::getInstance();
	Game *game = Game::getInstance();
	

	//int refreshRate = GetMonitorRefreshRate(0);
	SetTargetFPS(75);
	game->init();

	while (!WindowShouldClose())    // Detect window close button or ESC key
	{
		switch (game->currentScreen) {
		case GAMELOOP:
			game->gameLoopScreen();
			if (gameContext->gameOver) {
				game->currentScreen = GAMEOVER;
				// currentScreen = GAMEWAIT;
			}
			break;
		case GAMEOVER:
			game->gameOverScreen();
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
			{
				game->initGame();
				game->currentScreen = GAMELOOP;
			}
			break;
		case GAMEWAIT:
			gameContext->pause = 1;
			game->gameLoopScreen();
			break;
		case GAMEWIN:
			game->gameWinScreen();
			if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
			{
				game->initGame();
				game->currentScreen = GAMELOOP;
			}
			break;
		}
	}


	GameContext::releaseInstance();
	MapUtils::releaseInstance();
	Game::releaseInstance();

	CloseWindow();        // Close window and OpenGL context
	return 0;
}
