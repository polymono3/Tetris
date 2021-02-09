#include "Timer.h"
#include <SDL.h>

Timer::Timer() : 
	mStartTicks(0),
	mPausedTicks(0),
	mPaused(false),
	mStarted(false)
{

}

void Timer::Start()
{
	mStarted = true;
	mPaused = false;

	mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void Timer::Stop()
{
	mStarted = false;
	mPaused = true;

	mStartTicks = 0;
	mPausedTicks = 0;
}

void Timer::Pause()
{
	if (mStarted && !mPaused)
	{
		mPaused = true;

		mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
	}
}

void Timer::Unpause()
{
	if (mStarted && mPaused)
	{
		mPaused = false;

		mStartTicks = SDL_GetTicks() - mPausedTicks;
		mPausedTicks = 0;
	}
}

int Timer::GetTicks()
{
	int time = 0;

	if (mStarted)
	{
		if (mPaused)
		{
			time = mPausedTicks;
		}
		else
		{
			time = SDL_GetTicks() - mStartTicks;
		}
	}

	return time;
}