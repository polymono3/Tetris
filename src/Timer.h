#pragma once

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Pause();
	void Unpause();

	// Returns the time (in milliseconds) passed since timer started
	int GetTicks();

	bool IsStarted() { return mStarted;  }
	bool IsPaused() { return mPaused && mStarted; }

private:
	int mStartTicks;
	int mPausedTicks;
	bool mPaused;
	bool mStarted;
};