#pragma once

#include "Shaders.h"

class PixelShader : public Shaders
{
private:
	ID3D11PixelShader* pixelShader;

public:
	PixelShader(Graphics& graphic);
	virtual ~PixelShader();

	bool loadPS(const std::wstring fileName);

	inline ID3D11PixelShader*& getPS() { return this->pixelShader; }
};

