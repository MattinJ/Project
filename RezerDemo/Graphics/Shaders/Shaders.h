#pragma once
#include <string>
#include <d3d11.h>

class Graphics;

class Shaders
{
private:
	Graphics& graphic;

protected:
	bool loadShader(std::wstring shaderFile, ID3DBlob*& blob);

	inline Graphics& getGraphics() { return this->graphic; }

public:
	Shaders(Graphics& graphic);
	virtual ~Shaders();
};

