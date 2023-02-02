#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	// Get functions

	// Set functions [new pos x y z]

	// Mutators [move pos by x y z, move pos by XMFLOAT3]

	// Get world matrix -- probably also recalculate matrix here?

	DirectX::XMFLOAT4X4 GetWorldMatrix();

private:
	// position vector
	DirectX::XMFLOAT3 position;
	// rotation vector OR quaternion
	// scale vector

	// world matrix (4x4)
};

