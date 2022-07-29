#include "VertexShader.h"
#include "../Graphics.h"
#include "../../Application/ErrorLogger.h"

bool VertexShader::createInputLayout(InputLayoutDesc& inputLayoutDesc, ID3DBlob*& blob)
{
	HRESULT hr = this->getGraphics().getDevice()->CreateInputLayout(
		&inputLayoutDesc.getInputElementDesc().front(),
		(UINT)inputLayoutDesc.getInputElementDesc().size(),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&this->inputLayout
	);

	return ErrorLogger::hrError(hr, "Failed to create input layout.");
}

VertexShader::VertexShader(Graphics& graphic)
	:Shaders(graphic), vertexShader(nullptr), inputLayout(nullptr)
{
}

VertexShader::~VertexShader()
{
	if (this->vertexShader != nullptr)
		this->vertexShader->Release();

	if (this->inputLayout != nullptr)
		this->inputLayout->Release();
}

bool VertexShader::loadVS(std::wstring file, InputLayoutDesc& inputLayoutDesc)
{
	if (this->vertexShader != nullptr)
		this->vertexShader->Release();

	if (this->inputLayout != nullptr)
		this->inputLayout->Release();

	ID3DBlob* blob = nullptr;

	this->loadShader(file, blob);
	
	HRESULT hr = this->getGraphics().getDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->vertexShader);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create vertexShader.");
		return false;
	}

	bool inputReturn = false;

	if (inputLayoutDesc.getInputElementDesc().size() > 0)
	{
		inputReturn = this->createInputLayout(inputLayoutDesc, blob);
	}

	return inputReturn;
}
