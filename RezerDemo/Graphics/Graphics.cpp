#include "Graphics.h"
#include <iostream>
#include <wrl/client.h>
#include <d3dcompiler.h> //Compile and load shaders
#include "../Application/ErrorLogger.h"

#pragma comment(lib, "D3DCompiler.lib")

namespace dx = DirectX;

bool Graphics::resourceManagement()
{
	//Device, swapchain and device context.
	HRESULT hr;

	//Descriptor to swapchain
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.BufferDesc.Width = this->window->getWidth();
	swapChainDesc.BufferDesc.Height = this->window->getHeight();
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0; // TOP: Let use the refresh rate that the window already use.
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 0; //Bottom
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //8bits for each color
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; 
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //We don't care about the scaling.

	swapChainDesc.SampleDesc.Count = 1; //We don't want anti aliasing
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferUsage = DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1; //Front and back buffer
	swapChainDesc.OutputWindow = this->window->getWindowHandle();
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0; //We don't want any special flag.

	hr = D3D11CreateDeviceAndSwapChain(nullptr,			//Default adapter.
		D3D_DRIVER_TYPE_HARDWARE,						//Driver type.
		nullptr,										//Handle to a software rasterizer.
		D3D11_CREATE_DEVICE_DEBUG,						//D3D11_CREATE_DEVICE_DEBUG
		nullptr,										//Take what ever feature level we can get. (Could set this to directx 11)
		0,												//Number of elements in feature.
		D3D11_SDK_VERSION,								//Recommended
		&swapChainDesc,									//Swapchain descriptor
		&this->swapchain,								//Swapchain
		&this->device,									//Device
		nullptr,										//Output pointer to the feature level.
		&this->immediateContext
	);

	return ErrorLogger::hrError(hr, "Error creating device and swapchain.");;
}

bool Graphics::loadShaders()
{
	//Geometry pass shaders
	InputLayoutDesc inputLayoutDesc;
	inputLayoutDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT);
	inputLayoutDesc.add("UV", DXGI_FORMAT_R32G32_FLOAT);

	this->deferred_VS.loadVS(L"Deferred_VS", inputLayoutDesc);
	this->deferred_PS.loadPS(L"Deferred_PS");

	//Lighting pass shaders
	
	//Compute shader
	ID3DBlob* blob = nullptr;

	//Read file
	HRESULT hr = D3DReadFileToBlob(L"CompiledShaders/Deferred_CS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read compute shader.");
		return false;
	}

	//Create compute shader
	hr = this->device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->deffered_CS);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create compute shader.");
		return false;
	}

	blob->Release();

	//Set thread groups
	this->threadX = (UINT)this->window->getWidth() / 32;
	this->threadY = (UINT)this->window->getHeight() / 32;
	this->threadZ = 1;

	return true;
}

bool Graphics::createDevices()
{
	HRESULT hr;

	//swapchain front/back buffer
	//We use the backbuffer as a Texture for Render target view.
	hr = this->swapchain->GetBuffer(0, //use 0 if we have discord on swepchain
		__uuidof(ID3D11Resource), //What type of interface.
		reinterpret_cast<void**>(&this->backBuffer) //Pointer to backbuffer, we convert this to double pointer.
	);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Error getting back buffer.");
		return false;
	}

	//hr = this->device->CreateRenderTargetView(this->backBuffer, nullptr, &this->rtv);
	hr = this->device->CreateUnorderedAccessView(this->backBuffer, nullptr, &this->uavBackBuffer);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Error creating UAV backbuffer.");
		return false;
	}

	//Create stencil texture
	D3D11_TEXTURE2D_DESC dsTextureDesc = {};
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
	dsTextureDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&dsTextureDesc, nullptr, &this->dsTexture);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create depth stencil texture.");
		return false;
	}

	//Depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = this->device->CreateDepthStencilView(this->dsTexture, &dsvDesc, &this->dsv);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create depth stencil view");
		return false;
	}

	//Depth stencil state
	D3D11_DEPTH_STENCIL_DESC dStateDesc = {};
	dStateDesc.DepthEnable = true;
	dStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	hr = this->device->CreateDepthStencilState(&dStateDesc, &this->dsState);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create Depth Stencil State.");
		return false;
	}

	this->immediateContext->OMSetDepthStencilState(this->dsState, 0);

	return true;
}

void Graphics::shadowMap()
{
	this->light.renderShadowMap(this->meshes);
}

void Graphics::geometryPass()
{
	//Set renderer targets
	this->immediateContext->OMSetRenderTargets(BUFFER_COUNT, this->rtvArray, this->dsv);

	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		this->renderMesh(*this->meshes[i]);
	}	
}

