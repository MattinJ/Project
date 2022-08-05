#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

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

	void createTriangle();
	void createQuad();
	void createCube();
	void createTetrahedron();
	void createSphere(int resX = 10, int resY = 10);
	void createPlane(int resX = 10, int resY = 10);

	Vertex makeVert(float posX, float posY, float posZ, float u, float v);

public:
	MeshData();
	MeshData(DefaultMesh defaultMesh);
	virtual ~MeshData();

	bool createDefualtMesh(DefaultMesh mesh);
	void calculateNormals(DefaultMesh mesh);

	void addVertex(const Vertex& newVertex);
	void addIndex(const UINT& newIndice);
	void addSubMesh(const Submesh& newSubmes);

	inline std::vector<Vertex>& getVertices() { return this->vertices; }
	inline std::vector<UINT>& getIndices() { return this->indices; }
	inline std::vector<Submesh>& getSubMeshes() { return this->submeshes; }
};

