#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>
#include <map>

#include "../Graphics/Texture.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"

class Graphics;

class Resources
{
private:
	std::map<std::string, Texture*> textures;
	std::map<std::string, Material*> materials;

	std::vector<Mesh*> meshes;

	Graphics& graphic;

public:
	Resources(Graphics& graphic);
	~Resources();

	void addTexture(const std::string& textureFilePath);
	void addMesh(MeshData&& newMeshData, DirectX::SimpleMath::Vector3 pos = {0.0f, 0.0f, 0.0f}, DirectX::SimpleMath::Vector3 scale = {1.0f, 1.0f, 1.0f});
	void addMaterial(const std::string& diffuseTextureName, 
		const std::string& ambientTextureName = "defaultAmbientTexture.png", const std::string& specularextureName = "defaultSpecularTexture.png", float exponent = 32.0f);

	Texture& getTexture(const char* textureName);
	Material& getMaterial(const char* materialName);
	
	inline std::vector<Mesh*>& getAllMeshes() { return this->meshes; }

};

