#include "QuadTree.h"
#include "../Application/ErrorLogger.h"

using namespace DirectX::SimpleMath;
#include <iostream>

void QuadTree::createTree(Node*& node, int depth)
{
    Vector3 dirVectors[4] = { {-1.0f, 0.0f, -1.0f}, {1.0f, 0.0f, -1.0f }, {-1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f} };
 
    std::queue<Node*> nodeQueue;
    std::queue<Node*> nextQueue;

    nextQueue.push(node); //Add root

    for (int i = 0; i < depth; i++)
    {
        nodeQueue = nextQueue;
        nextQueue = std::queue<Node*>{};

        while (!nodeQueue.empty())
        {
            Node* currentNode = nodeQueue.front();
            nodeQueue.pop();

            for (int j = 0; j < 4; j++)
            {
                Node* newNode = new Node;

                for (int k = 0; k < 4; k++)
                {
                    newNode->childNodes[k] = nullptr;
                }

                float newExtentValue = currentNode->exentsValue * 0.5f;
                Vector3 newCenterPos = currentNode->centerPos + (dirVectors[j] * newExtentValue * 0.5f);
                Vector3 newExtent = { newExtentValue, this->extenstValue, newExtentValue };

                newNode->centerPos = newCenterPos;
                newNode->bBox = DirectX::BoundingBox(newCenterPos, newExtent);
                newNode->exentsValue = newExtentValue;

                currentNode->childNodes[j] = newNode;
                nextQueue.push(currentNode->childNodes[j]);
                this->addMeshQuadTreeWireMesh(MeshData(DefaultMesh::CUBE), newCenterPos, newExtent);
            }
        }
    }
}

void QuadTree::deleteNodes(Node* node)
{
 
    for (int i = 0; i < 4; i++)
    {
        if (node->childNodes[i] != nullptr)
            this->deleteNodes(node->childNodes[i]);
    }
    
    delete node;
 
}

void QuadTree::addMeshQuadTreeWireMesh(MeshData&& newMeshData, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale)
{
    // Create mesh
    Mesh* newMesh = new Mesh(this->graphic, std::move(newMeshData));
    newMesh->setPosition(pos);
    newMesh->setScaling(scale);

    // Insert mesh
    this->quadTreeWiereMeshes.push_back(newMesh);
}

void QuadTree::addMeshesToRender(std::vector<Mesh*>& nodeMeshes, std::vector<Mesh*>& renderMeshes)
{   
    if (nodeMeshes.size() <= 0)
        return;
    
    for (size_t i = 0; i < nodeMeshes.size(); i++)
    {
        bool addMesh = true;
        
        //Check if mesh already exist
        for (size_t j = 0; j < renderMeshes.size(); j++)
        {
            if (nodeMeshes[i] == renderMeshes[j])
            {
                addMesh = false;
                break;
            }
        }

        if (addMesh == true)
            renderMeshes.push_back(nodeMeshes[i]);
    }
}

QuadTree::QuadTree(Graphics& graphic)
    :graphic(graphic), rootNode(nullptr), centerPosition(0.0f, 0.0f, 0.0f), extents(10.0f, 10.0f, 10.0f)
{
}

QuadTree::~QuadTree()
{
    this->deleteNodes(this->rootNode);

    for (int i = 0; i < this->quadTreeWiereMeshes.size(); i++)
    {
        delete this->quadTreeWiereMeshes[i];
    }
}

bool QuadTree::init()
{
    this->rootNode = new Node;
    
    this->extents = Vector3(this->extenstValue, this->extenstValue, this->extenstValue);

    this->rootNode->exentsValue = this->extenstValue;
    this->rootNode->bBox = DirectX::BoundingBox(this->centerPosition, this->extents);
    this->rootNode->centerPos = this->centerPosition;

    this->addMeshQuadTreeWireMesh(MeshData(DefaultMesh::CUBE), centerPosition, this->extents);

    this->createTree(this->rootNode, this->depth);
 
    return true;
}

bool QuadTree::addMeshToTree(Mesh*& mesh)
{
    std::queue<Node*> nodeQueue;
  
    nodeQueue.push(this->rootNode);

    while (!nodeQueue.empty())
    {
        Node* currentNode = nodeQueue.front();
        nodeQueue.pop();

        for (int i = 0; i < 4; i++)
        {
            if (currentNode->childNodes[i]->bBox.Intersects(mesh->getBoundingSphere()) || currentNode->childNodes[i]->bBox.Contains(mesh->getBoundingSphere()))
            {
                if (currentNode->childNodes[i]->childNodes[0] == nullptr) //leaf node
                    currentNode->childNodes[i]->meshes.push_back(mesh);
                else
                    nodeQueue.push(currentNode->childNodes[i]);
            }
        }
    }

    return true;
}

std::vector<Mesh*> QuadTree::frustumCulling(DirectX::BoundingFrustum& frustum)
{
    std::vector<Mesh*> renderMeshes;
    
    std::queue<Node*> nodeQueue;

    nodeQueue.push(this->rootNode);

    while (!nodeQueue.empty())
    {
        Node* currentNode = nodeQueue.front();
        nodeQueue.pop();

        for (int i = 0; i < 4; i++)
        {
            if (frustum.Intersects(currentNode->childNodes[i]->bBox) || frustum.Contains(currentNode->childNodes[i]->bBox))
            {
                if (currentNode->childNodes[i]->childNodes[0] == nullptr)
                    this->addMeshesToRender(currentNode->childNodes[i]->meshes, renderMeshes);
                else
                    nodeQueue.push(currentNode->childNodes[i]);
            }
        }
    }
    
    return renderMeshes;
}
