#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <vector>

#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Texture.h"

#include "MeshData.h"

class Mesh
{
private:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	std::vector<Submesh> subMeshes;
	Submesh entireMesh;

	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 rot;
	DirectX::SimpleMath::Vector3 scale;

	DirectX::SimpleMath::Matrix worldMatrix;

	UINT startIndex;

	Texture texture;

	//Matrial
	DirectX::SimpleMath::Vector3  ambient;
	DirectX::SimpleMath::Vector3  specular;
	float specularPower;

	void updateWorldMatrix();

public:
	Mesh(Graphics& renderer, MeshData&& meshData);
	virtual ~Mesh();

	bool createTexture(std::string file);

	inline VertexBuffer& getVertexBuffer() { return this->vertexBuffer; }
	inline IndexBuffer& getIndexBuffer() { return this->indexBuffer; }
	inline std::vector<Submesh>& getSubmeshes() { return this->subMeshes; }
	inline const Submesh& getEntireSubmesh() { return this->entireMesh; }
	
	inline DirectX::SimpleMath::Vector3& getAmbient() { return this->ambient; }
	inline DirectX::SimpleMath::Vector3& getSpecular() { return this->specular; }
	inline float& getSpecularPower() { return this->specularPower; }

	void setPosition(DirectX::SimpleMath::Vector3 pos);
	void setRotation(DirectX::SimpleMath::Vector3 rot);
	void setScaling(DirectX::SimpleMath::Vector3 scale);

	void setMaterial(DirectX::SimpleMath::Vector3 ambient, DirectX::SimpleMath::Vector3 specular, float specularPower);
	void setSpecularPower(float power);

	void setStartIndex(unsigned int index);

	void move();

	inline const DirectX::SimpleMath::Vector3& getPosition() const { return this->pos; }
	inline const DirectX::SimpleMath::Vector3& getRotation() const { return this->rot; }
	inline const DirectX::SimpleMath::Vector3& getScaling() const { return this->scale; }

	inline const DirectX::SimpleMath::Matrix& getWorldMatrix() const { return this->worldMatrix; }

	inline const UINT getStartIndex() { return this->startIndex; }

	inline Texture& getTexture() { return this->texture; }

	void update();

};

