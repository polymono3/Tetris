#include "TitleState.h"
#include "Texture.h"
#include "Window.h"
#include "Game.h"
#include "PlayButton.h"

TitleState::TitleState(Game* game) : 
	GameState(game),
	mTitleImage(nullptr),
	mPlayButton(nullptr)
{
	mTitleImage = game->GetTexture("images/title_screen.png");

	mPlayButton = new PlayButton(game, (game->mWidth - 150) / 2, 650, 150, 50);
	mPlayButton->SetSprite(Button::BUTTON_MOUSE_OUT, "images/play_button.png");
	mPlayButton->SetSprite(Button::BUTTON_MOUSE_OVER, "images/play_button_mouseover.png");
}

TitleState::~TitleState()
{
	delete mPlayButton;
}

void TitleState::HandleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
		{
			mGame->SetNextState(mGame->STATE_QUIT);
		}

		mPlayButton->HandleEvent(&e);
	}
}

void TitleState::Logic()
{

}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(mGame->mRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mGame->mRenderer);

	mTitleImage->Render(0, 0);
	mPlayButton->Render();

	SDL_RenderPresent(mGame->mRenderer);
}