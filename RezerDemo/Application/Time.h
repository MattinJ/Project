#pragma once

#include <iostream>
#include <chrono>

class Engine;

class Time
{
private:
	friend class Engine;
	friend class SceneHandler;

	static float deltaTime;
	static float timeSinceStart;
	static bool oneSecondPassed;

	static std::chrono::system_clock::time_point lastTime;
	static std::chrono::system_clock::time_point currentTime;
	static std::chrono::duration<float> elapsedSeconds;

	static void init();
	static void updateDeltaTime();

public:
	Time();
	~Time();

	static const inline float& getDT() { return deltaTime; };
	static const inline float& getTimeSinceStart() { return timeSinceStart; }
	static const inline bool& hasOneSecondPassed() { return oneSecondPassed; }
};

