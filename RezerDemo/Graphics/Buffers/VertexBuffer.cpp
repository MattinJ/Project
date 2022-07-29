#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Graphics& graphic)
	:Buffer(graphic, "Vertex Buffer"), stride(0), offset(0)
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::createBuffer(UINT stride, UINT bufferDataSize, void* initialData )
{
	this->stride = stride;
	this->offset = 0; 
	
	//vertexDesc.ByteWidth = sizeof(vertices);

	return Buffer::createBuffer(
		D3D11_USAGE_DEFAULT, 
		D3D11_BIND_VERTEX_BUFFER, 
		bufferDataSize,
		initialData
	);
}
