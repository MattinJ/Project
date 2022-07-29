#include "IndexBuffer.h"

IndexBuffer::IndexBuffer(Graphics& graphic)
    :Buffer(graphic, "Index buffer"), nrOfIndex(0)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::createBuffer(UINT bufferDataSize, void* initialData, UINT nrOfIndex)
{
    this->nrOfIndex = nrOfIndex;
    
    return Buffer::createBuffer(
        D3D11_USAGE_IMMUTABLE,
        D3D11_BIND_INDEX_BUFFER,
        bufferDataSize,
        initialData
    );
}
