#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

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
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

struct Submesh
{
	char materialName[64];

	unsigned int startIndex;
	unsigned int numIndices;
};

class MeshData
{
private:
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Submesh> submeshes;
	float radius;

	void createTriangle();
	void createQuad();
	void createCube();
	void createTetrahedron();
	void createSphere(int resX = 10, int resY = 10);
	void createPlane(int resX = 10, int resY = 10);

	Vertex makeVert(float posX, float posY, float posZ, float u, float v);

public:
	MeshData();
	MeshData(DefaultMesh defaultMesh, std::string materialName = "defaultDiffuseTexture.png");
	virtual ~MeshData();

	bool createDefualtMesh(DefaultMesh mesh, std::string texture);
	void calculateNormals(DefaultMesh mesh);

	void addVertex(const Vertex& newVertex);
	void addIndex(const UINT& newIndice);
	void addSubMesh(const Submesh& newSubmes);
	
	void setRadius(float radius);
	
	inline float getRadius() { return this->radius; }

	inline std::vector<Vertex>& getVertices() { return this->vertices; }
	inline std::vector<UINT>& getIndices() { return this->indices; }
	inline std::vector<Submesh>& getSubMeshes() { return this->submeshes; }
};

