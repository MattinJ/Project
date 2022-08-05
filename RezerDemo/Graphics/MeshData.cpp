#include "MeshData.h"

using namespace DirectX;

MeshData::MeshData()
{
}

MeshData::MeshData(DefaultMesh defaultMesh)
{
	this->createDefualtMesh(defaultMesh);
}

MeshData::~MeshData()
{
}

bool MeshData::createDefualtMesh(DefaultMesh mesh)
{
	switch (mesh)
	{
	case DefaultMesh::TRIANGLE:
	{
		this->createTriangle();
		break;
	}
	case DefaultMesh::QUAD:
	{
		this->createQuad();
		break;
	}
	case DefaultMesh::CUBE:
	{
		this->createCube();
		break;
	}
	case DefaultMesh::SPHERE:
	{
		this->createSphere();
		break;
	}
	case DefaultMesh::TETRAHEDRON:
	{
		this->createTetrahedron();
		break;
	}
	case DefaultMesh::PLANE:
	{
		this->createPlane(20, 20);
		break;
	}
	default:
		break;
	}

	//Add submesh
	Submesh newSubMesh = {};
	newSubMesh.numIndices = this->indices.size();
	this->submeshes.push_back(newSubMesh);
	
	this->calculateNormals(mesh);

	/*this->vertexBuffer.createBuffer(sizeof(Vertex),
		sizeof(this->vertices[0]) * this->vertices.size(),
		(void*)&this->vertices[0]);

	this->indexBuffer.createBuffer(sizeof(this->indices[0]) * this->indices.size(),
		(void*)&this->indices[0],
		this->indices.size()
	);*/

	return true;
}

void MeshData::calculateNormals(DefaultMesh mesh)
{
	if (mesh != DefaultMesh::SPHERE)
	{
		std::vector<XMFLOAT3> vertexNormals;
		vertexNormals.resize(this->vertices.size(), { 0.0f, 0.0f, 0.0f });

		// Add flat normals to each vertex
		for (unsigned int i = 0; i < this->indices.size(); i += 3)
		{
			XMVECTOR vertPos0 = XMVectorSet(this->vertices[this->indices[i + 0]].posX, this->vertices[this->indices[i + 0]].posY, this->vertices[this->indices[i + 0]].posZ, 1.0f);
			XMVECTOR vertPos2 = XMVectorSet(this->vertices[this->indices[i + 1]].posX, this->vertices[this->indices[i + 1]].posY, this->vertices[this->indices[i + 1]].posZ, 1.0f);
			XMVECTOR vertPos1 = XMVectorSet(this->vertices[this->indices[i + 2]].posX, this->vertices[this->indices[i + 2]].posY, this->vertices[this->indices[i + 2]].posZ, 1.0f);

			XMVECTOR edge1 = vertPos1 - vertPos0;
			XMVECTOR edge2 = vertPos2 - vertPos0;

			XMVECTOR normal = XMVector3Normalize(XMVector3Cross(edge2, edge1));

			// Add calculated normal to current list of vertex normals
			XMStoreFloat3(
				&vertexNormals[this->indices[i + 0]],
				XMLoadFloat3(&vertexNormals[this->indices[i + 0]]) + normal
			);
			XMStoreFloat3(
				&vertexNormals[this->indices[i + 1]],
				XMLoadFloat3(&vertexNormals[this->indices[i + 1]]) + normal
			);
			XMStoreFloat3(
				&vertexNormals[this->indices[i + 2]],
				XMLoadFloat3(&vertexNormals[this->indices[i + 2]]) + normal
			);
		}

		// Normalize vector sums
		for (unsigned int i = 0; i < this->vertices.size(); ++i)
		{
			XMVECTOR vec = XMLoadFloat3(&vertexNormals[i]);

			// Normalize, if possible
			XMFLOAT3 dotResult;
			XMStoreFloat3(&dotResult, XMVector3Dot(vec, vec));
			if (dotResult.x > 0.0f)
				vec = XMVector3Normalize(vec);

			// Store normal
			XMFLOAT3 finalNormal;
			XMStoreFloat3(&finalNormal, vec);
			this->vertices[i].xN = finalNormal.x;
			this->vertices[i].yN = finalNormal.y;
			this->vertices[i].zN = finalNormal.z;
		}
	}
	// Sphere normals
	else
	{
		for (unsigned int i = 0; i < this->vertices.size(); ++i)
		{
			XMVECTOR vec = XMVectorSet(this->vertices[i].posX, this->vertices[i].posY, this->vertices[i].posZ, 0.0f);

			// Normalize position
			vec = XMVector3Normalize(vec);

			// Store normal
			XMFLOAT3 finalNormal;
			XMStoreFloat3(&finalNormal, vec);
			this->vertices[i].xN = finalNormal.x;
			this->vertices[i].yN = finalNormal.y;
			this->vertices[i].zN = finalNormal.z;
		}
	}
}

void MeshData::addVertex(const Vertex& newVertex)
{
	this->vertices.push_back(newVertex);
}

void MeshData::addIndex(const UINT& newIndice)
{
	this->indices.push_back(newIndice);
}

void MeshData::addSubMesh(const Submesh& newSubmes)
{
	this->submeshes.push_back(newSubmes);
}

void MeshData::createTriangle()
{
	//Vertices
	this->vertices.push_back(this->makeVert(1.0f, 0.0f, 0.0f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 1.0f, 0.0f, 0.5f, 0.0f));
	this->vertices.push_back(this->makeVert(-1.0f, 0.0f, 0.0f, 1.0f, 1.0f));

	//indices
	for (int i = 0; i < 3; ++i)
		this->indices.push_back(i);
}

