#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

#include "Buffers/ConstantBuffer.h"
#include "Shaders/InputLayoutDesc.h"
#include "Shaders/VertexShader.h"

#include "Mesh.h"
#include "Camera.h"

class Graphics;

enum class LightType
{
	DIRECTIONAL,
	SPOT
};

class Light
{
private:
	struct LightBufferVP
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 vpMatrix;
	} lightBufferMVP{};

	struct LightStruct
	{
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 direction;
		float cone;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 att;
		int lightType;
	};

	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;

	VertexShader shadowMap_VS;

	//Shadow map stuff
	D3D11_VIEWPORT shadowMapVP;
	ID3D11Texture2D* shadowMapDepthTexture;
	ID3D11DepthStencilView* shadowMapDSV;
	ID3D11ShaderResourceView* shadowMapSRV;

	ID3D11SamplerState* shadowMapSampler = nullptr;

	unsigned int shadowMapSize;
	
	//Light stuff
	std::vector<LightStruct> lights;

	Graphics& graphic;

	//Constant buffers
	ConstantBuffer shadowMapMVPBuffer;

	//Structure buffer
	ID3D11Buffer* strucutreBuffer = nullptr;
	ID3D11ShaderResourceView* structreSRV = nullptr;
	UINT bufferSize;

	bool createLight(LightType type, DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 dir);

	bool initLights();
	bool initBuffer();

public:
	Light(Graphics& graphic, LightType type = LightType::DIRECTIONAL);
	virtual ~Light();

	bool init();
	void renderShadowMap(std::vector<Mesh*>& meshes);
	bool update();

	inline ID3D11Texture2D*& getShadowMapTexture() { return this->shadowMapDepthTexture; }
	inline ID3D11DepthStencilView*& getShadowMapDSV() { return this->shadowMapDSV; }
	inline ID3D11ShaderResourceView*& getShadowMapSRV() { return this->shadowMapSRV; }

	inline DirectX::SimpleMath::Matrix& getViewMatrix() { return this->viewMatrix; }
	inline DirectX::SimpleMath::Matrix& getProjectionMatrix() { return this->projectionMatrix; }
	inline D3D11_VIEWPORT& getShadowMapVP() { return this->shadowMapVP; }

	inline ConstantBuffer& getShadowMapMVPConstnantBuffer() { return this->shadowMapMVPBuffer; }
	inline ID3D11Buffer*& getStructureBuffer() { return this->strucutreBuffer; }
	inline ID3D11ShaderResourceView*& getStructureSRV() { return this->structreSRV; }

	inline ID3D11SamplerState*& getSampler() { return this->shadowMapSampler; }
};

