#include "Mesh.h"

using namespace DirectX;

void Mesh::updateWorldMatrix()
{
	this->worldMatrix =
		DirectX::XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z) *
		DirectX::XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z) *
		DirectX::XMMatrixTranslation(this->pos.x, this->pos.y, this->pos.z);
}

Mesh::Mesh(Graphics& renderer, MeshData&& meshData)
	:vertexBuffer(renderer), indexBuffer(renderer), pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f),
	subMeshes(meshData.getSubMeshes()), worldMatrix(DirectX::SimpleMath::Matrix().Identity), startIndex(0), texture(renderer), 
	ambient(0.3f, 0.3f, 0.3f), specular(1.0f, 1.0f, 1.0f), specularPower(32.0f)
{
	this->updateWorldMatrix();
	this->vertexBuffer.createBuffer(meshData);
	this->indexBuffer.createBuffer(meshData);

	this->boundingSphere.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//this->boundingSphere.Radius = meshData.getRadius();
	this->boundingSphere.Radius = 1.0f;
}

Mesh::~Mesh()
{
}

void Mesh::setMaterial(DirectX::SimpleMath::Vector3 ambient, DirectX::SimpleMath::Vector3 specular, float specularPower)
{
	this->ambient = ambient;
	this->specular = specular;
	this->specularPower = specularPower;
}

bool Mesh::createTexture(std::string file)
{
	this->texture.loadTexture(file);
	
	return true;
}

void Mesh::setPosition(DirectX::SimpleMath::Vector3 pos)
{
	this->pos = pos;
	this->boundingSphere.Center = pos;
}

void Mesh::setRotation(DirectX::SimpleMath::Vector3 rot)
{
	this->rot = rot;
}

void Mesh::setScaling(DirectX::SimpleMath::Vector3 scale)
{
	this->scale = scale;
	this->boundingSphere.Radius =  scale.x * 0.5;
}

void Mesh::setStartIndex(unsigned int index)
{
	this->startIndex = index;
}

void Mesh::move()
{
}

void Mesh::setSpecularPower(float power)
{
	this->specularPower = power;
}

void Mesh::update()
{
	this->updateWorldMatrix();
}
