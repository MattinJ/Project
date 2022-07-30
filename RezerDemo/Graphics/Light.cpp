#include "Light.h"
#include "Graphics.h"
#include "../Application/Input.h"
#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"

using namespace DirectX;

bool Light::createLight(LightType type, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 dir)
{
	LightStruct light = {};
	light.position = pos;
	dir.Normalize();
	light.direction = dir;
	light.color = DirectX::SimpleMath::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	light.lightType = 1;

	switch (type)
	{
	case LightType::DIRECTIONAL:
	{
		light.lightType = 0;
		break;
	}
	case LightType::SPOT:
	{
		light.range = 10.0f;
		light.cone = 20.0f;
		light.att = DirectX::SimpleMath::Vector3(0.4f, 0.02f, 0.01f);
		light.lightType = 1;
		break;
	}
	default:
		break;
	}

	this->lights.push_back(light);

	return true;
}

bool Light::initLights()
{
	//this->createLight(LightType::DIRECTIONAL, DirectX::SimpleMath::Vector3(1.0f, 1.0f, 0.0f), DirectX::SimpleMath::Vector3(0.0f, -1.0f, 0.0f));
	this->createLight(LightType::SPOT, DirectX::SimpleMath::Vector3(0.0f, 4.0f, 0.0f), DirectX::SimpleMath::Vector3(0.5f, -1.0f, 0.0f));
	
	return true;
}

bool Light::initBuffer()
{
	//Buffer
	this->bufferSize = sizeof(LightStruct) * this->lights.size();

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = this->bufferSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(LightStruct);

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = lights.data();
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	HRESULT hr = this->graphic.getDevice()->CreateBuffer(&bufferDesc, &subData, &this->strucutreBuffer);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create structure buffer!");
		return false;
	}

	//SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = this->lights.size();

	hr = this->graphic.getDevice()->CreateShaderResourceView(this->strucutreBuffer, &srvDesc, &this->structreSRV);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create structure SRV!");
		return false;
	}
	
	return true;
}

Light::Light(Graphics& graphic, LightType type)
	:graphic(graphic), shadowMapMVPBuffer(graphic, "Shadow map CB"), bufferSize(0),
	shadowMapDepthTexture(nullptr), shadowMapDSV(nullptr), shadowMapSize(1024), shadowMapSRV(nullptr), shadowMap_VS(graphic)

{
}

Light::~Light()
{
	if(this->shadowMapDepthTexture != nullptr)
		this->shadowMapDepthTexture->Release();

	if (this->shadowMapDSV != nullptr)
		this->shadowMapDSV->Release();

	if (this->shadowMapSRV != nullptr)
		this->shadowMapSRV->Release();

	if (this->strucutreBuffer != nullptr)
		this->strucutreBuffer->Release();

	if (this->structreSRV != nullptr)
		this->structreSRV->Release();

	if (this->shadowMapSampler != nullptr)
		this->shadowMapSampler->Release();

}

bool Light::init()
{
	this->initLights();
	this->initBuffer();
	
	//Depth texture description
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = this->shadowMapSize;
	textureDesc.Height = this->shadowMapSize;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MipLevels = 0;

	//Create depth stencil texture
	HRESULT hr = this->graphic.getDevice()->CreateTexture2D(&textureDesc, nullptr, &this->shadowMapDepthTexture);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Faileded to create shadow map depth stencil texture");
		return false;
	}

	//Create shader resource view desc
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//Create shader resource view
	hr = this->graphic.getDevice()->CreateShaderResourceView(this->shadowMapDepthTexture, &srvDesc, &this->shadowMapSRV);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Faileded to create shadow map SRV");
		return false;
	}

	//Create depth stencil view desciptor
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	//Create depth stencil view
	hr = this->graphic.getDevice()->CreateDepthStencilView(this->shadowMapDepthTexture, &dsvDesc, &this->shadowMapDSV);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Faileded to create shadow map DSV");
		return false;
	}

	//shadow map vertex shader
	InputLayoutDesc shadowInputLayoutDesc;
	shadowInputLayoutDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	shadowInputLayoutDesc.add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	shadowInputLayoutDesc.add("UV", DXGI_FORMAT_R32G32_FLOAT);

	this->shadowMap_VS.loadVS(L"ShadowMap_VS", shadowInputLayoutDesc);

	//Set view port
	this->shadowMapVP = {};
	this->shadowMapVP.TopLeftX = 0;
	this->shadowMapVP.TopLeftY = 0;
	this->shadowMapVP.Width = (FLOAT)this->shadowMapSize;
	this->shadowMapVP.Height = (FLOAT)this->shadowMapSize;
	this->shadowMapVP.MinDepth = 0;
	this->shadowMapVP.MaxDepth = 1;

	this->projectionMatrix = DirectX::SimpleMath::Matrix::CreateOrthographic(
		100, 100, 0.1f, 200.0f
	);

	//Sampler
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	// Create sampler
	hr = this->graphic.getDevice()->CreateSamplerState(&samplerDesc, &this->shadowMapSampler);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Could not create depth map sampler.");

		return false;
	}

	//Create Constantbuffers
	this->shadowMapMVPBuffer.createBuffer(sizeof(lightBufferMVP), sizeof(LightBufferVP), &lightBufferMVP);

	return true;
}

