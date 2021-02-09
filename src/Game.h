#pragma once
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

class Game
{
	friend class GameState;
	friend class TitleState;
	friend class PlayState;
	friend class GameOverState;
	friend class PlayButton;
public:
	Game(int width, int height);
	~Game() { Close(); }

	SDL_Renderer* GetRenderer() const { return mRenderer; }

	class Texture* GetTexture(const std::string& path);
	Mix_Chunk* GetSound(const std::string& path);
	Mix_Music* GetMusic(const std::string& path);
	TTF_Font* GetFont(const std::string& path, int fontSize);

	void PlaySound(const std::string& path);
	void PlayMusic(const std::string& path);
	
	void Run();

private:
	// Startup/shutdown
	bool Initialise();
	void Close();

	// Utility
	void Delay(int time);

	// Media
	std::unordered_map<std::string, Texture*> mTextures;
	std::unordered_map<std::string, Mix_Chunk*> mSoundFX;
	std::unordered_map<std::string, Mix_Music*> mMusic;
	std::unordered_map<std::string, TTF_Font*> mFonts;

	// State
	void SetNextState(int newNextStateID);
	void ChangeState();
	enum States {STATE_NULL, STATE_TITLE, STATE_PLAY, STATE_GAMEOVER, STATE_QUIT};
	class GameState* mCurrentState;
	int mStateID;
	int mNextStateID;

	// Window
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	int mWidth;
	int mHeight;

	// Flags
	bool mInitialised;

	// Scoring
	int mScore;
	int mHighScore;
};