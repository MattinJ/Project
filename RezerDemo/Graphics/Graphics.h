#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

#include "Buffers/ConstantBuffer.h"

#include "Shaders/PixelShader.h"
#include "Shaders/VertexShader.h"

#include "Light.h"
#include "ParticleSystem.h"
#include "CubeMap.h"

#include "Mesh.h"
#include "Texture.h"

#include "Camera.h"

#include "Tessellering.h"

#include "../Application/Window.h"

const int BUFFER_COUNT = 6;

class Graphics
{
private:
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

	struct CubeMapBuffer
	{
		int index;
		int backBuffer;
		DirectX::XMFLOAT2 pad;

	}cubemapStruct{};

	struct TesserlingBuffer
	{
		DirectX::XMFLOAT3 cameraPos;
		float pad1;
		DirectX::XMFLOAT3 objetPos;
		float pad2;

	}tesserlingStruct{};

	//Device, swapchain and Context.
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swapchain = nullptr;
	ID3D11DeviceContext* immediateContext = nullptr;

	ID3D11UnorderedAccessView* uavBackBuffer = nullptr;
	ID3D11RenderTargetView* rtvBackBuffer = nullptr;
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

	//Light
	Light light;
	ID3D11Buffer* cbMaterial = nullptr;

	//Textures
	Texture ambientTexture;
	Texture specularTexture;

	CubeMap cubemap;

	//Camera
	Camera camera;
	ConstantBuffer cameraPos;
	ID3D11Buffer* cameraBuff = nullptr;

	//Tesserlering
	Tessellering tesselering;
	Mesh lodMesh;

	//Constant buffers
	ConstantBuffer mvpConstantBuffer;
	ConstantBuffer materialCB;
	ConstantBuffer cubemapCB;
	ConstantBuffer lodCB;

	//Window
	Window* window;

	//Meshes
	std::vector<Mesh*> meshes;

	//Particles
	ParticleSystem particleSystem;

	//Private help functions
	bool resourceManagement();
	bool createViews();
	bool defferdInit();
	bool loadShaders();
	bool initMeshes();

	void renderMesh(Mesh& mesh);
	void renderCubeMap(Mesh& mesh);

	void shadowMap();
	void geometryPass();
	void lightPass();
	void renderCubeMapTexture();
	void lodPass();

	void swapRasterState();
	int rasterStateValue = 1;

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

