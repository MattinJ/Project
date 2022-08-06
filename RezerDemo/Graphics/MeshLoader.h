#pragma once
#include <string>
#include <fstream>
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <map>

#include <SimpleMath.h>
#include "MeshData.h"
#include "Mesh.h"
#include "Material.h"

class Graphics;

class MeshLoader
{
private:
	
	Graphics& graphic;
	
	std::ifstream file;
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> normals;
	std::vector<DirectX::SimpleMath::Vector2> uvs;

	std::vector<DirectX::SimpleMath::Vector3> faces;

	std::string mTexture[3];
	float specularExponent;
	
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Submesh> submeshes;

	//std::vector<Material> materials;
	std::map<std::string, Material> materials;

	std::vector<Mesh*> meshes;

	Vertex makeVert(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector2 uv, 
		DirectX::SimpleMath::Vector3 normal);

	bool addIndex(Vertex& vertex);
	void printObj();
	void printMtl();

	bool loadObjFile(std::string file);
	bool loadMtlFile(std::string file);
	void createMesh();
	void addMaterial(std::string name, std::string diffuse, std::string specular = "defaultSpecularTexture.png", std::string ambient = "defaultAmbientTexture.png", float exponent = 32.0f);

public:
	MeshLoader(Graphics& graphic);
	virtual ~MeshLoader();

	bool loadModel(const std::string& fileName);

	inline std::vector<Mesh*>& getMeshes() { return this->meshes; }

	Material& getMaterial(const char* materialName);

};

