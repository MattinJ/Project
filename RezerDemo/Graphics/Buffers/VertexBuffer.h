#pragma once
#include "Buffer.h"
#include <vector>

class VertexBuffer : public Buffer
{
private:
	UINT stride;
	UINT offset;

public:
	VertexBuffer(Graphics& graphic);
	virtual ~VertexBuffer();

	bool createBuffer(UINT stride, UINT bufferDataSize, void* initialData);

	inline UINT& getStride() { return this->stride; }
	inline UINT& getOffset() { return this->offset; }

};

