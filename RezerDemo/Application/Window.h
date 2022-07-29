#pragma once

#include <string>
#include <Windows.h>
#include "Input.h"

class Window
{
private:
	unsigned int width;
	unsigned int height;

	Input input;

	HWND windowHandle;
	WNDCLASS wc;

	POINT cursorPoint;
	POINT lastCursorPoint;
	POINT screenMiddlePoint;

	std::wstring title;

	bool running;
	bool fullscreen;

public:
	Window();
	~Window();

	bool init(
		unsigned int width,
		unsigned int height,
		const std::wstring title,
		const bool isFullscreen = false
	);
	const bool& isRunning();

	void setTitle(const std::string& title);
	void quit();

	inline HWND& getWindowHandle() { return this->windowHandle; }
	inline const bool isFocus() { return GetFocus() != NULL; }
	inline const bool& getIsFullscreen() { return this->fullscreen; }
	inline const unsigned int getWidth() { return this->width; }
	inline const unsigned int getHeight() { return this->height; }
};

