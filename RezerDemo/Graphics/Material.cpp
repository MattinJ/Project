#include "Material.h"

Material::Material()
	:diffuseTexture(""), ambientTexture(""), specularTexture(""), specularExponent(0.0f)
{
}

Material::Material(std::string diffuse, std::string ambient, std::string specular, float exponent)
	:diffuseTexture(diffuse), ambientTexture(ambient), specularTexture(specular), specularExponent(exponent)
{
}

Material::~Material()
{
}

void Material::setDiffuseTexture(std::string diffuse)
{
	this->diffuseTexture = diffuse;
}

void Material::setAmbientTexture(std::string ambient)
{
	this->ambientTexture = ambient;
}

void Material::setSpecularTexture(std::string specular)
{
	this->specularTexture = specular;
}

void Material::setSpecularExponent(float exponent)
{
	this->specularExponent = exponent;
}
