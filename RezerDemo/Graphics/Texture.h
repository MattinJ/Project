#pragma once
#include <Windows.h>
#include <d3d11.h>
#include "string"

class Graphics;

class Texture
{
private:
	Graphics& graphic;
	ID3D11Texture2D* texture;
	D3D11_TEXTURE2D_DESC textureDesc;

	ID3D11ShaderResourceView* srv;
	ID3D11SamplerState* samplerState;
	
	int textureWidth;
	int textureHeight;
	int textureChannels;

	bool createSampler();

public:
	Texture(Graphics& graphic);
	virtual ~Texture();

	bool loadTexture(std::string fileName);

	inline ID3D11Texture2D*& getTexture() { return this->texture; }
	inline ID3D11ShaderResourceView*& getSRV() { return this->srv; }
	inline ID3D11SamplerState*& getSamplerState() { return this->samplerState; }
	
	inline const int& getTextureWidth() { return this->textureWidth; }
	inline const int& getTextureHeight() { return this->textureHeight; }
	inline const int& getTextureChannels() { return this->textureChannels; }

};

