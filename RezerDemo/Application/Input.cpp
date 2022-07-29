#include "Input.h"

bool Input::keys[] = { false };
bool Input::lastPressedKeys[] = { false };
bool Input::mouseButtons[] = { false };
bool Input::lastPressedMouseButtons[] = { false };
bool Input::lockCursorPos = false;
bool Input::cursorVisible = true;
bool Input::lastCursorVisible = true;
int Input::cursorX = 0;
int Input::cursorY = 0;
int Input::cursorDeltaX = 0;
int Input::cursorDeltaY = 0;

Input::Input()
{
	for (unsigned int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		keys[i] = false;
		lastPressedKeys[i] = false;
	}
}

Input::~Input()
{
}

void Input::updateLastPressedKeys()
{
	for (unsigned int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		lastPressedKeys[i] = keys[i];
	}
}

void Input::updateLastPressedMouseButtons()
{
	for (unsigned int i = 0; i < MAX_NUM_MOUSE_BUTTONS; ++i)
	{
		lastPressedMouseButtons[i] = mouseButtons[i];
	}
}

void Input::updateLastCursorVisible()
{
	lastCursorVisible = cursorVisible;
}

void Input::setKeyDown(unsigned int keyCode)
{
	keys[keyCode] = true;
}

void Input::setKeyUp(unsigned int keyCode)
{
	keys[keyCode] = false;
}

void Input::setMouseDown(unsigned int mouseCode)
{
	mouseButtons[mouseCode] = true;
}

void Input::setMouseUp(unsigned int mouseCode)
{
	mouseButtons[mouseCode] = false;
}

void Input::setLockCursorPosition(bool setLock)
{
	lockCursorPos = setLock;
}

void Input::setCursorVisible(bool setVisibility)
{
	cursorVisible = setVisibility;
}

void Input::updateCursorPosition(int newX, int newY)
{
	this->cursorX = newX;
	this->cursorY = newY;
}

void Input::updateCursorDelta(int newDeltaX, int newDeltaY)
{
	this->cursorDeltaX = newDeltaX;
	this->cursorDeltaY = newDeltaY;
}

bool Input::isKeyDown(Keys keyCode)
{
	return keys[(unsigned int)keyCode];
}

bool Input::isKeyJustPressed(Keys keyCode)
{
	return keys[(unsigned int)keyCode] && !lastPressedKeys[(unsigned int)keyCode];
}

bool Input::isKeyReleased(Keys keyCode)
{
	return !keys[(unsigned int)keyCode] && lastPressedKeys[(unsigned int)keyCode];
}

bool Input::isMouseButtonDown(Mouse mouseCode)
{
	return mouseButtons[(unsigned int)mouseCode];
}

bool Input::isMouseButtonJustPressed(Mouse mouseCode)
{
	return mouseButtons[(unsigned int)mouseCode] && !lastPressedMouseButtons[(unsigned int)mouseCode];
}

bool Input::isMouseButtonReleased(Mouse mouseCode)
{
	return !mouseButtons[(unsigned int)mouseCode] && lastPressedMouseButtons[(unsigned int)mouseCode];
}

bool Input::shouldLockCursor()
{
	return lockCursorPos;
}

bool Input::isCursorVisible()
{
	return cursorVisible;
}

bool Input::cursorVisibilitySwitched()
{
	return cursorVisible != lastCursorVisible;
}

int Input::getCursorX()
{
	return cursorX;
}

int Input::getCursorY()
{
	return cursorY;
}

int Input::getCursorDeltaX()
{
	return cursorDeltaX;
}

int Input::getCursorDeltaY()
{
	return cursorDeltaY;
}