void MeshData::createQuad()
{
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, 0.0f, 0.0f, 1.0f));	//Bottom Left	0
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, 0.0f, 0.0f, 0.0f));	//Top Left		1
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, 0.0f, 1.0f, 0.0f));		//Top Right		2
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, 0.0f, 1.0f, 1.0f));	//Bottom Right	3

	for (int i = 0; i < 3; ++i)
	{
		//Triangle 1
		this->indices.push_back(0);
		this->indices.push_back(1);
		this->indices.push_back(3);

		//trinagle 2
		this->indices.push_back(1);
		this->indices.push_back(2);
		this->indices.push_back(3);
	}
}

void MeshData::createCube()
{
	// Front
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, 0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, 0.5f, 0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, 0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, 0.5f, 1.0f, 0.0f));

	// Back
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, -0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, -0.5f, 1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, -0.5f, 0.0f, 0.0f));

	// Top
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, -0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, 0.5f, 1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f));

	// Bottom
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, -0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, 0.5f, 0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, 0.5f, 1.0f, 0.0f));

	// Left
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, -0.5f, 0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(0.5f, 0.5f, 0.5f, 1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, -0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, -0.5f, 0.5f, 1.0f, 1.0f));

	// Right
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, -0.5f, 1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, 0.5f, 0.0f, 1.0f));

	//Indices
	for (int i = 0; i < 6; ++i)
	{
		//Triangle 1
		this->indices.push_back(i * 4 + 0);
		this->indices.push_back(i * 4 + 1);
		this->indices.push_back(i * 4 + 2);

		//triangle 2
		this->indices.push_back(i * 4 + 1);
		this->indices.push_back(i * 4 + 3);
		this->indices.push_back(i * 4 + 2);
	}
}

void MeshData::createTetrahedron()
{
	// Vertices
	this->vertices.push_back(this->makeVert(-0.866f, 0.0f, 0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 0.0f, -1.0f, 0.5f, 0.0f));
	this->vertices.push_back(this->makeVert(0.866f, 0.0f, 0.5f, 1.0f, 1.0f));

	this->vertices.push_back(this->makeVert(0.0f, 0.0f, -1.0f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.866f, 0.0f, 0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 1.5f, 0.0f, 0.5f, 0.0f));

	this->vertices.push_back(this->makeVert(0.866f, 0.0f, 0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 0.0f, -1.0f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 1.5f, 0.0f, 0.5f, 0.0f));

	this->vertices.push_back(this->makeVert(-0.866f, 0.0f, 0.5f, 1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.866f, 0.0f, 0.5f, 0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.0f, 1.5f, 0.0f, 0.5f, 0.0f));


	// Indices
	for (int i = 0; i < 12; ++i)
		this->indices.push_back(i);
}

void MeshData::createSphere(int resX, int resY)
{
	// Default resolution
	if (resX <= 2 || resY <= 2)
	{
		resX = 10;
		resY = 10;
	}

	const float PI = 3.1415f;

	// Vertices
	for (int y = 0; y < resY; ++y)
	{
		double ty = (double)y / (resY - 1);

		for (int x = 0; x < resX; ++x)
		{
			double tx = (double)x / (resX - 1);

			float vx = (float)(sin(tx * 2.0 * PI) * sin(ty * PI));
			float vy = (float)-cos(ty * PI);
			float vz = (float)(cos(tx * 2.0 * PI) * sin(ty * PI));

			float vu = 1.0f - (float)tx;
			float vv = 1.0f - (float)ty;

			this->vertices.push_back(
				this->makeVert(
					vx, vy, vz,
					vu,
					vv
				)
			);
		}
	}

	// Indices
	for (int i = 0; i < (resX - 1) * (resY - 1); ++i)
	{
		int squareX = i % (resX - 1);
		int squareY = i / (resX - 1);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
		this->indices.push_back(squareY * resX + squareX + resX);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + 1);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
	}

	// Create triangles between the 2 vertical edges to avoid 
	// precision errors and z-fighting
	for (int i = 0; i < resY - 1; ++i)
	{
		int squareY = i * resX;

		this->indices.push_back(squareY + resX - 1 + 0);
		this->indices.push_back(squareY + resX - 1 + 1);
		this->indices.push_back(squareY + resX - 1 + resX);

		this->indices.push_back(squareY + resX - 1 + 0);
		this->indices.push_back(squareY + 0);
		this->indices.push_back(squareY + resX + 0);
	}
}

void MeshData::createPlane(int resX, int resY)
{
	// Default resolution
	if (resX <= 1 || resY <= 1)
	{
		resX = 10;
		resY = 10;
	}

	// Vertices
	for (int y = 0; y < resY; ++y)
	{
		for (int x = 0; x < resX; ++x)
		{
			this->vertices.push_back(
				this->makeVert(
					((float)x / (resX - 1)) - 0.5f,
					0,
					((float)-y / (resY - 1)) + 0.5f,

					(float)x / (resX - 1),
					(float)y / (resY - 1)
				)
			);
		}
	}

	// Indices
	for (int i = 0; i < (resX - 1) * (resY - 1); ++i)
	{
		int squareX = i % (resX - 1);
		int squareY = i / (resX - 1);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
		this->indices.push_back(squareY * resX + squareX + resX);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + 1);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
	}
}

Vertex MeshData::makeVert(float posX, float posY, float posZ, float u, float v)
{
	return {
		posX, posY, posZ,
		0.0f, 0.0f, -1.0f,
		u, v
	};
}
