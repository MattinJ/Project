#include "PixelShader.h"
#include "../Graphics.h"
#include "../../Application/ErrorLogger.h"

PixelShader::PixelShader(Graphics& graphic)
    :Shaders(graphic), pixelShader(nullptr)
{
}

PixelShader::~PixelShader()
{
    if (this->pixelShader != nullptr)
        this->pixelShader->Release();
}

bool PixelShader::loadPS(const std::wstring fileName)
{
    if (this->pixelShader != nullptr)
        this->pixelShader->Release();

    ID3DBlob* blob = nullptr;

    this->loadShader(fileName, blob);

    HRESULT hr = this->getGraphics().getDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &this->pixelShader);

    return ErrorLogger::hrError(hr, "Failed to load pixel shader: ");;
}
