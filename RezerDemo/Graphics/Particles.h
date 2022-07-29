#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

#include "Shaders/VertexShader.h"
#include "Shaders/PixelShader.h"
#include "Shaders/InputLayoutDesc.h"

class Graphics;

class Particles
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		float pad;

	}particleBuffer{};
	const int NR_OF_PARTICLES = 128;

	Graphics& graphic;

	VertexShader vertexShader;
	PixelShader pixelShader;
	ID3D11GeometryShader* geometryShader;

	ID3D11Buffer* vertexBuffer;
	UINT bufferSize;

	bool initShaders();
	bool initBuffers();

public:
	Particles(Graphics& graphic);
	~Particles();

	bool init();
	bool render();

};

