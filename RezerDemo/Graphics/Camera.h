#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <iostream>

#include "Buffers/ConstantBuffer.h"

class Graphics;

class Camera
{
private:
	struct CameraStruct
	{
		DirectX::XMFLOAT3 position;
		float pad1;
	} cameraBufferStruct{};

	Graphics& graphic;
	
	DirectX::BoundingFrustum boundingFrustum;

	DirectX::SimpleMath::Matrix worldMatrix;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;

	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, -4.0f);
	DirectX::SimpleMath::Vector3 target = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);
	DirectX::SimpleMath::Vector3 upDirection = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);

	DirectX::SimpleMath::Vector3 rightVec = DirectX::SimpleMath::Vector3(1.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3 upVec = DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f);
	DirectX::SimpleMath::Vector3 forwardVec = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f);

	DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;

	float speed;
	float sens;

	ConstantBuffer constantBuffer;

	void updateWorldMatrix();
	void updateDirectionalVectors();

	void updateCamera();
	void move();

public:
	Camera(Graphics& graphic);
	~Camera();

	void init();
	void update();

	void setPosition(float x, float y, float z);

	inline const DirectX::SimpleMath::Matrix& getViewMatrix() { return this->viewMatrix; }
	inline const DirectX::SimpleMath::Matrix& getProjectionMatrix() { return this->projectionMatrix; }

	inline const DirectX::SimpleMath::Vector3& getPostion() { return this->position; }

	inline DirectX::BoundingFrustum& getFrustum() { return this->boundingFrustum; }

	inline ConstantBuffer& getConstantBuffer() { return this->constantBuffer; }
};