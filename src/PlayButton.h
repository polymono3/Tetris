#pragma once
#include "Button.h"

class PlayButton : public Button
{
public:
	PlayButton(class Game* game, int x, int y, int w, int h);
	virtual void HandleEvent(SDL_Event* e) override;
};