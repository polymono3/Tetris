#include "PlayButton.h"
#include "Game.h"
#include "Texture.h"

PlayButton::PlayButton(Game* game, int x, int y, int w, int h) : Button(game, x, y, w, h)
{

}

void PlayButton::HandleEvent(SDL_Event* e)
{
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);

		bool inside = true;
		if (x < mRect.x ||
			x > mRect.x + mRect.w ||
			y < mRect.y ||
			y > mRect.y + mRect.h)
		{
			inside = false;
		}

		if (!inside)
		{
			mCurrentSprite = BUTTON_MOUSE_OUT;
		}
		else
		{
			mCurrentSprite = BUTTON_MOUSE_OVER;
			if (e->type == SDL_MOUSEBUTTONDOWN)
			{
				mGame->SetNextState(Game::STATE_PLAY);
			}
		}
	}
}