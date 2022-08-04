#include "Tessellering.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"

#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib")

bool Tessellering::loadShaders()
{
	InputLayoutDesc inputLayoutDesc;
	inputLayoutDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("UV", DXGI_FORMAT_R32G32_FLOAT);

	this->vertexShader.loadVS(L"Lod_VS", inputLayoutDesc);
	
	//Load Hull shader
	ID3DBlob* blob = nullptr;
	HRESULT hr = D3DReadFileToBlob(L"CompiledShaders/Lod_HS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read Hull shader.");
		return false;
	}

	//Create Hull shader
	hr = this->graphic.getDevice()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->hullShader);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create hull shader.");
		return false;
	}

	//Load Domain shader
	hr = D3DReadFileToBlob(L"CompiledShaders/Lod_DS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read Domain shader.");
		return false;
	}

	//Create Domain shader
	hr = this->graphic.getDevice()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->domainShader);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create domain shader.");
		return false;
	}

	blob->Release();

	return true;
}

Tessellering::Tessellering(Graphics& graphic)
	:graphic(graphic), vertexShader(graphic), hullShader(nullptr), domainShader(nullptr), rasterState(nullptr)
{
}

Tessellering::~Tessellering()
{
	if (this->hullShader != nullptr)
		this->hullShader->Release();

	if (this->domainShader != nullptr)
		this->domainShader->Release();

	if (this->rasterState != nullptr)
		this->rasterState->Release();

}

bool Tessellering::init()
{
	this->loadShaders();

	//Rasterizer state
	D3D11_RASTERIZER_DESC rDesc = {};
	rDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rDesc.FrontCounterClockwise = false;
	rDesc.DepthBias = 0;
	rDesc.DepthBiasClamp = 0.0f;
	rDesc.SlopeScaledDepthBias = 0.0f;
	rDesc.DepthClipEnable = true;
	rDesc.ScissorEnable = false;
	rDesc.MultisampleEnable = false;
	rDesc.AntialiasedLineEnable = false;

	HRESULT hr = this->graphic.getDevice()->CreateRasterizerState(&rDesc, &this->rasterState);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Error creating rasterizer state.");
		return false;
	}

	return true;
}
