#include "Particles.h"
#include "Graphics.h"

#include "../Application/ErrorLogger.h"

#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib")

bool Particles::initShaders()
{
	//Vertex Shader
	InputLayoutDesc inputDesc;
	inputDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);

	this->vertexShader.loadVS(L"Particles_VS", inputDesc);

	//Pixel shader
	this->pixelShader.loadPS(L"Particles_PS");
	
	//Geometry shader
	if (this->geometryShader != nullptr)
		this->geometryShader->Release();

	ID3DBlob* blob = nullptr;
	
	std::wstring filePath = L"CompiledShaders/Particles_GS.cso";

    HRESULT hr = D3DReadFileToBlob(filePath.c_str(), &blob);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to load particle GS.");
		return false;
	}

	//Create shader
	hr = this->graphic.getDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->geometryShader);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create particle GS.");
		return false;
	}

	blob->Release();

    return true;
}

bool Particles::initBuffers()
{
	/*D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = sizeof(particleBuffer);
	bufferDesc.ByteWidth = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = initialData;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	HRESULT hr = this->graphic.getDevice()->CreateBuffer(&bufferDesc,
		initialData != NULL ? &subData : NULL,
		&this->buffer
	);*/
	
	
	return false;
}

Particles::Particles(Graphics& graphic)
    :graphic(graphic), geometryShader(nullptr), vertexShader(graphic), pixelShader(graphic), vertexBuffer(nullptr), bufferSize(0)
{
}

Particles::~Particles()
{
	if (this->geometryShader != nullptr)
		this->geometryShader->Release();

	if (this->vertexBuffer != nullptr)
		this->vertexBuffer->Release();
}

bool Particles::init()
{
	this->initShaders();
	
	return true;
}

bool Particles::render()
{
	this->graphic.getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Bind
	this->graphic.getDeviceContext()->IASetInputLayout(this->vertexShader.getInputLayout());


	
	return false;
}
