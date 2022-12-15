#include "Camera.h"
#include "../Application/Settings.h"
#include "../Application/Input.h"
#include "Graphics.h"
#include "../Application/Time.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void Camera::updateWorldMatrix()
{
	this->worldMatrix =
		DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(this->rotation.x), DirectX::XMConvertToRadians(this->rotation.y), DirectX::XMConvertToRadians(this->rotation.z)) *
		DirectX::XMMatrixTranslation(this->position.x, this->position.y, this->position.z);
}

void Camera::updateDirectionalVectors()
{
	this->updateWorldMatrix();

	this->rightVec = this->worldMatrix.Right();
	this->upVec = this->worldMatrix.Up();
	this->forwardVec = this->worldMatrix.Forward();
}

void Camera::updateCamera()
{
	this->viewMatrix = XMMatrixLookToLH(this->position, this->forwardVec, this->upDirection);
	XMVECTOR determinant;
	XMMATRIX invViewMatrix = XMMatrixInverse(&determinant, this->viewMatrix);

	//update frustum
	this->boundingFrustum.CreateFromMatrix(this->boundingFrustum, this->projectionMatrix);
	this->boundingFrustum.Transform(this->boundingFrustum, invViewMatrix);
	//this->boundingFrustum.Origin = this->position;

	
}

void Camera::move()
{
	//Move
	Vector3 direction((float)(Input::isKeyDown(Keys::D) - Input::isKeyDown(Keys::A)), 
		(float)(Input::isKeyDown(Keys::SPACE) - Input::isKeyDown(Keys::LEFT_SHIFT)),
		(float)(Input::isKeyDown(Keys::W) - Input::isKeyDown(Keys::S)));
	direction.Normalize();
	
	Vector3 right = this->rightVec;
	right.y = 0.0f;
	right.Normalize();
	Vector3 forward = this->forwardVec;
	forward.y = 0.0f;
	forward.Normalize();
	Vector3 moveVec = (right * direction.x + -this->upDirection * direction.y + -forward * direction.z) * this->speed * Time::getDT();

	this->position += -moveVec;

	//Lookaround
	Vector3 rotation(-(float)Input::getCursorDeltaY(), (float)Input::getCursorDeltaX(), 0.0f);
	this->rotation += rotation * 0.5f;
	this->updateDirectionalVectors();

	Vector3 origRot = this->rotation;
	if (origRot.x > 80.0f)
		origRot.x = 80.0f;
	else if (origRot.x < -80.0f)
		origRot.x = -80.0f;
	this->rotation = origRot;
	this->updateDirectionalVectors();

	this->updateCamera();
}

Camera::Camera(Graphics& graphic)
	:graphic(graphic), viewMatrix(DirectX::SimpleMath::Matrix::Identity), projectionMatrix(DirectX::SimpleMath::Matrix::Identity),
	fov(90.0f), aspectRatio(float(s_resolusionX / s_resolusionY)), nearPlane(0.5f), farPlane(100.0f), speed(10.0), sens(0.01f), 
	constantBuffer(graphic, "Camera pos CB"), boundingFrustum()

{
	this->projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(this->fov), this->aspectRatio, this->nearPlane, this->farPlane);
	
	this->updateCamera();
}

Camera::~Camera()
{
}

void Camera::init()
{
	cameraBufferStruct.position = this->position;
	cameraBufferStruct.pad1 = 0.0f;
	this->constantBuffer.createBuffer(sizeof(cameraBufferStruct), sizeof(CameraStruct), &cameraBufferStruct);
}

void Camera::setPosition(float x, float y, float z)
{
	this->position = DirectX::SimpleMath::Vector3(x, y, z);
}

void Camera::update()
{
	this->move();
	this->cameraBufferStruct.position = this->position;
	this->constantBuffer.updateBuffer(&cameraBufferStruct);
}
