#pragma once

#include "Buffer.h"

class IndexBuffer : public Buffer
{
private:
	UINT nrOfIndex;

public:
	IndexBuffer(Graphics& graphic);
	virtual ~IndexBuffer();

	bool createBuffer(UINT bufferDataSize, void* initialData, UINT nrOfIndex);

	inline const UINT& getNrOFIndex() { return this->nrOfIndex; }
};

