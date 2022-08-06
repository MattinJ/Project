#include "Resources.h"
#include "ErrorLogger.h"

Resources::Resources(Graphics& graphic)
    :graphic(graphic)
{
}

Resources::~Resources()
{
	// Textures
	for (std::map<std::string, Texture*>::iterator itr = this->textures.begin(),
		itr_end = this->textures.end(); itr != itr_end; ++itr)
	{
		delete itr->second;
	}

	// Materials
	for (std::map<std::string, Material*>::iterator itr = this->materials.begin(), itr_end = this->materials.end(); itr != itr_end; ++itr)
	{
		delete itr->second;
	}

	for (size_t i = 0; i < this->meshes.size(); i++)
	{
		delete meshes[i];
	}
}

void Resources::addTexture(const std::string& textureFilePath)
{
	if (this->textures.count(textureFilePath) > 0)
	{
		ErrorLogger::errorMessage(textureFilePath + " is already added.");
		return;
	}

	Texture* newTexture = new Texture(this->graphic);
	newTexture->loadTexture(textureFilePath);

	this->textures.insert(std::pair<std::string, Texture*>(textureFilePath, newTexture));
}

void Resources::addMesh(MeshData&& newMeshData, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scale)
{
	// Create mesh
	Mesh* newMesh = new Mesh(this->graphic, std::move(newMeshData));
	newMesh->setPosition(pos);
	newMesh->setScaling(scale);

	// Insert mesh
	this->meshes.push_back(newMesh);

}

void Resources::addMaterial(const std::string& materialName, const std::string& diffuseTextureName,
	const std::string& ambientTextureName, const std::string& specularextureName, float exponent)
{
	if (this->materials.count(materialName) > 0)
	{
		ErrorLogger::errorMessage(materialName + " is already added.");
		return;
	}

	// Create material
	Material* newMaterial = new Material(diffuseTextureName, ambientTextureName, specularextureName, exponent);

	// Insert material
	this->materials.insert(std::pair<std::string, Material*>(materialName, newMaterial));
}

Texture& Resources::getTexture(const char* textureName)
{
	Texture* foundTexture = this->textures[textureName];

	if (!foundTexture)
		ErrorLogger::errorMessage("Texture do not exist: " + std::string(textureName));

	return *foundTexture;
}		

Material& Resources::getMaterial(const char* materialName)
{
	Material* foundMaterial = this->materials[materialName];

	if (!foundMaterial)
		ErrorLogger::errorMessage("Matrial do not exist: " + std::string(materialName));

	return *foundMaterial;
}
