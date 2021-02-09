#include "Texture.h"
#include <stdio.h>
#include <SDL_image.h>

void Texture::Free()
{
	if (mTexture != nullptr)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = mHeight = 0;
	}
}

bool Texture::LoadFromFile(const std::string& path)
{
	Free();

	SDL_Texture* newTexture = nullptr;

	SDL_Surface* surface = IMG_Load(path.c_str());
	if (!surface)
	{
		printf("Failed to load image at %s. Error: %s\n", path.c_str(), SDL_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(mRenderer, surface);
		if (!newTexture)
		{
			printf("Failed to create texture from image %s. Error: %s\n", path.c_str(), IMG_GetError());
		}
		else
		{
			mWidth = surface->w;
			mHeight = surface->h;
		}

		SDL_FreeSurface(surface);
	}
	

	mTexture = newTexture;
	return mTexture != nullptr;
}

bool Texture::CreateFromText(const std::string& text, TTF_Font* font, SDL_Color colour)
{
	Free();

	SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), colour);
	if (!textSurface)
	{
		printf("Failed to create surface for text. Error: %s\n", TTF_GetError());
	}
	else
	{
		mTexture = SDL_CreateTextureFromSurface(mRenderer, textSurface);
		if (!mTexture)
		{
			printf("Failed to create texture from surface. Error: %s\n", SDL_GetError());
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface(textSurface);
	}
	
	return mTexture != nullptr;
}

void Texture::Render(int x, int y, SDL_Rect* clip)
{
	SDL_Rect dstRect = { x,y,mWidth,mHeight };
	SDL_RenderCopy(mRenderer, mTexture, clip, &dstRect);
}

void Texture::Render(int x, int y, int w, int h, SDL_Rect* clip)
{
	SDL_Rect dstRect = { x,y,w,h };
	SDL_RenderCopy(mRenderer, mTexture, clip, &dstRect);
}