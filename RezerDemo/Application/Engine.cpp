#include "Engine.h"
#include "ErrorLogger.h"
#include "Settings.h"
#include "Time.h"

Engine::Engine()
	:graphic()
{
	this->window.init(s_resolusionX, s_resolusionY, s_title, false);
	this->graphic.init(this->window);
}

Engine::~Engine()
{
}

void Engine::run()
{
	auto lastTime = std::chrono::high_resolution_clock::now();

	while (this->window.isRunning())
	{
		// Track delta time
		Time::updateDeltaTime();

		// ---------- Start tracking time
		//lastTime = std::chrono::high_resolution_clock::now();

		// render
		this->graphic.render();

		// ---------- Stop tracking time
		//std::chrono::duration<double, std::milli> fp_ms = std::chrono::high_resolution_clock::now() - lastTime;
		this->window.setTitle("My project: " + std::to_string((int)(1.0f / Time::getDT())) + " FPS | DT " + std::to_string(Time::getDT() * 1000.0f) + " ms");

		// Present
		this->graphic.presentSC();
	}
}
