#include "MeshLoader.h"
#include "Graphics.h"
#include "../Application/ErrorLogger.h"

#include <sstream>

#include <DirectXMath.h>

using namespace DirectX;

Vertex MeshLoader::makeVert(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector2 uv,
    DirectX::SimpleMath::Vector3 normal)
{    
    return {
        {position.x, position.y, position.z},
        {normal.x, normal.y, normal.z},
        {uv.x, uv.y}
    };
}

bool MeshLoader::addIndex(Vertex& vertex)
{
    XMVECTOR p1 = { 0.0f, 0.0f, 0.0f, 0.0f };
    XMVECTOR n1 = { 0.0f, 0.0f, 0.0f, 0.0f };
    XMVECTOR u1 = { 0.0f, 0.0f, 0.0f, 0.0f };

    XMVECTOR p2 = XMLoadFloat3(&vertex.pos);
    XMVECTOR n2 = XMLoadFloat3(&vertex.normal);
    XMVECTOR u2 = XMLoadFloat2(&vertex.uv);
 
    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        p1 = XMLoadFloat3(&this->vertices[i].pos);
        n1 = XMLoadFloat3(&this->vertices[i].normal);
        u1 = XMLoadFloat2(&this->vertices[i].uv);

        if (XMVector3Equal(p1, p2) && XMVector3Equal(n1, n2) && XMVector2Equal(u1, u2))
        {
            this->indices.push_back((UINT)i);
            return true;
        }
    }

    this->vertices.push_back(vertex);
    this->indices.push_back((UINT(this->vertices.size() - 1)));
    
    return true;
}

void MeshLoader::printObj()
{
    /*std::cout << "--- POSITIONS ---" << std::endl;
    for (size_t i = 0; i < this->positions.size(); i++)
    {
        std::cout << this->positions[i].x << " " << this->positions[i].y << " " << this->positions[i].z << std::endl;
    }

    std::cout << std::endl;
    std::cout << "--- UVS ---" << std::endl;
    for (size_t i = 0; i < this->uvs.size(); i++)
    {
        std::cout << this->uvs[i].x << " " << this->uvs[i].y << std::endl;
    }

    std::cout << std::endl;
    std::cout << "--- NORMALS ---" << std::endl;
    for (size_t i = 0; i < this->normals.size(); i++)
    {
        std::cout << this->normals[i].x << " " << this->normals[i].y << " " << this->normals[i].z << std::endl;
    }

    std::cout << std::endl;
    std::cout << "--- FACES ---" << std::endl;
    for (size_t i = 0; i < (this->vertices.size() / 3); i++)
    {
        int j = i * 3;

        std::cout << "Position: " << this->vertices[j].pos.x << " " << this->vertices[j].pos.y << " " << this->vertices[j].pos.z << " \t";
        std::cout << "Position: " << this->vertices[j + 1].pos.x << " " << this->vertices[j + 1].pos.y << " " << this->vertices[j + 1].pos.z << " \t";
        std::cout << "Position: " << this->vertices[j + 2].pos.x << " " << this->vertices[j + 2].pos.y << " " << this->vertices[j + 2].pos.z << std::endl;

        std::cout << "UV: " << this->vertices[j].uv.x << " " << this->vertices[j].uv.x << "\t\t\t";
        std::cout << "UV: " << this->vertices[j + 1].uv.x << " " << this->vertices[j + 1].uv.y << "\t\t\t";
        std::cout << "UV: " << this->vertices[j + 2].uv.x << " " << this->vertices[j + 2].uv.y << std::endl;

        std::cout << "Normal: " << this->vertices[j].normal.x << " " << this->vertices[j].normal.y << " " << this->vertices[j].normal.z << "\t\t";
        std::cout << "Normal: " << this->vertices[j].normal.x << " " << this->vertices[j + 1].normal.y << " " << this->vertices[j + 2].normal.z << "\t\t";
        std::cout << "Normal: " << this->vertices[j].normal.x << " " << this->vertices[j].normal.y << " " << this->vertices[j].normal.z << std::endl << std::endl;
    }*/

    /*for (size_t i = 0; i < this->indices.size(); i++)
    {
        std::cout << "Indcies: " << this->indices[i] << std::endl << std::endl;
    }*/

    std::cout << "Nr of submeshes: " << this->submeshes.size() << std::endl << std::endl;

    for (size_t i = 0; i < this->submeshes.size(); i++)
    {
        std::cout << "Start index: " << this->submeshes[i].startIndex << " Nr of index: " << this->submeshes[i].numIndices <<
            " Matarial name: ";

        for (int c = 0; c < std::strlen(this->submeshes[i].materialName); c++)
        {
            std::cout << this->submeshes[i].materialName[c];
        }

        std::cout << std::endl;
    }
}

