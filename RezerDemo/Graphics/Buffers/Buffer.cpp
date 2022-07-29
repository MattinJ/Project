#include "Buffer.h"
#include "../../Application/ErrorLogger.h"
#include "../Graphics.h"

bool Buffer::createBuffer(D3D11_USAGE usage, UINT bindFlags, UINT bufferDataSize, void* initialData, UINT cpuAccessFlags, UINT miscFlags, UINT structureByteStride)
{
    if (this->buffer != nullptr)
        this->buffer->Release();

    this->bufferSize = bufferDataSize;

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = usage;
    bufferDesc.ByteWidth = bufferDataSize;
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.CPUAccessFlags = cpuAccessFlags;
    bufferDesc.MiscFlags = miscFlags;
    bufferDesc.StructureByteStride = structureByteStride;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = initialData;
    subData.SysMemPitch = 0;
    subData.SysMemSlicePitch = 0;

    HRESULT hr = this->graphic.getDevice()->CreateBuffer(&bufferDesc, 
        initialData != NULL ? &subData : NULL,
        &this->buffer
    );
    
    return ErrorLogger::hrError(hr, "Failed to create buffer" + this->debugName);
}

Buffer::Buffer(Graphics& graphic, const std::string& debugName)
    :graphic(graphic), buffer(nullptr), bufferSize(0), debugName("")
{
}

Buffer::~Buffer()
{
    if (this->buffer != nullptr)
        this->buffer->Release();
}
