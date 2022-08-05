#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Shaders/VertexShader.h"
#include "Shaders/PixelShader.h"
#include "Texture.h"

#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Camera.h"

class Graphics;

class ParticleSystem
{
private:
	struct Vertex
	{
		float x;
		float y;
		float z;
		float fallspeed;
	} particleVertex{};

	struct ParticleSystemBuffer
	{
		DirectX::XMFLOAT4X4 vpMatrix;
		DirectX::XMFLOAT3 cameraPos;
		float pad;
	} particleSystemStruct{};

	struct ParticleBuffer
	{
		DirectX::XMFLOAT3 startPosition;
		float pad1;
		float deltaTime;
		float time;
		float speed;
		float pad2;
	} particleStruct{};

	Graphics& graphic;
	
	ID3D11Buffer* vertexBuffer;
	UINT stride;
	UINT offSet;
	float time = 1;

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* uav;
	ID3D11UnorderedAccessView* uavNull = nullptr;
	
	std::vector<Vertex> particles;

	DirectX::SimpleMath::Matrix worldMatrix;

	DirectX::SimpleMath::Vector3 startPosition;
	
	//Shaders
	VertexShader particle_VS;
	PixelShader particle_PS;
	ID3D11GeometryShader* particle_GS = nullptr;
	ID3D11ComputeShader* particle_CS = nullptr;
	
	int nrOfPartciles = 64;
	float speed = 0.1f;

	Texture particleTexture;

	ConstantBuffer particlesystemCB;
	ConstantBuffer particleCB;

	bool initBuffers();
	bool initShaders();
	void initParticle();
	
	
public:
	ParticleSystem(Graphics& graphic);
	~ParticleSystem();

	void init();
	void render(Camera& camera);
	void update();

	void setStartPosition(DirectX::SimpleMath::Vector3 pos);

	inline UINT& getStride() { return this->stride; }
	inline UINT& getOffSet() { return this->offSet; }


	inline DirectX::SimpleMath::Matrix& getWorldMatrix() { return this->worldMatrix; }

	inline ID3D11Buffer*& getVertexBuffer() { return this->vertexBuffer; }
};