void MeshLoader::printMtl()
{
   /* for (size_t i = 0; i < this->materials.size(); i++)
    {
        std::cout << "Diffuse: " << this->materials[i].getDiffuseTexture() << std::endl << "Ambient: " << this->materials[i].getAmbientTexture() << std::endl <<
            "Specular: " << this->materials[i].getSpecularTexture() << std::endl << "Exponent: " << this->materials[i].getSpecularExponent() << std::endl << std::endl;
    }*/
}

bool MeshLoader::loadObjFile(std::string file)
{
    std::string line;
    std::string objPath = "Resources/Models/" + file + ".obj";
    UINT vertexNr = 0;
    UINT nrOfIndicies = 0;
    std::string m;


    Submesh newSubMesh = {};

    this->file.open(objPath);

    if (!this->file.is_open())
    {
        ErrorLogger::errorMessage("Failed to load file" + file + ".obj");
        return false;
    }

    while (std::getline(this->file, line))
    {
        std::stringstream ss(line);
        std::string firstWord;
        std::string v[3];

        std::getline(ss, firstWord, ' ');

        if (firstWord == "v") //Positions
        {
            for (int i = 0; i < 3; i++)
            {
                std::getline(ss, v[i], ' ');
            }
            DirectX::SimpleMath::Vector3 temp = DirectX::SimpleMath::Vector3(std::stof(v[0]), std::stof(v[1]), std::stof(v[2]));
            this->positions.push_back(temp);

        }
        else if (firstWord == "vt") //UV
        {
            for (int i = 0; i < 2; i++)
            {
                std::getline(ss, v[i], ' ');
            }
            DirectX::SimpleMath::Vector2 temp = DirectX::SimpleMath::Vector2(std::stof(v[0]), std::stof(v[1]));
            this->uvs.push_back(temp);
        }
        else if (firstWord == "vn") //normals
        {
            for (int i = 0; i < 3; i++)
            {
                std::getline(ss, v[i], ' ');
            }
            DirectX::SimpleMath::Vector3 temp = DirectX::SimpleMath::Vector3(std::stof(v[0]), std::stof(v[1]), std::stof(v[2]));
            this->normals.push_back(temp);
        }
        else if (firstWord == "g") //New submesh
        {

            if (nrOfIndicies > 0)
            {
                newSubMesh.numIndices = nrOfIndicies;
                newSubMesh.startIndex = (UINT)(this->indices.size() - nrOfIndicies);
                strcpy_s(newSubMesh.materialName, m.c_str());
                this->submeshes.push_back(newSubMesh);
                nrOfIndicies = 0;
            }
        }
        else if (firstWord == "usemtl") //Material
        {
            std::getline(ss, m, ' ');
        }
        else if (firstWord == "f") //face
        {
            std::string f[3];
            int index[3] = { 0,0,0 };

            for (int i = 0; i < 3; i++)
            {
                std::getline(ss, v[i], ' ');
            }

            for (int i = 0; i < 3; i++)
            {
                std::stringstream ssTemp(v[i]);

                for (int j = 0; j < 3; j++)
                {
                    std::getline(ssTemp, f[j], '/');
                    index[j] = (std::stoi(f[j]) - 1);
                }

                Vertex newVertex = this->makeVert(this->positions[index[0]], this->uvs[index[1]], this->normals[index[2]]);
                this->addIndex(newVertex);
                nrOfIndicies++;
            }
        }
    }

    newSubMesh.numIndices = nrOfIndicies;
    newSubMesh.startIndex = (UINT)(this->indices.size() - nrOfIndicies);
    strcpy_s(newSubMesh.materialName, m.c_str());
    this->submeshes.push_back(newSubMesh);
    nrOfIndicies = 0;

    this->createMesh();

    //this->printObj();

    this->file.close();

    return true;
}

