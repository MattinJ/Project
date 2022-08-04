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
		float posX;
		float posY;
		float posZ;

		float xN;
		float yN;
		float zN;

		float u;
		float v;
	};
	
	std::ifstream file;
	std::vector<DirectX::SimpleMath::Vector3> positions;
	std::vector<DirectX::SimpleMath::Vector3> normals;
	std::vector<DirectX::SimpleMath::Vector2> uvs;

public:
	MeshLoader();
	virtual ~MeshLoader();

	bool loadModel(const std::string& fileName);

};

