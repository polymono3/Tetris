#include "PlayState.h"
#include "Game.h"
#include "Texture.h"
#include "Constants.h"
#include <stdlib.h>
#include <utility>

PlayState::PlayState(Game* game) :
	GameState(game),
	mField(nullptr),
	mRotateHold(false),
	mHardDropHold(false),
	mMoveDown(false),
	mHardDrop(false),
	mAlreadySwapped(false),
	mFirstKeyPress(true),
	mSpeed(80),
	mSpeedCounter(0),
	mTotalLinesCleared(0),
	mScore(0),
	mScoreText(nullptr),
	mNextPieceID(0),
	mTimer(),
	mKeyPressCooldown(0)
{
	// Initialise playing field
	mField = new CELL_ENTRY[NUM_ROWS * NUM_COLUMNS];
	for (int x = 0; x < NUM_COLUMNS; x++)
	{
		for (int y = 0; y < NUM_ROWS; y++)
		{
			if (x == 0 || x == NUM_COLUMNS - 1 || y == NUM_ROWS - 1)
			{
				mField[y * NUM_COLUMNS + x] = BORDER;
			}
			else
			{
				mField[y * NUM_COLUMNS + x] = EMPTY;
			}
		}
	}

	mTetrominos =
	{
		// ID: 0
		{0, 0, 1, 0,
		 0, 0, 1, 0,
		 0, 0, 1, 0,
		 0, 0, 1, 0},
		// 1
		{0, 0, 1, 0,
		 0, 1, 1, 0,
		 0, 1, 0, 0,
		 0, 0, 0, 0},
		// 2
		{0, 1, 0, 0,
		 0, 1, 1, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 0},
		// 3
		{0, 0, 0, 0,
		 0, 1, 1, 0,
		 0, 1, 1, 0,
		 0, 0, 0, 0},
		// 4
		{0, 0, 1, 0,
		 0, 1, 1, 0,
		 0, 0, 1, 0,
		 0, 0, 0, 0},
		// 5
		{0, 0, 0, 0,
		 0, 1, 1, 0,
		 0, 0, 1, 0,
		 0, 0, 1, 0},
		// 6
		{0, 0, 0, 0,
		 0, 1, 1, 0,
		 0, 1, 0, 0,
		 0, 1, 0, 0},
	};

	mColours = 
	{
		// Tetrominos
		// 0
		{ 66, 239, 245, 255},
		// 1
		{255,   0,   0, 255},
		// 2
		{  0, 255,   0, 255},
		// 3
		{255, 238,   0, 255},
		// 4
		{187,   0, 255, 255},
		// 5
		{255, 149,   0, 255},
		// 6
		{  0,   0, 255, 255},
	
		// Other
		// 7: border cell
		{122, 122, 122, 255},
		// 8: line clear
		{255, 255, 255, 255}
	};

	mScoreText = new Texture(mGame->GetRenderer());

	// Configure current and next tetromino pieces
	mNextPieceID = rand() % 7;
	mCurrentPiece.mID = rand() % 7;
	mCurrentPiece.mPosX = NUM_COLUMNS / 2 - 2;
	mCurrentPiece.mRotation = rand() % 4;
}

PlayState::~PlayState()
{
	delete[] mField;
	mField = nullptr;

	delete mScoreText;
	mScoreText = nullptr;
}

