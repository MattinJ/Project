#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <DirectXMath.h>
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
	
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;

	DirectX::XMVECTOR camDefualtPosition = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);

	DirectX::XMVECTOR camPosition = DirectX::XMVectorSet(0.0f, 0.0f, -4.0f, 0.0f);
	DirectX::XMVECTOR camTarget = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR camUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMVECTOR DefaultForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR DefaultRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR camForward = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR camRight = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	DirectX::XMMATRIX camRotationMatrix;

	float moveLeftRight = 0.0f;
	float moveBackForward = 0.0f;
	float moveUpDown = 0.0f;

	float camYaw = 0.0f;
	float camPitch = 0.0f;
	float camRoll = 0.0f;

	float fov;
	float aspectRatio;
	float nearPlane;
	float farPlane;

	float speed;
	float sens;

	ConstantBuffer constantBuffer;

	void updateCamera();
	void move();

public:
	Camera(Graphics& graphic);
	~Camera();

	void init();

	void setPosition(float x, float y, float z);
	void setTarget(float x, float y, float z);
	void setUpDirection(float x, float y, float z);

	void setSpeed(float speed);

	void update();

	inline const DirectX::SimpleMath::Matrix& getViewMatrix() { return this->viewMatrix; }
	inline const DirectX::SimpleMath::Matrix& getProjectionMatrix() { return this->projectionMatrix; }

	inline const DirectX::SimpleMath::Vector3& getPostion() { return this->camPosition; }
	inline const DirectX::SimpleMath::Vector3& getTarget() { return this->camTarget; }
	inline const DirectX::SimpleMath::Vector3& getUpDirection() { return this->camUp; }

	inline const float getSpeed() { return this->speed; }
	inline const float getSens() { return this->sens; }

	inline ConstantBuffer& getConstantBuffer() { return this->constantBuffer; }
};