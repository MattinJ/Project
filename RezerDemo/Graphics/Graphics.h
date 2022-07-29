#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Buffers/ConstantBuffer.h"

#include "Shaders/PixelShader.h"
#include "Shaders/VertexShader.h"

#include "Light.h"

#include "Mesh.h"
#include "Texture.h"

#include "Camera.h"

#include "../Application/Window.h"

const int BUFFER_COUNT = 4;

class Graphics
{
private:
	struct LightStruct
	{
		DirectX::XMFLOAT3 position;
		float range;
		DirectX::XMFLOAT3 direction;
		float cone;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 att;
		int lightType;
	} lightBufferStruct{};
	
	struct mvpBuffer
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT4X4 vpMatrix;
	} mvpBufferStruct{};

	struct MaterialStruct
	{
		DirectX::XMFLOAT4 ambient;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 specular;
		float specularPower;
	} materialBufferStruct{};

	//Device, swapchain and Context.
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swapchain = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;

	ID3D11UnorderedAccessView* uavBackBuffer = nullptr;
	ID3D11Resource* backBuffer = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11RenderTargetView* nullRTV = nullptr;

	//Gbuffers
	ID3D11Texture2D* rtvTextureArray[BUFFER_COUNT];
	ID3D11RenderTargetView* rtvArray[BUFFER_COUNT];
	ID3D11ShaderResourceView* srvArray[BUFFER_COUNT];

	ID3D11RenderTargetView* nullRTVarray[BUFFER_COUNT];
	
	D3D11_VIEWPORT viewPort;

	ID3D11DepthStencilView* dsv = nullptr;
	ID3D11Texture2D* dsTexture = nullptr;
	ID3D11DepthStencilState* dsState = nullptr;

	//Shaders
	VertexShader deferred_VS;
	PixelShader deferred_PS;
	ID3D11ComputeShader* deffered_CS = nullptr;

	UINT threadX;
	UINT threadY;
	UINT threadZ;

	//Buffers
	ID3D11Buffer* default_VS_Buffer = nullptr;

	//Texture
	/*Texture defaultTexture;
	Texture grassTexture;*/

	//Light
	Light light;
	ID3D11Buffer* cbMaterial = nullptr;

	//Camera
	Camera camera;
	ConstantBuffer cameraPos;
	ID3D11Buffer* cameraBuff = nullptr;

	//Constant buffers
	ConstantBuffer mvpConstantBuffer;
	ConstantBuffer materialCB;

	//Window
	Window* window;

	//Meshes
	std::vector<Mesh*> meshes;

	//Private help functions
	bool resourceManagement();
	bool createDevices();
	bool defferdInit();
	bool loadShaders();
	bool initMeshes();

	void renderMesh(Mesh& mesh);

	void shadowMap();
	void geometryPass();
	void lightPass();

	//Private help variables.
	float angle = 0;

public:
	Graphics();
	virtual ~Graphics();

	bool init(Window& window);
	void render();

	void presentSC();

	inline ID3D11Device* getDevice() const { return this->device; }
	inline ID3D11DeviceContext* getDeviceContext() const { return this->immediateContext; }

};

