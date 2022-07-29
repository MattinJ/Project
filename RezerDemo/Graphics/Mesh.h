#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <vector>

#include "Buffers/VertexBuffer.h"
#include "Buffers/IndexBuffer.h"
#include "Texture.h"

enum class DefaultMesh
{
	TRIANGLE,
	QUAD,
	CUBE,
	SPHERE,
	TETRAHEDRON,
	PLANE
};

struct Vertex
{
	//Position
	float posX;
	float posY;
	float posZ;

	//Normal
	float xN;
	float yN;
	float zN;

	//UV coords
	float u;
	float v;
};

class Mesh
{
private:
	VertexBuffer vertexBuffer;
	IndexBuffer indexBuffer;

	std::vector<Vertex> vertices;
	std::vector<UINT> indices;

	Vertex makeVert(float posX, float posY, float posZ, float u, float v);

	DirectX::SimpleMath::Vector3 pos;
	DirectX::SimpleMath::Vector3 rot;
	DirectX::SimpleMath::Vector3 scale;

	DirectX::SimpleMath::Matrix worldMatrix;

	UINT startIndex;

	Texture texture;

	void updateWorldMatrix();

	void createTriangle();
	void createQuad();
	void createCube();
	void createTetrahedron();
	void createSphere(int resX = 10, int resY = 10);
	void createPlane(int resX = 10, int resY = 10);

	void calculateNormals(DefaultMesh mesh);

public:
	Mesh(Graphics& renderer);
	virtual ~Mesh();

	bool createDefualtMesh(DefaultMesh mesh);

	bool createTexture(std::string file);

	inline VertexBuffer& getVertexBuffer() { return this->vertexBuffer; }
	inline IndexBuffer& getIndexBuffer() { return this->indexBuffer; }

	inline std::vector<Vertex>& getVertices() { return this->vertices; }
	inline std::vector<UINT>& getIndices() { return this->indices; }

public:
	void setPosition(float x, float y, float z);
	void setRotation(float x, float y, float z);
	void setScaling(float x, float y, float z);

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

