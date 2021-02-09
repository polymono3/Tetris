#pragma once
#include "GameState.h"
#include <vector>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <unordered_map>
#include "Timer.h"

struct Tetromino
{
	int mID = 0;
	int mRotation = 0;
	int mPosX = 0;
	int mPosY = 0;
	int mVelX = 0;
	int mVelY = 0;
	bool mRotating = false;
};

class PlayState : public GameState
{
public:
	PlayState(class Game* game);
	~PlayState();

	void HandleEvents() override;
	void Logic() override;
	void Render() override;

private:
	// Utility functions
	bool PieceFits(int tetromino, int rotation, int posX, int posY);
	int Rotate(int x, int y, int r);
	void DrawCell(int x, int y, SDL_Color colour);
	void DrawCellOutline(int x, int y, SDL_Color colour);

	// Playing field
	enum CELL_ENTRY {I, Z, S, O, T, L, J, BORDER, LINE_CLEAR, EMPTY};
	CELL_ENTRY* mField;

	// Flags
	bool mRotateHold;
	bool mHardDropHold;
	bool mMoveDown;
	bool mHardDrop;
	bool mAlreadySwapped;
	bool mFirstKeyPress;

	// Difficulty parameters
	int mSpeed;
	int mSpeedCounter;
	int mTotalLinesCleared;

	// Score
	int mScore;
	class Texture* mScoreText;

	// Array of the different tetrominos
	std::vector<std::vector<int>> mTetrominos;

	int mNextPieceID;

	std::vector<SDL_Color> mColours;

	Tetromino mCurrentPiece;
	
	std::vector<int> mLinesCleared;

	Timer mTimer;
	int mKeyPressCooldown;
};