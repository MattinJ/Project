#include "MeshLoader.h"
#include "../Application/ErrorLogger.h"

#include <sstream>

MeshLoader::MeshLoader()
{
}

MeshLoader::~MeshLoader()
{
}

bool MeshLoader::loadModel(const std::string& fileName)
{
    std::string line;
    std::string objPath = "Resources/Models/" + fileName + ".obj";
    std::string matPath = "Resources/Models/" + fileName + ".mtl";
    
    //------------------- OBJ FILES ------------------
    this->file.open(objPath);
    
    if (!this->file.is_open())
    {
        ErrorLogger::errorMessage("Failed to load file" + fileName);
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
               
                Vertex vertex = {};
                vertex.position = this->positions[index[0]];
                vertex.uv = this->uvs[index[1]];
                vertex.normal = this->normals[index[2]];

                this->vertices.push_back(vertex);
            }
   
        }

    }

    std::cout << "--- POSITIONS ---" << std::endl;
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
    for (size_t i = 0; i < this->vertices.size(); i++)
    {
        std::cout << "Position: " << this->vertices[i].position.x << " " << this->vertices[i].position.y << " " << this->vertices[i].position.z << std::endl;
        std::cout << "UV: " << this->vertices[i].uv.x << " " << this->vertices[i].uv.y << std::endl;
        std::cout << "Normal: " << this->vertices[i].normal.x << " " << this->vertices[i].normal.y << " " << this->vertices[i].normal.z << std::endl << std::endl;
    }

    this->file.close();
    
    return false;
}
