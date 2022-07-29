#pragma once

#include <chrono>

#include "Window.h"
#include "../Graphics/Graphics.h"

class Engine
{
private:
	Window window;
	Graphics graphic;

public:
	Engine();
	virtual ~Engine();

	void run();
};

