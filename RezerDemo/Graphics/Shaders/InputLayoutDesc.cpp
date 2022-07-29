#include "InputLayoutDesc.h"

InputLayoutDesc::InputLayoutDesc()
{
}

InputLayoutDesc::~InputLayoutDesc()
{
}

void InputLayoutDesc::add(const LPCSTR& semanticName, const DXGI_FORMAT& format, const unsigned int semanticIndex)
{
	D3D11_INPUT_ELEMENT_DESC inputDesc =
	{
		semanticName,
		semanticIndex,
		format,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0
	};

	if(this->inputElementDesc.size() > 0)
		inputDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

	this->inputElementDesc.push_back(inputDesc);

}