bool MeshLoader::loadMtlFile(std::string file)
{
    std::string line;
    std::string objPath = "Resources/Models/" + file + ".mtl";
    UINT vertexNr = 0;
    UINT nrOfIndicies = 0;
    std::string m, s = "";
    std::string map[3] = { "" };

    this->file.open(objPath);

    if (!this->file.is_open())
    {
        ErrorLogger::errorMessage("Failed to load file" + file + ".mtl");
        return false;
    }

    while (std::getline(this->file, line))
    {
        std::stringstream ss(line);
        std::string firstWord;

        std::getline(ss, firstWord, ' ');

        if (firstWord == "newmtl") //Material name
        {
            std::getline(ss, m, ' ');
        }
        else if (firstWord.length() == 0)
        {
            if (s.length() != 0)
            {
                if (map[0].length() == 0)
                    this->mTexture[0] = "defaultDiffuseTexture.png";
                else
                    this->mTexture[0] = map[0];

                if (map[1].length() == 0)
                    this->mTexture[1] = "defaultSpecularTexture.png";
                else
                    this->mTexture[1] = map[1];

                if (map[2].length() == 0)
                    this->mTexture[2] = "defaultAmbientTexture.png";
                else
                    this->mTexture[2] = map[2];

                this->specularExponent = std::stof(s);

                Material newMat;
                newMat.setDiffuseTexture(this->mTexture[0]);
                newMat.setSpecularTexture(this->mTexture[1]);
                newMat.setAmbientTexture(this->mTexture[2]);
                newMat.setSpecularExponent(this->specularExponent);

                this->materials.insert(std::pair<std::string, Material>(m, newMat));
            }
        }
        else if (firstWord == "Ns") //specular exponent
        {
            std::getline(ss, s, ' ');
        }
        else if (firstWord == "map_Kd") //map diffuse
        {
            std::getline(ss, map[0], ' ');
        }
        else if (firstWord == "map_Ks") //map specular
        {
            std::getline(ss, map[1], ' ');
        }
        else if (firstWord == "map_Ka") //map ambient
        {
            std::getline(ss, map[2], ' ');
        }
    }

    if (map[0].length() == 0)
        this->mTexture[0] = "defaultDiffuseTexture.png";
    else
        this->mTexture[0] = map[0];

    if (map[1].length() == 0)
        this->mTexture[1] = "defaultSpecularTexture.png";
    else
        this->mTexture[1] = map[1];

    if (map[2].length() == 0)
        this->mTexture[2] = "defaultAmbientTexture.png";
    else
        this->mTexture[2] = map[2];

    this->specularExponent = std::stof(s);

    Material newMat;
    newMat.setDiffuseTexture(this->mTexture[0]);
    newMat.setSpecularTexture(this->mTexture[1]);
    newMat.setAmbientTexture(this->mTexture[2]);
    newMat.setSpecularExponent(this->specularExponent);

    this->materials.insert(std::pair<std::string, Material>(m, newMat));
    
    this->printMtl();

    this->file.close();

    return true;
}

void MeshLoader::createMesh()
{
    MeshData meshData;
    
    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        meshData.addVertex(this->vertices[i]);
    }

    for (size_t i = 0; i < this->indices.size(); i++)
    {
        meshData.addIndex(this->indices[i]);
    }

    for (size_t i = 0; i < this->submeshes.size(); i++)
    {
        meshData.addSubMesh(this->submeshes[i]);
    }
    
    Mesh* mesh = new Mesh(this->graphic, std::move(meshData));
    mesh->setPosition(DirectX::SimpleMath::Vector3(4.0f, 0.0f, -4.0f));
    this->meshes.push_back(mesh);
}

void MeshLoader::addMaterial(std::string name, std::string diffuse, std::string specular, std::string ambient, float exponent)
{
    Material newMat;
    newMat.setDiffuseTexture(diffuse);
    newMat.setSpecularTexture(specular);
    newMat.setAmbientTexture(ambient);
    newMat.setSpecularExponent(exponent);

    this->materials.insert(std::pair<std::string, Material>(name, newMat));
}

MeshLoader::MeshLoader(Graphics& graphic)
    :graphic(graphic), specularExponent(0.0f)
{
    this->addMaterial("defaultMaterial", "defaultDiffuseTexture.png");
    this->addMaterial("texture3d_blue", "texture3d_blue.png");
    this->addMaterial("texture3d_green", "texture3d_green.png");
    this->addMaterial("texture3d_purple", "texture3d_purple.png");
    this->addMaterial("texture3d_yellow", "texture3d_yellow.png");
    this->addMaterial("lavarock", "lavarock.jpg");
    this->addMaterial("ground", "ground.jpg");
    this->addMaterial("brick", "brick.jpg");
}

MeshLoader::~MeshLoader()
{
}

bool MeshLoader::loadModel(const std::string& fileName)
{
    this->loadObjFile(fileName);
    this->loadMtlFile(fileName);

    return true;
}

Material& MeshLoader::getMaterial(const char* materialName)
{
    return this->materials[materialName];
}
