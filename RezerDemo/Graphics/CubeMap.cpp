#include "CubeMap.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"

#include <d3dcompiler.h> //Compile and load shaders

#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX::SimpleMath;

void CubeMap::initVP()
{
	this->viewMatrix[0] = Matrix::CreateLookAt(this->position, Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// +X
	this->viewMatrix[1] = Matrix::CreateLookAt(this->position, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// -X
	this->viewMatrix[2] = Matrix::CreateLookAt(this->position, Vector3(0.0f, -1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));		// +Y
	this->viewMatrix[3] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f));		// -Y
	this->viewMatrix[4] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));		// +Z
	this->viewMatrix[5] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));		// -Z
}

bool CubeMap::initShaders()
{
	//Geometry pass shaders
	InputLayoutDesc inputLayoutDesc;
	inputLayoutDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("UV", DXGI_FORMAT_R32G32B32_FLOAT);

	this->vertexShader.loadVS(L"Cubemap_VS", inputLayoutDesc);
	this->pixelShader.loadPS(L"Cubemap_PS");

	//Compute shader.
	//Load compute shader
	ID3DBlob* blob = nullptr;
	HRESULT hr = D3DReadFileToBlob(L"CompiledShaders/Cubemap_CS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read CS shader.");
		return false;
	}

	//Create geometry shader
	hr = this->graphic.getDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->computeShader);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create CS.");
		return false;
	}

	return true;
}

bool CubeMap::initViews()
{
	D3D11_TEXTURE2D_DESC dsvTextureDesc = {};
	dsvTextureDesc.Width = this->textureSize;
	dsvTextureDesc.Height = this->textureSize;
	dsvTextureDesc.MipLevels = 1;
	dsvTextureDesc.ArraySize = 1;
	dsvTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	dsvTextureDesc.SampleDesc.Count = 1;
	dsvTextureDesc.SampleDesc.Quality = 0;

	dsvTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	dsvTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsvTextureDesc.CPUAccessFlags = 0;
	dsvTextureDesc.MiscFlags = 0;

	HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&dsvTextureDesc, nullptr, &this->dsvTexture);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cubemap depth stencil texture.");
		return false;
	}

	//Depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = this->graphic.getDevice()->CreateDepthStencilView(this->dsvTexture, &dsvDesc, &this->dsv);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cubemap depth stencil view");
		return false;
	}

	return true;
}

CubeMap::CubeMap(Graphics& graphic)
	:graphic(graphic),texture(nullptr), uav(nullptr), textureSize(512), position(0.0f, 0.0f, 0.0f), pixelShader(graphic), vertexShader(graphic),
	srv(nullptr), vp(), dsv(nullptr), dsvTexture(nullptr), computeShader(nullptr)
{
	for (int i = 0; i < VIEW_SIZE; i++)
	{
		this->rtvArray[i] = nullptr;
	}
}

CubeMap::~CubeMap()
{
	
	if (this->texture != nullptr)
		this->texture->Release();

	if (this->srv != nullptr)
		this->srv->Release();

	if (this->uav != nullptr)
		this->uav->Release();

	for (int i = 0; i < VIEW_SIZE; i++)
	{
		this->rtvArray[i]->Release();
	}

	if (this->dsvTexture != nullptr)
		this->dsvTexture->Release();

	if (this->dsv != nullptr)
		this->dsv->Release();

	if (this->computeShader != nullptr)
		this->computeShader->Release();
}

bool CubeMap::init()
{
	this->initVP();
	this->initShaders();

	//Create texture cubemap
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = this->textureSize;
	textureDesc.Height = this->textureSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.CPUAccessFlags = 0;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&textureDesc, nullptr, &this->texture);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cube map texture.");
		return false;
	}
	//Create uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = 6;
	uavDesc.Texture2DArray.FirstArraySlice = 0;

	hr = this->graphic.getDevice()->CreateUnorderedAccessView(this->texture, &uavDesc, &this->uav);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cubemap UAV.");
		return false;
	}

	//SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = this->graphic.getDevice()->CreateShaderResourceView(this->texture, &srvDesc, &this->srv);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cubemap SRV.");
		return false;
	}

	//RTV

	for (int i = 0; i < VIEW_SIZE; i++)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		rtvDesc.Texture2DArray.ArraySize = 1;
		rtvDesc.Texture2DArray.FirstArraySlice = (UINT)i;

		this->graphic.getDevice()->CreateRenderTargetView(this->texture, &rtvDesc, &this->rtvArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to create cubemap RTV.");
			return false;
		}
	}


	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;
	this->vp.Width = (float)this->textureSize;
	this->vp.Height = (float)this->textureSize;
	this->vp.MinDepth = 0;
	this->vp.MaxDepth = 1;

	return true;
}


