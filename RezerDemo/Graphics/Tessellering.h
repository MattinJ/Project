#pragma once
#include <d3d11.h>

#include "Shaders/VertexShader.h"
#include "Shaders/InputLayoutDesc.h"

class Graphics;

class Tessellering
{
private:
	Graphics& graphic;

	VertexShader vertexShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11RasterizerState* rasterState;

	bool loadShaders();

public:
	Tessellering(Graphics& graphic);
	~Tessellering();

	bool init();

	inline VertexShader& getVertexShader() { return this->vertexShader; }
	inline ID3D11HullShader*& getHullShader() { return this->hullShader; }
	inline ID3D11DomainShader*& getDomainShader() { return this->domainShader; }
	inline ID3D11RasterizerState*& getRasterWireState() { return this->rasterState; }

};

