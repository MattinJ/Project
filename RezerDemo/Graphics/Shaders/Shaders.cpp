#include "Shaders.h"
#include "../Graphics.h"
#include "../../Application/ErrorLogger.h"

#include <d3dcompiler.h>

#pragma comment(lib, "D3DCompiler.lib")

bool Shaders::loadShader(std::wstring shaderFile, ID3DBlob*& blob)
{
    std::wstring filePath = L"CompiledShaders/" + shaderFile + L".cso";
    
    HRESULT hr = D3DReadFileToBlob(filePath.c_str(), &blob);

    return ErrorLogger::hrError(hr, "Failed to load shader.");
}

Shaders::Shaders(Graphics& graphic)
    :graphic(graphic)
{
}

Shaders::~Shaders()
{
}
