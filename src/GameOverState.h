#pragma once
#include "GameState.h"

class GameOverState : public GameState
{
public:
	GameOverState(class Game* game);
	~GameOverState();

	void HandleEvents() override;
	void Logic() override;
	void Render() override;

private:
	class Texture* mGameOverImage;
	class PlayButton* mRetryButton;
	class Texture* mScoreText;
	int mScore;
	int mHighScore;
	bool mSoundPlayed;
};