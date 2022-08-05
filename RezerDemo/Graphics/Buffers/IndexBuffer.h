#pragma once

#include "Buffer.h"
#include "../MeshData.h"

class IndexBuffer : public Buffer
{
private:
	UINT nrOfIndex;

public:
	IndexBuffer(Graphics& graphic);
	virtual ~IndexBuffer();

	bool createBuffer(MeshData& meshData);

	inline const UINT& getNrOFIndex() { return this->nrOfIndex; }
};

