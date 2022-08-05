#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& graphic)
    :Buffer(graphic, "Index buffer"), nrOfIndex(0)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::createBuffer(MeshData& meshData)
{
    this->nrOfIndex = meshData.getIndices().size();
    
    UINT bufferSize = sizeof(meshData.getIndices()[0]) * this->nrOfIndex;
    
    return Buffer::createBuffer(
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        bufferSize,
        (void*)&meshData.getIndices()[0]
    );
}
