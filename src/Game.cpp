#include "Game.h"
#include <stdio.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include "Timer.h"
#include "GameState.h"
#include "TitleState.h"
#include <stdlib.h>
#include <time.h>
#include "Texture.h"
#include "PlayState.h"
#include "Constants.h"
#include "GameOverState.h"

Game::Game(int width, int height) :
	mWindow(nullptr),
	mRenderer(nullptr),
	mWidth(width),
	mHeight(height),
	mCurrentState(nullptr),
	mStateID(0),
	mNextStateID(0),
	mInitialised(false),
	mScore(0),
	mHighScore(0)
{
	if (!Initialise())
	{
		printf("Failed to initialise game.\n");
	}
	else
	{
		mInitialised = true;
	}

	mCurrentState = new TitleState(this);

	srand(static_cast<unsigned int>(time(nullptr)));
}

bool Game::Initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL failed to initialise. Error: %s\n", SDL_GetError());
		return false;
	}

	mWindow = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, SDL_WINDOW_SHOWN);
	if (!mWindow)
	{
		printf("Failed to create window. Error: %s\n", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED);
	if (!mRenderer)
	{
		printf("Failed to create renderer. Error: %s\n", SDL_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer failed to initialise. Error: %s\n", Mix_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf failed to initialise. Error: %s\n", TTF_GetError());
		return false;
	}

	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image failed to initialise. Error: %s\n", IMG_GetError());
		return false;
	}

	return true;
}

void Game::Close()
{
	// Textures
	for (auto& i : mTextures)
	{
		delete i.second;
		i.second = nullptr;
	}
	mTextures.clear();

	// SoundFX
	for (auto& i : mSoundFX)
	{
		Mix_FreeChunk(i.second);
		i.second = nullptr;
	}
	mSoundFX.clear();

	// Music
	for (auto& i : mMusic)
	{
		Mix_FreeMusic(i.second);
		i.second = nullptr;
	}
	mMusic.clear();

	// Fonts
	for (auto& i : mFonts)
	{
		TTF_CloseFont(i.second);
		i.second = nullptr;
	}
	mFonts.clear();

	// Window
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	mWindow = nullptr;
	mRenderer = nullptr;

	delete mCurrentState;

	// Quit subsystems
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();
	SDL_Quit();
}

void Game::Delay(int time)
{
	int timeout = SDL_GetTicks() + time;
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), timeout));
}

Texture* Game::GetTexture(const std::string& path)
{
	if (!mTextures[path])
	{
		mTextures[path] = new Texture(mRenderer);
		if (!(mTextures[path]->LoadFromFile(path)))
		{
			printf("Failed to load texture at %s.\n", path.c_str());
		}
	}

	return mTextures[path];
}

Mix_Chunk* Game::GetSound(const std::string& path)
{
	if (!mSoundFX[path])
	{
		mSoundFX[path] = Mix_LoadWAV(path.c_str());
		if (!mSoundFX[path])
		{
			printf("Failed to load sound at %s. Error: %s\n", path.c_str(), Mix_GetError());
		}
	}

	return mSoundFX[path];
}

Mix_Music* Game::GetMusic(const std::string& path)
{
	if (!mMusic[path])
	{
		mMusic[path] = Mix_LoadMUS(path.c_str());
		if (!mMusic[path])
		{
			printf("Failed to load music at %s. Error: %s\n", path.c_str(), Mix_GetError());
		}
	}

	return mMusic[path];
}

TTF_Font* Game::GetFont(const std::string& path, int fontSize)
{
	if (!mFonts[path])
	{
		mFonts[path] = TTF_OpenFont(path.c_str(), fontSize);
		if (!mFonts[path])
		{
			printf("Failed to load font at %s. Error: %s\n", path.c_str(), TTF_GetError());
		}
	}

	return mFonts[path];
}

void Game::Run()
{
	if (mInitialised)
	{
		Timer fpsTimer;
		while (mStateID != STATE_QUIT)
		{
			fpsTimer.Start();

			mCurrentState->HandleEvents();
			mCurrentState->Logic();
			ChangeState();
			mCurrentState->Render();

			int frameTicks = fpsTimer.GetTicks();
			if (frameTicks < 1000 / FPS)
			{
				SDL_Delay(1000 / FPS - frameTicks);
			}
		}
	}
	else
	{
		printf("Error: cannot run uninitialised game.\n");
	}
}

void Game::SetNextState(int newNextStateID)
{
	if (mNextStateID != STATE_QUIT)
	{
		mNextStateID = newNextStateID;
	}
}

void Game::ChangeState()
{
	if (mNextStateID != STATE_NULL)
	{
		if (mNextStateID != STATE_QUIT)
		{
			delete mCurrentState;
		}

		switch (mNextStateID)
		{
			case STATE_PLAY:
				mCurrentState = new PlayState(this);
				break;
			case STATE_GAMEOVER:
				mCurrentState = new GameOverState(this);
				break;
		}

		mStateID = mNextStateID;
		mNextStateID = STATE_NULL;
	}
}

void Game::PlaySound(const std::string& path)
{
	if (!mSoundFX[path])
	{
		mSoundFX[path] = Mix_LoadWAV(path.c_str());
		if (!mSoundFX[path])
		{
			printf("Failed to load sound at %s. Error: %s\n", path.c_str(), Mix_GetError());
			return;
		}
	}

	Mix_PlayChannel(-1, mSoundFX[path], 0);
}

void Game::PlayMusic(const std::string& path)
{
	if (!mMusic[path])
	{
		mMusic[path] = Mix_LoadMUS(path.c_str());
		if (!mMusic[path])
		{
			printf("Failed to load music at %s. Error: %s\n", path.c_str(), Mix_GetError());
			return;
		}
	}

	if (!Mix_PlayingMusic())
	{
		Mix_PlayMusic(mMusic[path], -1);
	}
}