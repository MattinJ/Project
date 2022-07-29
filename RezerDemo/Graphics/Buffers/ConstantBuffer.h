#pragma once

#include "Buffer.h"

class ConstantBuffer : public Buffer
{
private:

public:
	ConstantBuffer(Graphics& graphic, const std::string& debugName);
	virtual ~ConstantBuffer();

	void updateBuffer(void* bufferData);

	bool createBuffer(UINT byteWidth, UINT stride, void* initialData);
};

