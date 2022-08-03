#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include "Mesh.h"

#include "Shaders/PixelShader.h"
#include "Shaders/VertexShader.h"
#include "Shaders/InputLayoutDesc.h"

const int VIEW_SIZE = 6;

class Graphics;

class CubeMap
{
private:	
	DirectX::SimpleMath::Matrix viewMatrix[VIEW_SIZE];

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* uav;
	ID3D11ShaderResourceView* srv;
	ID3D11RenderTargetView* rtvArray[VIEW_SIZE];
	D3D11_VIEWPORT vp;

	Graphics& graphic;

	PixelShader pixelShader;
	VertexShader vertexShader;

	UINT textureSize;

	Mesh mesh;

	DirectX::SimpleMath::Vector3 position;

	void initVP();
	void initShaders();

public:
	CubeMap(Graphics& graphic);
	~CubeMap();

	bool init();

	inline ID3D11Texture2D*& getTexture() { return this->texture; }
	inline ID3D11UnorderedAccessView*& getUAV(int index) { return this->uav; }
	inline ID3D11ShaderResourceView*& getSRV() { return this->srv; }
	inline D3D11_VIEWPORT& getVP() { return this->vp; }

	inline ID3D11RenderTargetView*& getRTV(int index) { return this->rtvArray[index]; }

	inline DirectX::SimpleMath::Matrix& getVPMatrix(int index) { return this->viewMatrix[index]; }

	inline PixelShader& getPixelShader() { return this->pixelShader; }
	inline VertexShader& getVertexShader() { return this->vertexShader; }

	inline Mesh& getMesh() { return this->mesh; }

};

