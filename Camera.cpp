#include "Camera.h"
#include "Input.h"

using namespace DirectX;

Camera::Camera(float aspectRatio, XMFLOAT3 position, XMFLOAT4 orientation, float fov, float moveSpeed, float mouseLookSpeed, float nearClipDist, float farClipDist, bool leftHanded, XMFLOAT3 worldUp)
	: fov(fov),
	moveSpeed(moveSpeed),
	mouseLookSpeed(mouseLookSpeed),
	nearClipDist(nearClipDist),
	farClipDist(farClipDist),
	leftHanded(leftHanded),
	worldUp(worldUp)
{
	transform = std::make_shared<Transform>(Transform(position, orientation, XMFLOAT3(1, 1, 1)));
	projectionMatrix = XMFLOAT4X4();
	viewMatrix = XMFLOAT4X4();
	UpdateProjectionMatrix(aspectRatio);
	UpdateViewMatrix();
}

Camera::~Camera()
{
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

std::shared_ptr<Transform> Camera::GetTransform()
{
	return transform;
}

bool Camera::IsLeftHanded()
{
	return leftHanded;
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();
	int sprintSpeed = 1;
	int handedness = leftHanded ? 1 : -1;

	// Keyboard data
	if (input.KeyDown(VK_SHIFT)) { sprintSpeed = 2; }; // When shift is held down, forward/back speed is doubled

	if (input.KeyDown('W')) { transform->LocalMoveBy(XMFLOAT3(0, 0, dt * moveSpeed * sprintSpeed)); }
	if (input.KeyDown('S')) { transform->LocalMoveBy(XMFLOAT3(0, 0, -dt * moveSpeed * sprintSpeed)); }
	if (input.KeyDown('A')) { transform->LocalMoveBy(XMFLOAT3(- dt * moveSpeed * handedness, 0, 0)); }
	if (input.KeyDown('D')) { transform->LocalMoveBy(XMFLOAT3(dt * moveSpeed * handedness, 0, 0)); }

	if (input.KeyDown(VK_SPACE)) { transform->MoveBy(XMFLOAT3(0, dt * moveSpeed, 0)); }
	if (input.KeyDown('X')) { transform->MoveBy(XMFLOAT3(0, -dt * moveSpeed, 0)); }
	
	
	// Mouse data
	if (input.MouseLeftDown())
	{
		int cursorMovementX = input.GetMouseXDelta();
		int cursorMovementY = input.GetMouseYDelta();

		float maxPitch = XM_PIDIV2 - FLT_EPSILON;
		float minPitch = -XM_PIDIV2 + FLT_EPSILON;
		transform->SetRotation(min(max(minPitch, transform->GetPitch() + cursorMovementY * mouseLookSpeed), maxPitch), transform->GetYaw() + cursorMovementX * mouseLookSpeed * handedness, 0);
	}

	if (input.KeyDown('P')) { transform->SetRotation(XMFLOAT3(0, 0, 0)); }

	UpdateViewMatrix();
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	if (leftHanded) {
		XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClipDist, farClipDist));
	}
	else {
		XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearClipDist, farClipDist));
	}
}

void Camera::UpdateViewMatrix()
{
	if (leftHanded) {
		XMStoreFloat4x4(&viewMatrix, XMMatrixLookToLH(XMLoadFloat3(transform->GetPosition()), XMLoadFloat3(transform->GetForward()), XMLoadFloat3(&worldUp)));
	}
	else {
		XMStoreFloat4x4(&viewMatrix, XMMatrixLookToRH(XMLoadFloat3(transform->GetPosition()), XMLoadFloat3(transform->GetForward()), XMLoadFloat3(&worldUp)));
	}
}
