#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>

class Camera
{
public:
	Camera();
	Camera(float aspectRatio, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 orientation = DirectX::XMFLOAT4(0, 0, 1, 0), float fov = DirectX::XM_PIDIV2, float moveSpeed = 2, float mouseLookSpeed = 0.01, float nearClipDist = 0.01, float farClipDist = 1000, bool leftHanded = true, DirectX::XMFLOAT3 worldUp = DirectX::XMFLOAT3(0, 1, 0));
	~Camera();

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	std::shared_ptr<Transform> GetTransform();
	bool IsLeftHanded();

	// Update functions
	void Update(float dt);
	void UpdateProjectionMatrix(float aspectRatio);

private:
	std::shared_ptr<Transform> transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	float fov;
	float nearClipDist;
	float farClipDist;

	float moveSpeed;
	float mouseLookSpeed;

	bool leftHanded;
	DirectX::XMFLOAT3 worldUp;
	
	void UpdateViewMatrix();
};
