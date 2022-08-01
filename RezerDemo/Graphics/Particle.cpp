#include "Particle.h"
#include "Graphics.h"

#include "../Application/ErrorLogger.h"
#include "../Application/Settings.h"

void Particle::updateWorldMatrix()
{
	this->worldMatrix =
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(this->rotation.x, this->rotation.y, this->rotation.z) *
		DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
}

bool Particle::initBuffers()
{
	Vertex dot = { 0.0f, 0.0f, 0.0f };

	this->stride = sizeof(Vertex);
	this->offSet = 0;

	//Vertex buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = this->stride;

	D3D11_SUBRESOURCE_DATA subData = {};
	subData.pSysMem = &dot;
	subData.SysMemPitch = 0;
	subData.SysMemSlicePitch = 0;

	HRESULT hr = this->graphic.getDevice()->CreateBuffer(&bufferDesc, &subData, &this->vertexBuffer);

	if (FAILED(hr))
	{
		ErrorLogger::errorMessage("Failed to create particle vertex buffer");
		return false;
	}

	return true;
}

void Particle::initParticle()
{
	this->updateWorldMatrix();

	this->particleVertex.x = this->position.x;
	this->particleVertex.y = this->position.y;
	this->particleVertex.z = this->position.z;
}

Particle::Particle(Graphics& graphic)
	:graphic(graphic), vertexBuffer(nullptr), position(0.0f, 0.0f, 0.0f), stride(0), offSet(0), uav(nullptr)
{
}

Particle::~Particle()
{
	if (this->vertexBuffer != nullptr)
		this->vertexBuffer->Release();

	if (this->uav != nullptr)
		this->uav->Release();
}

void Particle::init()
{
	this->initBuffers();
	this->initParticle();
}

void Particle::setPosition(DirectX::SimpleMath::Vector3 pos)
{
	this->position = pos;
}

void Particle::setRot(DirectX::SimpleMath::Vector3 rot)
{
	this->rotation = rot;
}