void Light::renderShadowMap(std::vector<Mesh*>& meshes)
{
	for (size_t i = 0; i < this->lights.size(); i++)
	{
		//Update view matrix
		DirectX::XMFLOAT3 test;
		test.x = this->lights[i].position.x + this->lights[i].direction.x;
		test.y = this->lights[i].position.y + this->lights[i].direction.y;
		test.z = this->lights[i].position.z + this->lights[i].direction.z;

		this->viewMatrix = DirectX::SimpleMath::Matrix::CreateLookAt(this->lights[i].position, test, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f));

		//Make vpMatrix
		DirectX::SimpleMath::Matrix vpMatrix = this->viewMatrix * this->projectionMatrix;

		//Clear depth texture
		this->graphic.getDeviceContext()->ClearDepthStencilView(this->shadowMapDSV, D3D11_CLEAR_DEPTH, 1, 0);

		ID3D11RenderTargetView* nullRTV = nullptr;
		this->graphic.getDeviceContext()->OMSetRenderTargets(1, &nullRTV, this->shadowMapDSV);

		//Set view port
		this->graphic.getDeviceContext()->RSSetViewports(1, &this->shadowMapVP);

		//Set Input and vertex shader
		this->graphic.getDeviceContext()->IASetInputLayout(this->shadowMap_VS.getInputLayout());
		this->graphic.getDeviceContext()->VSSetShader(this->shadowMap_VS.getVS(), nullptr, 0);

		//Set pixel shader as null
		this->graphic.getDeviceContext()->PSSetShader(nullptr, nullptr, 0);

		//Set renderer target as null and add shadow DSV

		//Set mvp buffer
		this->graphic.getDeviceContext()->VSSetConstantBuffers(0, 1, &this->shadowMapMVPBuffer.getBuffer());

		//Set null to sampler and texture
		//ID3D11SamplerState* nullSampler[]{ nullptr };
		ID3D11ShaderResourceView* nullSRV[]{ nullptr };

		//this->graphic.getDeviceContext()->PSSetSamplers(0, 1, nullSampler);
		this->graphic.getDeviceContext()->PSSetShaderResources(0, 1, nullSRV);

		//Go over all meshes

		for (size_t i = 0; i < meshes.size(); i++)
		{
			DirectX::SimpleMath::Matrix m = meshes[i]->getWorldMatrix();
			this->lightBufferMVP.worldMatrix = m.Transpose();
			this->lightBufferMVP.vpMatrix = vpMatrix.Transpose();

			this->shadowMapMVPBuffer.updateBuffer(&lightBufferMVP);

			//Set vertex buffer
			this->graphic.getDeviceContext()->IASetVertexBuffers(0, 1,
				&meshes[i]->getVertexBuffer().getBuffer(),
				&meshes[i]->getVertexBuffer().getStride(),
				&meshes[i]->getVertexBuffer().getOffset()
			);

			//Set index buffer
			this->graphic.getDeviceContext()->IASetIndexBuffer(
				meshes[i]->getIndexBuffer().getBuffer(),
				DXGI_FORMAT_R32_UINT, 0
			);

			//Draw
			this->graphic.getDeviceContext()->DrawIndexed(meshes[i]->getIndices().size(), 0, 0);
		}
	}
	
	
}

bool Light::update(Camera& camera)
{
	/*this->lightBufferMVP.vpMatrix = this->viewMatrix * this->projectionMatrix;
	this->shadowMapMVPBuffer.updateBuffer(&lightBufferMVP);*/

	/*for (size_t i = 0; i < this->lights.size(); i++)
	{
		if (this->lights[i].lightType == 1)
		{
			this->lights[0].position = camera.getPostion();
			this->lights[0].direction = camera.getTarget() - this->lights[0].position;
		}
	}*/
	
	//Map
	/*D3D11_MAPPED_SUBRESOURCE mappedSubResoruce;
	HRESULT hr = this->graphic.getDeviceContext()->Map(this->strucutreBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubResoruce);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to MAP structure buffer!");
		return false;
	}

	memcpy(mappedSubResoruce.pData, this->lights.data(), this->bufferSize);

	Unmap
	this->graphic.getDeviceContext()->Unmap(this->strucutreBuffer, 0);*/

	return true;

}

