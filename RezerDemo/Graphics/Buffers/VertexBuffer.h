#pragma once
#include "Buffer.h"
#include <vector>
#include "../MeshData.h"

class VertexBuffer : public Buffer
{
private:
	UINT stride;
	UINT offset;

public:
	VertexBuffer(Graphics& graphic);
	virtual ~VertexBuffer();

	bool createBuffer(MeshData& meshData);

	inline UINT& getStride() { return this->stride; }
	inline UINT& getOffset() { return this->offset; }

};

