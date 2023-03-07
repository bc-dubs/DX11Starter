#pragma once

// Ben Coukos-Wiley
// 2/3/2023
// A representation of the physical aspects of an object

#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale);
	Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 pitchYawRoll, DirectX::XMFLOAT3 scale);
	~Transform();

	// Getters
	DirectX::XMFLOAT3* GetPosition();
	DirectX::XMFLOAT4* GetRotation();
	DirectX::XMFLOAT3* GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	DirectX::XMFLOAT3* GetRight();
	DirectX::XMFLOAT3* GetUp();
	DirectX::XMFLOAT3* GetForward();
	float GetPitch();
	float GetYaw();

	// Setters
	void SetPosition(DirectX::XMFLOAT3 newPos);
	void SetPosition(float x, float y, float z);
	void SetRotation(DirectX::XMFLOAT3 newPitchYawRoll);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT4 newQuaternion);
	void SetScale(DirectX::XMFLOAT3 newScale);
	void SetScale(float x, float y, float z);

	// Mutators
	void MoveBy(DirectX::XMFLOAT3 offset);
	void MoveBy(float x, float y, float z);
	void LocalMoveBy(DirectX::XMFLOAT3 offset); // Moves relative to this transform's rotation
	void LocalMoveBy(float x, float y, float z);
	void RotateBy(DirectX::XMFLOAT4 quaternion);
	void RotateBy(DirectX::XMFLOAT3 pitchYawRoll);
	void RotateBy(float pitch, float yaw, float roll);
	void ScaleBy(DirectX::XMFLOAT3 scaleFactor);
	void ScaleBy(float x, float y, float z);

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 scale;
	
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 forward;

	float pitch;
	float yaw;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	// Has the transform changed since the matrix was last calculated?
	bool transformAltered;
	// Has the rotation changed since the matrix was last calculated?
	bool rotationAltered;

	void UpdateMatrices();
	void UpdateRotation();
};

