#pragma once

#include <string>
#include <vector>
#include <d3d11.h>

class InputLayoutDesc
{
private:
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;

public:
	InputLayoutDesc();
	virtual ~InputLayoutDesc();

	void add(const LPCSTR& semanticName, const DXGI_FORMAT& format, const unsigned int semanticIndex = 0);

	inline std::vector<D3D11_INPUT_ELEMENT_DESC>& getInputElementDesc() { return this->inputElementDesc; }
};