bool PlayState::PieceFits(int tetromino, int rotation, int posX, int posY)
{
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			// Get index into tetromino
			int ti = Rotate(x, y, rotation);

			// Get index into field
			int fi = (posY + y) * NUM_COLUMNS + (posX + x);

			if (posX + x >= 0 && posX + x < NUM_COLUMNS)
			{
				if (posY + y >= 0 && posY + y < NUM_ROWS)
				{
					// Check for collision
					if (mTetrominos[tetromino][ti] == 1 && mField[fi] != EMPTY)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

int PlayState::Rotate(int x, int y, int r)
{
	int newIndex = 0;
	switch (r % 4)
	{
		case 0: // 0 degrees
			newIndex = 4 * y + x;
			break;
		case 1: // 90 degrees
			newIndex = 12 + y - 4 * x;
			break;
		case 2: // 180 degrees
			newIndex = 15 - 4 * y - x;
			break;
		case 3: // 270 degrees
			newIndex = 3 - y + 4 * x;
			break;
	}

	return newIndex;
}

void PlayState::HandleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
		{
			mGame->SetNextState(mGame->STATE_QUIT);
		}
	}

	const Uint8* keyState = SDL_GetKeyboardState(nullptr);
	
	// Movement
	mCurrentPiece.mVelX = 0;
	mCurrentPiece.mVelY = 0;
	if (keyState[SDL_SCANCODE_A] && PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX - 1, mCurrentPiece.mPosY))
	{
		mCurrentPiece.mVelX = -1;
	}
	else if (keyState[SDL_SCANCODE_D] && PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX + 1, mCurrentPiece.mPosY))
	{
		mCurrentPiece.mVelX = 1;
	}
	else if (keyState[SDL_SCANCODE_S] && PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX, mCurrentPiece.mPosY + 1))
	{
		mCurrentPiece.mVelY = 1;
	}

	// Rotation
	mCurrentPiece.mRotating = false;
	if (keyState[SDL_SCANCODE_LSHIFT] &&
		PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation + 1, mCurrentPiece.mPosX, mCurrentPiece.mPosY))
	{
		// Prevent endless spinning when rotate key is held down
		if (!mRotateHold)
		{
			mCurrentPiece.mRotating = true;
			mRotateHold = true;
		}
	}
	else
	{
		mRotateHold = false;
	}

	// Hard drop
	mHardDrop = false;
	if (keyState[SDL_SCANCODE_SPACE])
	{
		// Prevent too many hard drops happening within a short time period
		if (!mHardDropHold)
		{
			mHardDrop = true;
			for (int y = mCurrentPiece.mPosY; y < NUM_ROWS - 1; y++)
			{
				if (!PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX, y))
				{
					mCurrentPiece.mVelY = y - mCurrentPiece.mPosY - 1;
					break;
				}
			}
			mHardDropHold = true;
		}
	}
	else
	{
		mHardDropHold = false;
	}

	// Swap with upcoming piece
	if (keyState[SDL_SCANCODE_F] && 
		PieceFits(mNextPieceID, 0, mCurrentPiece.mPosX, mCurrentPiece.mPosY) &&
		!mAlreadySwapped)
	{
		std::swap(mNextPieceID, mCurrentPiece.mID);
		mAlreadySwapped = true;
	}
}

void PlayState::Logic()
{
	// Calculate time (in milliseconds) since last frame
	int deltaTime = mTimer.GetTicks();
	mTimer.Start();

	mKeyPressCooldown -= deltaTime;

	mGame->PlayMusic("audio/background_music.wav");
	mSpeedCounter++;

	if (mHardDrop)
	{
		mCurrentPiece.mPosY += mCurrentPiece.mVelY;
	}
	else if (mCurrentPiece.mVelX == 0 && mCurrentPiece.mVelY == 0)
	{
		mFirstKeyPress = true;
	}
	else
	{
		if (mFirstKeyPress)
		{
			mGame->PlaySound("audio/shift_piece.wav");
			mCurrentPiece.mPosX += mCurrentPiece.mVelX;
			mCurrentPiece.mPosY += mCurrentPiece.mVelY;
			mKeyPressCooldown = FIRST_KEY_PRESS_COOLDOWN;
			mFirstKeyPress = false;
		}
		else if (mKeyPressCooldown <= 0)
		{
			mGame->PlaySound("audio/shift_piece.wav");
			mCurrentPiece.mPosX += mCurrentPiece.mVelX;
			mCurrentPiece.mPosY += mCurrentPiece.mVelY;
			mKeyPressCooldown = KEY_PRESS_COOLDOWN;
		}
	}

	mCurrentPiece.mRotation += (mCurrentPiece.mRotating ? 1 : 0); 
	mMoveDown = (mSpeedCounter == mSpeed) || mHardDrop;

	if (mMoveDown)
	{
		if (PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX, mCurrentPiece.mPosY + 1))
		{
			mCurrentPiece.mPosY++;
		}
		else
		{
			// Lock tetromino into field
			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (mTetrominos[mCurrentPiece.mID][Rotate(x, y, mCurrentPiece.mRotation)] == 1)
					{
						mGame->PlaySound("audio/drop.wav");
						mField[(mCurrentPiece.mPosY + y) * NUM_COLUMNS + mCurrentPiece.mPosX + x] = static_cast<CELL_ENTRY>(mCurrentPiece.mID);
					}
				}
			}
			mAlreadySwapped = false;

			// Check if we have any lines
			for (int y = 0; y < 4; y++)
			{
				// first check if we're inbounds
				if (mCurrentPiece.mPosY + y < NUM_ROWS - 1)
				{
					bool line = true;
					for (int x = 1; x < NUM_COLUMNS - 1; x++)
					{
						// If any cell in this row is empty, then we haven't cleared a line for this row
						if (mField[(mCurrentPiece.mPosY + y) * NUM_COLUMNS + x] == EMPTY)
						{
							line = false;
						}
					}

					if (line)
					{
						mTotalLinesCleared++;

						// Update the playing field
						for (int x = 1; x < NUM_COLUMNS - 1; x++)
						{
							mField[(mCurrentPiece.mPosY + y) * NUM_COLUMNS + x] = LINE_CLEAR;
						}
						mLinesCleared.push_back(mCurrentPiece.mPosY + y);

						int previousLevel = (mTotalLinesCleared - 1) / 10;
						int currentLevel = mTotalLinesCleared / 10;

						// The difficulty increases after every 10 lines cleared
						if (currentLevel - previousLevel == 1)
						{
							// prevent the game getting too fast
							if (mSpeed >= 10)
							{
								mSpeed -= 10;
							}
						}
					}
				}
			}

			// Scoring
			mScore += 25;
			if (!mLinesCleared.empty())
			{
				// Score increases exponentially for each line cleared
				mScore += (1 << mLinesCleared.size()) * 100;
			}

			// Choose next piece
			mCurrentPiece.mID = mNextPieceID;
			mNextPieceID = rand() % 7;
			mCurrentPiece.mPosX = NUM_COLUMNS / 2 - 2;
			mCurrentPiece.mPosY = 0;
			mCurrentPiece.mRotation = 0;

			// If piece doesn't fit, game over
			if (!PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX, mCurrentPiece.mPosY))
			{
				mGame->mScore = mScore;
				if (mScore > mGame->mHighScore)
				{
					mGame->mHighScore = mScore;
				}
				Mix_HaltMusic();
				mGame->SetNextState(Game::STATE_GAMEOVER);
			}
		}

		mSpeedCounter = 0;
	}
}

