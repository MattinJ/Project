#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

class Graphics;

class Particle
{
private:
	struct Vertex
	{
		float x;
		float y;
		float z;
	} particleVertex{};

	Graphics& graphic;
	
	ID3D11Buffer* vertexBuffer;
	UINT stride;
	UINT offSet;

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* uav;

	DirectX::SimpleMath::Matrix worldMatrix;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;

	void updateWorldMatrix();
	
	bool initBuffers();
	void initParticle();
	
public:
	Particle(Graphics& graphic);
	~Particle();

	void init();

	void setPosition(DirectX::SimpleMath::Vector3 pos);
	void setRot(DirectX::SimpleMath::Vector3 rot);

	inline UINT& getStride() { return this->stride; }
	inline UINT& getOffSet() { return this->offSet; }

	inline DirectX::SimpleMath::Vector3& getPosition() { return this->position; }
	inline DirectX::SimpleMath::Vector3& getRotation() { return this->rotation; }

	inline DirectX::SimpleMath::Matrix& getWorldMatrix() { return this->worldMatrix; }

	inline ID3D11Buffer*& getVertexBuffer() { return this->vertexBuffer; }
};

