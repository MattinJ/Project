#include "Texture.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../External/stbi/stb_image.h"

bool Texture::createSampler()
{
	if (this->samplerState != nullptr)
		this->samplerState->Release();
	
	//texture sampler
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;				//Use anisotropic interpolation
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;			//Wrap x
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;			//Wrap y
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;			//Wrap z
	samplerDesc.MipLODBias = 0;									//Offset of mipmap level
	samplerDesc.MaxAnisotropy = 1;								//Max clap value for Anisotropic
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;		//No comparison
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;										//Lower end of mipmap clamp. 0 is larges and most detailed mipmap
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;						//no upper limit on LOD set
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	//Create sample state
	HRESULT hr = this->graphic.getDevice()->CreateSamplerState(&samplerDesc, &this->samplerState);

	if (FAILED(hr))
	{
		ErrorLogger::hrError(hr, "Error creating sample state.");
		return false;
	}
	
	this->graphic.getDeviceContext()->PSSetSamplers(0, 1, &this->samplerState);

	return true;
}

Texture::Texture(Graphics& graphic)
	:graphic(graphic), texture(nullptr), textureDesc{}, srv(nullptr), samplerState(nullptr),
	textureWidth(0), textureHeight(0), textureChannels(0)
{
}

Texture::~Texture()
{
	if (this->texture != nullptr)
		this->texture->Release();

	if (this->srv != nullptr)
		this->srv->Release();

	if (this->samplerState != nullptr)
		this->samplerState->Release();
}

bool Texture::loadTexture(std::string fileName)
{
	if (this->texture != nullptr)
		this->texture->Release();

	if (this->srv != nullptr)
		this->srv->Release();
	
	
	//Load texture from file
	std::string filePath = "Resources/Textures/" + fileName;
	
	unsigned char* dataTexture = stbi_load(filePath.c_str(), &this->textureWidth, &this->textureHeight, &this->textureChannels, 4);
	if (dataTexture == nullptr)
	{
		ErrorLogger::errorMessage("Error loading texture: " + fileName);
		return false;
	}

	this->textureDesc = {};
	this->textureDesc.Width = this->textureWidth;
	this->textureDesc.Height = this->textureHeight;
	this->textureDesc.MipLevels = 1;
	this->textureDesc.ArraySize = 1;
	this->textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	
	this->textureDesc.SampleDesc.Count = 1;
	this->textureDesc.SampleDesc.Quality = 0;

	this->textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	this->textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	this->textureDesc.CPUAccessFlags = 0;
	this->textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA textureSubData = {};
	textureSubData.pSysMem = dataTexture; //pointer to data
	textureSubData.SysMemPitch = this->textureWidth * 4; //Distance in byte from beginning of one line to the next
	textureSubData.SysMemSlicePitch = 0; //Distance in byte from begnning depth to next 

	//Create texture2D
	HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&this->textureDesc, &textureSubData, &this->texture);

	if (FAILED(hr))
	{
		ErrorLogger::hrError(hr, "Error creating texture 2D");
		return false;
	}

	//Create Shader resource view. NULL to create a view that accesses the entire resource.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = this->textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = this->graphic.getDevice()->CreateShaderResourceView(this->texture, &srvDesc, &this->srv);

	if (FAILED(hr))
	{
		ErrorLogger::hrError(hr, "Error creating texture SRV");
		return false;
	}

	//Bind sampler and shaderresource
	this->graphic.getDeviceContext()->PSSetShaderResources(0, 1, &this->srv);

	this->createSampler();

	stbi_image_free(dataTexture);

	return true;
}
