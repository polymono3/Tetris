#include "Button.h"
#include "Game.h"
#include "Texture.h"

Button::Button(Game* game, int x, int y, int w, int h) : mGame(game), mCurrentSprite(BUTTON_MOUSE_OUT)
{
	mRect = { x,y,w,h };
}

Button::~Button()
{
}

void Button::SetPosition(int x, int y)
{
	mRect.x = x;
	mRect.y = y;
}

void Button::Render()
{
	mSpriteMap[mCurrentSprite]->Render(mRect.x, mRect.y, mRect.w, mRect.h);
}

void Button::SetSprite(ButtonSprite sprite, const std::string& spritePath)
{
	mSpriteMap[sprite] = mGame->GetTexture(spritePath);
}