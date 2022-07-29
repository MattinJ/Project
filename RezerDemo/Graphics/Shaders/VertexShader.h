#pragma once
#include "Shaders.h"
#include "InputLayoutDesc.h"

class VertexShader : public Shaders
{
private:
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* inputLayout;

	bool createInputLayout(InputLayoutDesc& inputLayoutDesc, ID3DBlob*& blob);

public:
	VertexShader(Graphics& graphic);
	virtual ~VertexShader();

	bool loadVS(std::wstring file, InputLayoutDesc& inputLayoutDesc);

	inline ID3D11VertexShader*& getVS() { return this->vertexShader; }
	inline ID3D11InputLayout*& getInputLayout() { return this->inputLayout; }


};

