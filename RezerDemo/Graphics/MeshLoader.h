#pragma once
#include <string>
#include <fstream>
#include <d3d11.h>
#include <iostream>
#include <vector>

#include <SimpleMath.h>

class MeshLoader
{
private:
	struct Vertex
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector3 normal;
		DirectX::SimpleMath::Vector2 uv;
	};
	
	std::ifstream file;
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> normals;
	std::vector<DirectX::SimpleMath::Vector2> uvs;

	std::vector<DirectX::SimpleMath::Vector3> faces;
	
	std::vector<Vertex> vertices;

public:
	MeshLoader();
	virtual ~MeshLoader();

	bool loadModel(const std::string& fileName);

};

