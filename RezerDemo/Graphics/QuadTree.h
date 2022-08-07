#pragma once
#include "d3d11.h"
#include <DirectXCollision.h>
#include <SimpleMath.h>
#include <vector>
#include "Mesh.h"
#include <queue>

class Graphics;

class QuadTree
{
private:
	struct Node
	{
		DirectX::BoundingBox bBox;
		DirectX::SimpleMath::Vector3 centerPos;
		float exentsValue;
		Node* childNodes[4];
		std::vector<Mesh*> meshes;
	};

	const int depth = 2;

	Graphics& graphic;

	Node* rootNode;

	DirectX::SimpleMath::Vector3 centerPosition;
	DirectX::SimpleMath::Vector3 extents;
	float extenstValue = 40;

	void createTree(Node*& node, int depth);
	void deleteNodes(Node* node);

	void addMesh(MeshData&& newMeshData, DirectX::SimpleMath::Vector3 pos = { 0.0f, 0.0f, 0.0f }, DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f });

	std::vector<Mesh*> meshes;

public:
	QuadTree(Graphics& graphic);
	~QuadTree();

	bool init();
	bool addMeshToTree(Mesh*& mesh);
	bool render();

	inline std::vector<Mesh*>& getMeshes() { return this->meshes; }
};

