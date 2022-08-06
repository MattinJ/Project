#pragma once

#include <string>

class Material
{
private:
	std::string diffuseTexture;
	std::string ambientTexture;
	std::string specularTexture;

	float specularExponent;

public:
	Material();
	Material(std::string diffuse, std::string ambient, std::string specular, float exponent);
	~Material();

	void setDiffuseTexture(std::string diffuse);
	void setAmbientTexture(std::string ambient);
	void setSpecularTexture(std::string specular);
	void setSpecularExponent(float exponent);

	inline const std::string& getDiffuseTexture() { return this->diffuseTexture; }
	inline const std::string& getAmbientTexture() { return this->ambientTexture; }
	inline const std::string& getSpecularTexture() { return this->specularTexture; }
	inline const float getSpecularExponent() { return this->specularExponent; }
};

