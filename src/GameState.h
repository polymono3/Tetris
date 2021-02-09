#pragma once
#include <SDL.h>

class GameState
{
public:
	GameState(class Game* game) : mGame(game) {}
	virtual void HandleEvents() = 0;
	virtual void Logic() = 0;
	virtual void Render() = 0;
	virtual ~GameState() {}

protected:
	class Game* mGame;
};