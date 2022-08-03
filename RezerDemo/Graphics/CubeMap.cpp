#include "CubeMap.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"

using namespace DirectX::SimpleMath;

void CubeMap::initVPmatrix()
{
	this->viewMatrix[0] = Matrix::CreateLookAt(this->position, Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// +X
	this->viewMatrix[1] = Matrix::CreateLookAt(this->position, Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// -X
	this->viewMatrix[2] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// +Y
	this->viewMatrix[3] = Matrix::CreateLookAt(this->position, Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));		// -Y
	this->viewMatrix[4] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f));		// +Z
	this->viewMatrix[5] = Matrix::CreateLookAt(this->position, Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f));		// -Z
}

void CubeMap::initShaders()
{
	//Geometry pass shaders
	InputLayoutDesc inputLayoutDesc;
	inputLayoutDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("UV", DXGI_FORMAT_R32G32B32_FLOAT);

	this->vertexShader.loadVS(L"Cubemap_VS", inputLayoutDesc);
	this->pixelShader.loadPS(L"Cubemap_PS");
}

CubeMap::CubeMap(Graphics& graphic)
	:graphic(graphic),texture(nullptr), textureSize(512), position(0.0f, 0.0f, 0.0f), pixelShader(graphic), vertexShader(graphic), 
	mesh(graphic), srv(nullptr), vp()
{
	for (int i = 0; i < VIEW_SIZE; i++)
	{
		this->uav[i] = nullptr;
	}
}

CubeMap::~CubeMap()
{
	
	if (this->texture != nullptr)
		this->texture->Release();

	if (this->srv != nullptr)
		this->srv->Release();

	for (int i = 0; i < VIEW_SIZE; i++)
	{
		this->uav[i]->Release();
	}
	
}

bool CubeMap::init()
{
	this->initVPmatrix();
	this->initShaders();

	this->mesh.createDefualtMesh(DefaultMesh::CUBE);
	this->mesh.setPosition(0.0f, 0.0f, 0.0f);

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
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&textureDesc, nullptr, &this->texture);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create cube map texture.");
		return false;
	}
	
	for (int i = 0; i < VIEW_SIZE; i++)
	{
		//Create uav
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = textureDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		uavDesc.Texture2DArray.ArraySize = 1;
		uavDesc.Texture2DArray.FirstArraySlice = (UINT)i;

		hr = this->graphic.getDevice()->CreateUnorderedAccessView(this->texture, &uavDesc, &this->uav[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to create cubemap UAV.");
			return false;
		}

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

	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;
	this->vp.Width = this->textureSize;
	this->vp.Height = this->textureSize;
	this->vp.MinDepth = 0;
	this->vp.MaxDepth = 1;

	return true;
}


