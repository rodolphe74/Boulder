#include "GameContext.h"

GameContext *GameContext::singleton;

GameContext *GameContext::getInstance()
{
	if (singleton == nullptr) {
		singleton = new GameContext();
	}
	return singleton;
}

void GameContext::releaseInstance()
{
	delete singleton;
}
