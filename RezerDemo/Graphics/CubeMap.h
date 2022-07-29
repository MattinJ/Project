#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>

const int BUFFER_SIZE = 6;
const int TEXTURE_SIZE = 512;

class Graphics;

class CubeMap
{
private:
	struct CubeMapBuffer
	{
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projectionMatrix;
	} cubeStruct{};

	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;
	
	ID3D11Texture2D* textureArray[BUFFER_SIZE];
	ID3D11RenderTargetView* rtvArray[BUFFER_SIZE];
	ID3D11ShaderResourceView* srvArray[BUFFER_SIZE];
	D3D11_VIEWPORT viewPort;

	ID3D11DepthStencilView* dsv;
	ID3D11DepthStencilState* dss;
	ID3D11Texture2D* dsTexture;

	Graphics& graphic;

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 target;
	DirectX::SimpleMath::Vector3 upDir;



public:
	CubeMap(Graphics& graphic);
	~CubeMap();

	void setPosition(DirectX::SimpleMath::Vector3 pos);
	void setTarget(DirectX::SimpleMath::Vector3 target);
	void setDir(DirectX::SimpleMath::Vector3 dir);

	inline DirectX::SimpleMath::Vector3& getPosition() { return this->position; }
	inline DirectX::SimpleMath::Vector3& getTarget() { return this->target; }
	inline DirectX::SimpleMath::Vector3& getUpDir() { return this->upDir; }

	bool init();
	void render();

};