Graphics::Graphics()
	:device(nullptr), immediateContext(nullptr), swapchain(nullptr), viewPort(), backBuffer(nullptr),
	deferred_VS(*this), deferred_PS(*this), window(), camera(*this), light(*this),
	mvpConstantBuffer(*this, "MVP CB"), materialCB(*this, "Matieral CB"), cameraPos(*this, "Camera pos CB"),
	threadX(0), threadY(0), threadZ(0), ambientTexture(*this), specularTexture(*this)
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->srvArray[i] = nullptr;
		this->rtvArray[i] = nullptr;
		this->rtvTextureArray[i] = nullptr;
		this->nullRTVarray[i] = nullptr;
	}
}

Graphics::~Graphics()
{
	this->backBuffer->Release();
	this->uavBackBuffer->Release();
	this->deffered_CS->Release();

	this->dsState->Release();
	this->dsTexture->Release();
	this->dsv->Release();

	this->immediateContext->Release();
	this->swapchain->Release();
	this->device->Release();

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->srvArray[i]->Release();
		this->rtvArray[i]->Release();
		this->rtvTextureArray[i]->Release();
	}

	for (int i = 0; i < this->meshes.size(); i++)
	{
		delete this->meshes[i];
	}
}

void Graphics::render()
{
	//Clear screen.
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->immediateContext->ClearRenderTargetView(this->rtvArray[i], clearColor);
	}
	this->immediateContext->ClearDepthStencilView(this->dsv, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Update camera
	DirectX::SimpleMath::Matrix vp = this->camera.getViewMatrix() * this->camera.getProjectionMatrix();
	this->mvpBufferStruct.vpMatrix = vp.Transpose();

	//Shadow map
	this->shadowMap();

	//Set viewport
	this->immediateContext->RSSetViewports(1, &viewPort);
	
	//Geomentry Pass
	this->geometryPass();

	//Light pass
	this->lightPass();

	//Unbind rtv
	this->immediateContext->OMSetRenderTargets(1, this->nullRTVarray, nullptr);

}

bool Graphics::initMeshes()
{
	Mesh* planeMesh = new Mesh(*this);
	planeMesh->createDefualtMesh(DefaultMesh::PLANE);
	planeMesh->createTexture("ground.jpg");
	planeMesh->setPosition(0.0f, -2.0f, 0.0f);
	planeMesh->setScaling(20.0f, 1.0f, 20.0f);
	this->meshes.push_back(planeMesh);

	Mesh* sphereMesh = new Mesh(*this);
	sphereMesh->createDefualtMesh(DefaultMesh::SPHERE);
	sphereMesh->createTexture("texture3d.jpg");
	sphereMesh->setPosition(0.0f, 0.0f, 0.0f);
	this->meshes.push_back(sphereMesh);

	Mesh* cubeMesh = new Mesh(*this);
	cubeMesh->createDefualtMesh(DefaultMesh::CUBE);
	cubeMesh->createTexture("texture3d.jpg");
	cubeMesh->setPosition(3.0f, 0.0f, 0.0f);
	this->meshes.push_back(cubeMesh);

	Mesh* cubeMesh2 = new Mesh(*this);
	cubeMesh2->createDefualtMesh(DefaultMesh::CUBE);
	cubeMesh2->createTexture("texture3d.jpg");
	cubeMesh2->setPosition(-3.0f, 0.0f, 0.0f);
	this->meshes.push_back(cubeMesh2);

	Mesh* cubeMesh3 = new Mesh(*this);
	cubeMesh3->createDefualtMesh(DefaultMesh::CUBE);
	cubeMesh3->createTexture("texture3d.jpg");
	cubeMesh3->setPosition(0.0f, 0.0f, -3.0f);
	this->meshes.push_back(cubeMesh3);

	Mesh* cubeMesh4 = new Mesh(*this);
	cubeMesh4->createDefualtMesh(DefaultMesh::CUBE);
	cubeMesh4->createTexture("texture3d.jpg");
	cubeMesh4->setPosition(0.0f, 0.0f, 3.0f);
	this->meshes.push_back(cubeMesh4);

	
	
	return true;
}

bool Graphics::defferdInit()
{
	HRESULT hr;

	//Renderer target texture description
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = this->window->getWidth();
	textureDesc.Height = this->window->getHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;

	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MipLevels = 0;

	//Create renderer target texture
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = this->device->CreateTexture2D(&textureDesc, nullptr, &this->rtvTextureArray[i]);

		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to create renderer target texture: " + i);
			return false;
		}
	}

	//Renderer target view description
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	//Create renderer target view
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = this->device->CreateRenderTargetView(this->rtvTextureArray[i], &rtvDesc, &this->rtvArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to create renderer target view: " + i);
			return false;
		}
	}

	//Shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	//Create 32bit shader resoruce view
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->device->CreateShaderResourceView(this->rtvTextureArray[i], &srvDesc, &this->srvArray[i]);
		if (FAILED(hr))
		{
			ErrorLogger::errorMessage("Failed to create shader resource view: " + i);
			return false;
		}

	}

	return true;
}

