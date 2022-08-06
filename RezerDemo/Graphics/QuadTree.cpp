#include "QuadTree.h"
#include "../Application/ErrorLogger.h"

using namespace DirectX::SimpleMath;
#include <iostream>

void QuadTree::createTree(Node*& node, float extentsValue)
{
    Vector3 dirVectors[4] = { {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f }, {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} };

    for (int i = 0; i < 4; i++)
    {
        Node* childNode = new Node;

        float newExtentsValue = extentsValue * 0.50f;
        Vector3 newCenterPos = { dirVectors[i] * newExtentsValue * 0.50f };
        Vector3 newExtents = { newExtentsValue, newExtentsValue, newExtentsValue };
        Vector3 scale = { newExtentsValue, extentsValue, newExtentsValue };

        childNode->centerPos = newCenterPos;
        childNode->bBox = DirectX::BoundingBox(newCenterPos, newExtents);
        node->childNodes[i] = childNode;

        this->addMesh(MeshData(DefaultMesh::CUBE), newCenterPos, scale);
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            Node* childNode = new Node;

            float newExtentsValue = extentsValue * 0.25f;
            Vector3 newCenterPos = node->childNodes[i]->centerPos + Vector3({ dirVectors[j] * newExtentsValue * 0.50f });
            Vector3 newExtents = { newExtentsValue, newExtentsValue, newExtentsValue };
            Vector3 scale = { newExtentsValue, extentsValue, newExtentsValue };

            childNode->centerPos = newCenterPos;
            childNode->bBox = DirectX::BoundingBox(newCenterPos, newExtents);
            node->childNodes[i]->childNodes[j] = childNode;

            this->addMesh(MeshData(DefaultMesh::CUBE), newCenterPos, scale);
        }
    }
}

void QuadTree::deleteNodes(Node* node)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (node->childNodes[i]->childNodes[j] != nullptr)
                delete node->childNodes[i]->childNodes[j];
        }
        
        if (node->childNodes[i] != nullptr)
            delete node->childNodes[i];

    }

    if (this->rootNode != nullptr)
        delete this->rootNode;
}

void QuadTree::addMeshToNode(Node* node, Mesh*& mesh)
{
    node->meshes.push_back(mesh);
}

void QuadTree::addMesh(MeshData&& newMeshData, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale)
{
    // Create mesh
    Mesh* newMesh = new Mesh(this->graphic, std::move(newMeshData));
    newMesh->setPosition(pos);
    newMesh->setScaling(scale);

    // Insert mesh
    this->meshes.push_back(newMesh);
}

QuadTree::QuadTree(Graphics& graphic)
    :graphic(graphic), rootNode(nullptr), centerPosition(0.0f, 0.0f, 0.0f), extents(10.0f, 10.0f, 10.0f)
{
}

QuadTree::~QuadTree()
{
    this->deleteNodes(this->rootNode);

    for (int i = 0; i < this->meshes.size(); i++)
    {
        delete this->meshes[i];
    }
}

bool QuadTree::init()
{
    this->rootNode = new Node;
    
    this->extents = Vector3(this->extenstValue, this->extenstValue, this->extenstValue);

    this->rootNode->bBox = DirectX::BoundingBox(this->centerPosition, this->extents);
    this->rootNode->centerPos = this->centerPosition;

    this->addMesh(MeshData(DefaultMesh::CUBE), centerPosition, this->extents);

    this->createTree(this->rootNode, this->extenstValue);
 
    std::cout << "Nr of meshes: " << this->meshes.size();

    return true;
}

bool QuadTree::render()
{
    return false;
}
