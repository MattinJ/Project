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
        std::string v1;
        std::string v2;
        std::string v3;
       

        std::getline(ss, firstWord, ' ');

        if (firstWord == "v") //Positions
        {
            std::getline(ss, v1, ' ');
            std::getline(ss, v2, ' ');
            std::getline(ss, v3, ' ');

            DirectX::SimpleMath::Vector3 temp = DirectX::SimpleMath::Vector3(std::stof(v1), std::stof(v2), std::stof(v3));

            this->positions.push_back(temp);
     
        }
        else if (firstWord == "vt") //UV
        {
            std::getline(ss, v1, ' ');
            std::getline(ss, v2, ' ');

            DirectX::SimpleMath::Vector2 temp = DirectX::SimpleMath::Vector2(std::stof(v1), std::stof(v2));

            this->uvs.push_back(temp);
        }
        else if (firstWord == "vn") //normals
        {
            std::getline(ss, v1, ' ');
            std::getline(ss, v2, ' ');
            std::getline(ss, v3, ' ');

            DirectX::SimpleMath::Vector3 temp = DirectX::SimpleMath::Vector3(std::stof(v1), std::stof(v2), std::stof(v3));

            this->normals.push_back(temp);
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

    this->file.close();
    
    return false;
}
