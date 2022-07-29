#pragma once
#include <string>

class MeshLoader
{
private:

public:
	MeshLoader();
	virtual ~MeshLoader();

	bool loadModel(const std::string& fileName);

};

