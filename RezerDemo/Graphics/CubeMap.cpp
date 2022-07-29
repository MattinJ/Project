#include "CubeMap.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"

using namespace DirectX::SimpleMath;

CubeMap::CubeMap(Graphics& graphic)
	:graphic(graphic), position(0.0f, 0.0f, 0.0f), target(0.0f, 0.0f, 0.0f), upDir(0.0f, 0.0f, 0.0f),
	dsv(nullptr), dss(nullptr), dsTexture(nullptr)
{
	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		this->textureArray[i] = nullptr;
		this->rtvArray[i] = nullptr;
		this->srvArray[i] = nullptr;
	}
}

CubeMap::~CubeMap()
{
	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		this->textureArray[i]->Release();
		this->rtvArray[i]->Release();
		this->srvArray[i]->Release();
	}

	if (this->dsv != nullptr)
		this->dsv->Release();

	if (this->dss != nullptr)
		this->dss->Release();

	if (this->dsTexture != nullptr)
		this->dsTexture->Release();
}

bool CubeMap::init()
{
	//Set view matrix
	this->viewMatrix = Matrix::CreateLookAt(this->position, this->target, this->upDir);

	//Set projection matrix
	//this->projectionMatrix = Matrix::CreatePerspectiveFieldOfView(90.0f, )

	//Might need a sampler

	//Create texture
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (UINT)TEXTURE_SIZE;
	textureDesc.Height = (UINT)TEXTURE_SIZE;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.CPUAccessFlags = 0;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&textureDesc, nullptr, &this->textureArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to load cube map texture.");
			return false;
		}
	}

	//Create RTV
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2D.MipSlice = 1;

	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		HRESULT hr = this->graphic.getDevice()->CreateRenderTargetView(this->textureArray[i], nullptr, &this->rtvArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to load cube map rtv.");
			return false;
		}
	}

	//Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	for (size_t i = 0; i < BUFFER_SIZE; i++)
	{
		HRESULT hr = this->graphic.getDevice()->CreateShaderResourceView(this->textureArray[i], nullptr, &this->srvArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to load cube map srv.");
			return false;
		}
	}

	//DSV texture
	/*D3D11_TEXTURE2D_DESC dsTextureDesc = {};
	dsTextureDesc.Width = this->window->getWidth();
	dsTextureDesc.Height = this->window->getHeight();
	dsTextureDesc.MipLevels = 1;
	dsTextureDesc.ArraySize = 1;
	dsTextureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	dsTextureDesc.SampleDesc.Count = 1;
	dsTextureDesc.SampleDesc.Quality = 0;

	dsTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	dsTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsTextureDesc.CPUAccessFlags = 0;
	dsTextureDesc.MiscFlags = 0;*/

	return true;
}

void CubeMap::render()
{

}

void CubeMap::setPosition(DirectX::SimpleMath::Vector3 pos)
{
	this->position = pos;
}

void CubeMap::setTarget(DirectX::SimpleMath::Vector3 target)
{
	this->target = target;
}

void CubeMap::setDir(DirectX::SimpleMath::Vector3 dir)
{
	this->target = dir;
}
