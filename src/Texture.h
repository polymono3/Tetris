#pragma once
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class Texture
{
public:
	Texture(SDL_Renderer* renderer) : mTexture(nullptr), mRenderer(renderer), mWidth(0), mHeight(0) {}
	void Free();
	bool LoadFromFile(const std::string& path);
	bool CreateFromText(const std::string& text, TTF_Font* font, SDL_Color colour);
	void Render(int x, int y, SDL_Rect* clip = nullptr);
	void Render(int x, int y, int w, int h, SDL_Rect* clip = nullptr);
	~Texture() { Free(); };

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

private:
	SDL_Texture* mTexture;
	SDL_Renderer* mRenderer;
	int mWidth;
	int mHeight;
};