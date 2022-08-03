#include "ParticleSystem.h"
#include "Graphics.h"

#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"
#include "../Application/Time.h"

#include <d3dcompiler.h> //Compile and load shaders

#pragma comment(lib, "D3DCompiler.lib")

using namespace DirectX::SimpleMath;

bool ParticleSystem::initBuffers()
{
	this->stride = sizeof(Vertex);
	this->offSet = 0;

	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * this->nrOfPartciles;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = &this->particles[0];
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	HRESULT hr = this->graphic.getDevice()->CreateBuffer(&bufferDesc, &subData, &this->vertexBuffer);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create particle vertex buffer");
		return false;
	}

	//Particle uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = this->nrOfPartciles * 4;
	uavDesc.Buffer.Flags = 0;

	hr = this->graphic.getDevice()->CreateUnorderedAccessView(this->vertexBuffer, &uavDesc, &this->uav);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create particle uav");
		return false;
	}
	
	this->particlesystemCB.createBuffer(sizeof(particleSystemStruct), sizeof(ParticleSystemBuffer), &particleSystemStruct);
	this->particleCB.createBuffer(sizeof(particleStruct), sizeof(ParticleBuffer), &particleStruct);

	return true;
}

bool ParticleSystem::initShaders()
{
	//Load vertex and pixel shdaer
	InputLayoutDesc particleInputDesc;
	particleInputDesc.add("POSITION", DXGI_FORMAT_R32G32B32_FLOAT);

	this->particle_VS.loadVS(L"Particles_VS", particleInputDesc);
	this->particle_PS.loadPS(L"Particles_PS");

	//Load geometry shader
	ID3DBlob* blob = nullptr;
	HRESULT hr = D3DReadFileToBlob(L"CompiledShaders/Patricles_GS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read GS shader.");
		return false;
	}

	//Create geometry shader
	hr = this->graphic.getDevice()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->particle_GS);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create GS.");
		return false;
	}

	//Create pixel CS
	hr = D3DReadFileToBlob(L"CompiledShaders/Particles_CS.cso", &blob);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to read particle CS shader.");
		return false;
	}

	hr = this->graphic.getDevice()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->particle_CS);
	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create GS.");
		return false;
	}
	
	return true;
}

void ParticleSystem::initParticle()
{
	for (int i = 0; i < this->nrOfPartciles; i++)
	{
		float randomX = (float)(-1000 + (std::rand() % 2000)) * 0.01;
		float randomY = (float)(2000 + (std::rand() % 1000)) * 0.01;
		float randomZ = (float)(-1000 + (std::rand() % 2000)) * 0.01;

		float fallSpeed = (float)(1000 + (std::rand() % 2000)) * 0.0001;
		
		Vertex point = { randomX, randomY, randomZ, fallSpeed};
		particles.push_back(point);
	}
}

ParticleSystem::ParticleSystem(Graphics& graphic)
	:graphic(graphic), vertexBuffer(nullptr), startPosition(0.0f, 0.0f, 0.0f), stride(0), offSet(0), uav(nullptr), particlesystemCB(graphic, "ParticleSystem CB"),
	particle_VS(graphic), particle_PS(graphic), particleTexture(graphic), particleCB(graphic, "Particle CB")
{
}

ParticleSystem::~ParticleSystem()
{
	if (this->vertexBuffer != nullptr)
		this->vertexBuffer->Release();

	if (this->uav != nullptr)
		this->uav->Release();

	if (this->particle_CS != nullptr)
		this->particle_CS->Release();

	if (this->particle_GS != nullptr)
		this->particle_GS->Release();
}

void ParticleSystem::init()
{
	this->particleTexture.loadTexture("particle.png");
	
	this->initParticle();
	this->initBuffers();
	this->initShaders();
}

void ParticleSystem::render(Camera& camera)
{
	ID3D11DeviceContext* immediateContext = this->graphic.getDeviceContext();
	immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	//Update camera
	DirectX::SimpleMath::Matrix vp = camera.getViewMatrix() * camera.getProjectionMatrix();
	this->particleSystemStruct.vpMatrix = vp.Transpose();

	//---------------------- Compute shader --------------------------
	
	//Set particle CS
	immediateContext->CSSetShader(this->particle_CS, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &this->uav, nullptr);

	immediateContext->CSSetConstantBuffers(0, 1, &this->particleCB.getBuffer());

	//Run
	immediateContext->Dispatch(this->nrOfPartciles / 32, 1, 1);

	//Rebind
	immediateContext->CSSetShader(nullptr, nullptr, 0);
	immediateContext->CSSetUnorderedAccessViews(0, 1, &this->uavNull, nullptr);
	
	//---------------------- Pipeline -----------------------

	//Set input layout and vertex shader
	immediateContext->IASetInputLayout(this->particle_VS.getInputLayout());
	immediateContext->VSSetShader(this->particle_VS.getVS(), nullptr, 0);

	//Set geometryshader
	immediateContext->GSSetShader(this->particle_GS, nullptr, 0);

	//Set pixel shader
	immediateContext->PSSetShader(this->particle_PS.getPS(), nullptr, 0);

	//Set partcile buffer
	this->particleSystemStruct.cameraPos = camera.getPostion();
	this->particlesystemCB.updateBuffer(&this->particleSystemStruct);

	//Set vertex/index buffer
	immediateContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &this->stride, &this->offSet);

	//Set constant buffer to GS
	immediateContext->GSSetConstantBuffers(0, 1, &this->particlesystemCB.getBuffer());

	//Set Sampler and textures
	immediateContext->PSSetSamplers(0, 1, &this->particleTexture.getSamplerState());
	immediateContext->PSSetShaderResources(0, 1, &this->particleTexture.getSRV());

	//Draw
	immediateContext->Draw(this->nrOfPartciles, 0);

	//remove sampler srv and gemotryshader 
	ID3D11SamplerState* nullSampler = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	immediateContext->PSSetSamplers(0, 1, &nullSampler);
	immediateContext->PSSetShaderResources(0, 1, &nullSRV);
	immediateContext->GSSetShader(nullptr, nullptr, 0);
}

void ParticleSystem::update()
{
	this->particleStruct.deltaTime = Time::getDT();
	this->particleStruct.speed = this->speed;
	this->particleStruct.time = this->time;
	this->time += 0.1;
	this->particleCB.updateBuffer(&this->particleStruct);
}

void ParticleSystem::setStartPosition(DirectX::SimpleMath::Vector3 pos)
{
	this->startPosition = pos;
}