void PlayState::Render()
{
	// Clear screen
	SDL_SetRenderDrawColor(mGame->GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(mGame->GetRenderer());

	// Scoreboard
	mScoreText->CreateFromText("Score: " + std::to_string(mScore), mGame->GetFont("fonts/font.ttf", 28), SDL_Color{ 255,255,255 });
	mScoreText->Render(480 + (200 - mScoreText->GetWidth()) / 2, 50);

	// Playing field
	for (int x = 0; x < NUM_COLUMNS; x++)
	{
		for (int y = 0; y < NUM_ROWS; y++)
		{
			// Don't need to render anything for empty cells
			if (mField[y * NUM_COLUMNS + x] != EMPTY)
			{
				int index = mField[y * NUM_COLUMNS + x];
				SDL_Color colour = mColours[index];
				DrawCell(x, y, colour);
			}
		}
	}

	// Render current piece and a preview of where it will drop
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (mTetrominos[mCurrentPiece.mID][Rotate(x, y, mCurrentPiece.mRotation)] != 0)
			{
				// current piece
				SDL_Color colour = mColours[mCurrentPiece.mID];
				DrawCell(mCurrentPiece.mPosX + x, mCurrentPiece.mPosY + y, colour);

				// preview
				for (int previewY = 0; previewY < NUM_ROWS - 1; previewY++)
				{
					if (!PieceFits(mCurrentPiece.mID, mCurrentPiece.mRotation, mCurrentPiece.mPosX, previewY))
					{
						DrawCellOutline(mCurrentPiece.mPosX + x, previewY - 1 + y, colour);
						break;
					}
				}
			}
		}
	}

	// If a line is cleared, animate its removal
	if (!mLinesCleared.empty())
	{
		SDL_RenderPresent(mGame->GetRenderer());
		mGame->Delay(400);
		bool maxLinesCleared = (mLinesCleared.size() == 4);
		for (auto& line : mLinesCleared)
		{
			for (int x = 1; x < NUM_COLUMNS - 1; x++)
			{
				for (int y = line; y > 0; y--)
				{
					mField[y * NUM_COLUMNS + x] = mField[(y - 1) * NUM_COLUMNS + x];
				}
				mField[x] = EMPTY;
			}

			mLinesCleared.clear();
		}
		maxLinesCleared ? mGame->PlaySound("audio/max_line_clear.wav") : mGame->PlaySound("audio/line_clear.wav");
	}

	// Render upcoming tetromino (off to the side)
	for (int x = 0; x < 4; x++)
	{
		for (int y = 0; y < 4; y++)
		{
			if (mTetrominos[mNextPieceID][Rotate(x, y, 0)] != 0)
			{
				SDL_Color colour = mColours[mNextPieceID];
				DrawCell(NUM_COLUMNS + x, 3 + y, colour);
			}
		}
	}

	// Swap front and back buffer
	SDL_RenderPresent(mGame->GetRenderer());
}

void PlayState::DrawCell(int x, int y, SDL_Color colour)
{
	SDL_Rect r = { x * CELL_WIDTH, y * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT };
	SDL_SetRenderDrawColor(mGame->GetRenderer(), colour.r, colour.g, colour.b, colour.a);
	SDL_RenderFillRect(mGame->GetRenderer(), &r);
	SDL_SetRenderDrawColor(mGame->GetRenderer(), 55, 55, 55, 255);
	SDL_RenderDrawRect(mGame->GetRenderer(), &r);
}

void PlayState::DrawCellOutline(int x, int y, SDL_Color colour)
{
	SDL_Rect r = { x * CELL_WIDTH,y * CELL_HEIGHT,CELL_WIDTH,CELL_HEIGHT };
	SDL_SetRenderDrawColor(mGame->GetRenderer(), colour.r, colour.g, colour.b, colour.a);
	SDL_RenderDrawRect(mGame->GetRenderer(), &r);
}