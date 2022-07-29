#include "ConstantBuffer.h"
#include "../Graphics.h"
#include "../../Application/ErrorLogger.h"

ConstantBuffer::ConstantBuffer(Graphics& graphic, const std::string& debugName)
    :Buffer(graphic, debugName)
{
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::updateBuffer(void* bufferData)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubResoruce;

    if (this->getBuffer() == nullptr)
    {
        ErrorLogger::errorMessage("Buffer is nullptr");

        return;
    }

    //Map
    HRESULT hr = this->getGraphics().getDeviceContext()->Map(
        this->getBuffer(),
        NULL,
        D3D11_MAP_WRITE_DISCARD,
        NULL,
        &mappedSubResoruce
    );

    if (FAILED(hr))
        ErrorLogger::errorMessage("Failed to map buffer" + this->getDebugName());

    //Copy over data
    memcpy(mappedSubResoruce.pData, bufferData, Buffer::getBufferSize());

    //Unmap
    this->getGraphics().getDeviceContext()->Unmap(this->getBuffer(), 0);
}

bool ConstantBuffer::createBuffer(UINT byteWidth, UINT stride, void* initialData)
{
    return Buffer::createBuffer(
        D3D11_USAGE_DYNAMIC,
        D3D11_BIND_CONSTANT_BUFFER,
        byteWidth,
        initialData,
        D3D11_CPU_ACCESS_WRITE,
        0,
        stride
    );
}