void Graphics::renderMesh(Mesh& mesh)
{
	//Update mesh first
	mesh.update();

	//Set input layout and vertex shader
	this->immediateContext->IASetInputLayout(this->deferred_VS.getInputLayout());
	this->immediateContext->VSSetShader(this->deferred_VS.getVS(), nullptr, 0);

	//Set pixel shader
	this->immediateContext->PSSetShader(this->deferred_PS.getPS(), nullptr, 0);

	//Set mvp matrix
	DirectX::SimpleMath::Matrix m = mesh.getWorldMatrix();
	this->mvpBufferStruct.worldMatrix = m.Transpose();
	this->mvpConstantBuffer.updateBuffer(&this->mvpBufferStruct);

	//Set vertex/index buffer
	this->immediateContext->IASetVertexBuffers(
		0, 1, &mesh.getVertexBuffer().getBuffer(), &mesh.getVertexBuffer().getStride(), &mesh.getVertexBuffer().getOffset()
	);

	this->immediateContext->IASetIndexBuffer(
		mesh.getIndexBuffer().getBuffer(), DXGI_FORMAT_R32_UINT, 0
	);

	//Set vertex shader CB
	this->immediateContext->VSSetConstantBuffers(0, 1, &this->mvpConstantBuffer.getBuffer());

	//Set Sampler and textures
	this->immediateContext->PSSetSamplers(0, 1, &mesh.getTexture().getSamplerState());
	this->immediateContext->PSSetShaderResources(0, 1, &mesh.getTexture().getSRV());
	this->immediateContext->PSSetShaderResources(1, 1, &this->ambientTexture.getSRV());
	this->immediateContext->PSSetShaderResources(2, 1, &this->specularTexture.getSRV());

	//Draw
	this->immediateContext->DrawIndexed(
		mesh.getIndices().size(), 0, 0
	);

	//rebind sampler
	ID3D11SamplerState* nullSampler = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	this->immediateContext->PSSetSamplers(0, 1, &nullSampler);
	this->immediateContext->PSSetShaderResources(0, 1, &nullSRV);
}

void Graphics::lightPass()
{
	//Clear renderer target view
	this->immediateContext->OMSetRenderTargets(BUFFER_COUNT, this->nullRTVarray, nullptr);

	//Set compute shader
	this->immediateContext->CSSetShader(this->deffered_CS, nullptr, 0);

	//Set UAV
	this->immediateContext->CSSetUnorderedAccessViews(0, 1, &this->uavBackBuffer, nullptr);

	//Bind structure buffer
	this->light.update(this->camera);
	this->immediateContext->CSSetShaderResources(0, 1, &this->light.getStructureSRV());

	//Set SRV
	this->immediateContext->CSSetShaderResources(1, (UINT)BUFFER_COUNT, this->srvArray);

	//Set constant buffers
	this->immediateContext->CSSetConstantBuffers(0, 1, &this->light.getShadowMapMVPConstnantBuffer().getBuffer());

	//Material
	this->materialCB.createBuffer(sizeof(materialBufferStruct), sizeof(MaterialStruct), &materialBufferStruct);
	this->immediateContext->CSSetConstantBuffers(1, 1, &this->materialCB.getBuffer());

	//Camera
	this->camera.update();
	this->immediateContext->CSSetConstantBuffers(2, 1, &this->camera.getConstantBuffer().getBuffer());

	//Shadow map


	//Dispatch
	this->immediateContext->Dispatch(this->threadX, this->threadY, this->threadZ);

	//Reset
	this->immediateContext->CSSetShader(nullptr, nullptr, 0);
	this->immediateContext->CSSetUnorderedAccessViews(0, 1, &this->nullUAV, nullptr);
}

bool Graphics::init(Window& window)
{
	this->window = &window;

	this->resourceManagement();
	this->initMeshes();

	this->createDevices();

	this->defferdInit();

	this->loadShaders();

	//Textures
	this->specularTexture.loadTexture("specular.png");
	this->ambientTexture.loadTexture("ambient.png");

	//view port
	this->viewPort.TopLeftX = 0.0f;
	this->viewPort.TopLeftY = 0.0f;
	this->viewPort.Width = static_cast<float>(this->window->getWidth());
	this->viewPort.Height = static_cast<float>(this->window->getHeight());
	this->viewPort.MinDepth = 0;
	this->viewPort.MaxDepth = 1;

	//Constantbuffers
	this->mvpConstantBuffer.createBuffer(sizeof(mvpBufferStruct), sizeof(mvpBuffer), &mvpBufferStruct);

	//Light
	this->light.init();

	//Light material
	this->materialBufferStruct.ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	this->materialBufferStruct.specular = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	this->materialBufferStruct.specularPower = 32.0f;

	//Camera
	this->camera.init();

	//Light Structure buffer

	//Wont change during runtime
	this->immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;

}

void Graphics::presentSC()
{
	//Presenting frames
	this->swapchain->Present(3, 0); //First one describes how many frame rates / sec you can do.
}
