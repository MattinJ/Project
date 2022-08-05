#include "Camera.h"
#include "../Application/Settings.h"
#include "../Application/Input.h"
#include "Graphics.h"
#include "../Application/Time.h"

using namespace DirectX;

void Camera::updateCamera()
{
	this->camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, camRoll);
	this->camTarget = XMVector3TransformCoord(this->DefaultForward, this->camRotationMatrix);
	this->camTarget = XMVector3Normalize(this->camTarget);

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY(camYaw);

	this->camRight = XMVector3TransformCoord(this->DefaultRight, RotateYTempMatrix);
	this->camUp = XMVector3TransformCoord(this->camUp, RotateYTempMatrix);
	this->camForward = XMVector3TransformCoord(this->DefaultForward, RotateYTempMatrix);

	this->camPosition += this->moveLeftRight * this->camRight;
	this->camPosition += this->moveBackForward * this->camForward;
	this->camPosition += this->moveUpDown * this->camUp;

	this->moveLeftRight = 0.0f;
	this->moveBackForward = 0.0f;
	this->moveUpDown = 0.0f;

	this->camTarget = this->camPosition + this->camTarget;

	this->viewMatrix = XMMatrixLookAtLH(this->camPosition, this->camTarget, this->camUp);
	
}

void Camera::move()
{
	float move = this->speed * Time::getDT();
	
	if (Input::isKeyDown(Keys::A))
		this->moveLeftRight -= move;
	else if (Input::isKeyDown(Keys::D))
		this->moveLeftRight += move;

	if (Input::isKeyDown(Keys::W))
		this->moveBackForward += move;
	else if (Input::isKeyDown(Keys::S))
		this->moveBackForward -= move;

	if (Input::isKeyDown(Keys::SPACE))
		this->moveUpDown += move;
	else if (Input::isKeyDown(Keys::LEFT_SHIFT))
		this->moveUpDown -= move;

	if (Input::getCursorDeltaX() != 0 || Input::getCursorDeltaY() != 0)
	{
		this->camYaw += Input::getCursorDeltaX() * this->sens;
		this->camPitch += Input::getCursorDeltaY() * this->sens;

		//Snap camera
		if (this->camPitch > 0.9f)
			this->camPitch = 0.9f;
		else if (this->camPitch < -0.9f)
			this->camPitch = -0.9f;
	}
	
	this->updateCamera();
}

Camera::Camera(Graphics& graphic)
	:graphic(graphic), viewMatrix(DirectX::SimpleMath::Matrix::Identity), projectionMatrix(DirectX::SimpleMath::Matrix::Identity),
	fov(90.0f), aspectRatio(float(s_resolusionX / s_resolusionY)), nearPlane(0.5f), farPlane(100.0f), speed(10.0), sens(0.01f), constantBuffer(graphic, "Camera pos CB")

{
	this->projectionMatrix = XMMatrixPerspectiveFovLH(this->fov, this->aspectRatio, this->nearPlane, this->farPlane);
	
	this->updateCamera();
}

Camera::~Camera()
{
}

void Camera::init()
{
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, this->camDefualtPosition);
	
	cameraBufferStruct.position = temp;
	cameraBufferStruct.pad1 = 0.0f;
	this->constantBuffer.createBuffer(sizeof(cameraBufferStruct), sizeof(CameraStruct), &cameraBufferStruct);
}

void Camera::setPosition(float x, float y, float z)
{
	this->camPosition = DirectX::SimpleMath::Vector3(x, y, z);
}

void Camera::setTarget(float x, float y, float z)
{
	this->camTarget = DirectX::SimpleMath::Vector3(x, y, z);
}

void Camera::setUpDirection(float x, float y, float z)
{
	this->camTarget = DirectX::SimpleMath::Vector3(x, y, z);
}

void Camera::setSpeed(float speed)
{
	this->speed = speed;
}

void Camera::update()
{
	this->move();
	
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, this->camPosition);
	this->cameraBufferStruct.position = temp;
	this->constantBuffer.updateBuffer(&cameraBufferStruct);
}
