#pragma once

#include <d3d11.h>
#include <string>

class Graphics;

class Buffer
{
private:
	Graphics& graphic;
	
	ID3D11Buffer* buffer;
	UINT bufferSize;

	std::string debugName;

protected:
	bool createBuffer(D3D11_USAGE usage, UINT bindFlags, UINT bufferDataSize, void* initialData, UINT cpuAccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0);
	inline Graphics& getGraphics() { return this->graphic; }

public:
	Buffer(Graphics& graphic, const std::string& debugName);
	virtual ~Buffer();

	inline ID3D11Buffer*& getBuffer() { return this->buffer; }
	inline UINT& getBufferSize() { return this->bufferSize; }
	inline std::string getDebugName() { return this->debugName; }
};

