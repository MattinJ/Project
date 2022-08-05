#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(Graphics& graphic)
	:Buffer(graphic, "Vertex Buffer"), stride(0), offset(0)
{
}

VertexBuffer::~VertexBuffer()
{
}

bool VertexBuffer::createBuffer(MeshData& meshData)
{
	this->stride = sizeof(Vertex);
	this->offset = 0; 
	
	UINT bufferSize = sizeof(meshData.getVertices()[0]) * meshData.getVertices().size();

	return Buffer::createBuffer(
		D3D11_USAGE_DEFAULT, 
		D3D11_BIND_VERTEX_BUFFER, 
		bufferSize,
		(void*)&meshData.getVertices()[0]
	);
}
