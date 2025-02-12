#include "GameContext.h"

std::unique_ptr<GameContext> GameContext::singleton = nullptr;

GameContext *GameContext::getInstance()
{
	if (singleton.get() == nullptr) {
		singleton = std::make_unique<GameContext>();
	}
	return singleton.get();
}
