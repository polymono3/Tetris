#pragma once
#include "GameState.h"

class TitleState : public GameState
{
public:
	TitleState(class Game* game);
	~TitleState();

	void HandleEvents() override;
	void Logic() override;
	void Render() override;

private:
	class Texture* mTitleImage;
	class PlayButton* mPlayButton;
};