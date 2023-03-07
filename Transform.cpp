#include "Transform.h"

using namespace DirectX;

#pragma region === CONSTRUCTORS ===
Transform::Transform() : Transform(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1))
{
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale) 
    : position(position),
    rotation(rotation),
    scale(scale)
{
    XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
    transformAltered = true;
    UpdateRotation();
}

Transform::Transform(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 pitchYawRoll, DirectX::XMFLOAT3 scale)
    : Transform(position, XMFLOAT4(0, 0, 0, 0), scale)
{
    SetRotation(pitchYawRoll);
    UpdateRotation();
}

Transform::~Transform()
{
}
#pragma endregion


#pragma region === GETTERS ===
DirectX::XMFLOAT3* Transform::GetPosition()
{
    return &position;
}

DirectX::XMFLOAT4* Transform::GetRotation()
{
    return &rotation;
}

DirectX::XMFLOAT3* Transform::GetScale()
{
    return &scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    if (transformAltered)
        UpdateMatrices();

    return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
    if (transformAltered)
        UpdateMatrices();

    return worldInverseTransposeMatrix;
}
DirectX::XMFLOAT3* Transform::GetRight()
{
    if (rotationAltered)
        UpdateRotation();

    return &right;
}
DirectX::XMFLOAT3* Transform::GetUp()
{
    if (rotationAltered)
        UpdateRotation();

    return &up;
}
DirectX::XMFLOAT3* Transform::GetForward()
{
    if (rotationAltered)
        UpdateRotation();

    return &forward;
}
float Transform::GetPitch()
{
    return pitch;
}
float Transform::GetYaw()
{
    return yaw;
}
#pragma endregion



#pragma region === SETTERS ===
void Transform::SetPosition(DirectX::XMFLOAT3 newPos)
{
    position = newPos;
    transformAltered = true;
}

void Transform::SetPosition(float x, float y, float z)
{
    position = XMFLOAT3(x, y, z);
    transformAltered = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 newPitchYawRoll)
{
    XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&newPitchYawRoll)));
    transformAltered = true;
    rotationAltered = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
    XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
    transformAltered = true;
    rotationAltered = true;
}

void Transform::SetRotation(DirectX::XMFLOAT4 newQuaternion)
{
    rotation = newQuaternion;
    transformAltered = true;
    rotationAltered = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 newScale)
{
    scale = newScale;
    transformAltered = true;
}

void Transform::SetScale(float x, float y, float z)
{
    scale = XMFLOAT3(x, y, z);
    transformAltered = true;
}
#pragma endregion



#pragma region === MUTATORS ===
void Transform::MoveBy(DirectX::XMFLOAT3 offset)
{
    XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&offset)));
    transformAltered = true;
}

void Transform::MoveBy(float x, float y, float z)
{
    XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMVectorSet(x, y, z, 0)));
    transformAltered = true;
}

void Transform::LocalMoveBy(DirectX::XMFLOAT3 offset)
{
    XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMVector3Rotate(XMLoadFloat3(&offset), XMLoadFloat4(&rotation))));
    transformAltered = true;
}

void Transform::LocalMoveBy(float x, float y, float z)
{
    XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), XMVector3Rotate(XMVectorSet(x, y, z, 0), XMLoadFloat4(&rotation))));
    transformAltered = true;
}

void Transform::RotateBy(DirectX::XMFLOAT4 quaternion)
{
    XMStoreFloat4(&rotation, XMQuaternionMultiply(XMLoadFloat4(&rotation), XMLoadFloat4(&quaternion)));
    transformAltered = true;
    rotationAltered = true;
}

void Transform::RotateBy(DirectX::XMFLOAT3 pitchYawRoll)
{
    XMStoreFloat4(&rotation, XMQuaternionMultiply(XMLoadFloat4(&rotation), XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll))));
    transformAltered = true;
    rotationAltered = true;
}

void Transform::RotateBy(float pitch, float yaw, float roll)
{
    XMStoreFloat4(&rotation, XMQuaternionMultiply(XMLoadFloat4(&rotation), XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)));
    transformAltered = true;
    rotationAltered = true;

}

void Transform::ScaleBy(DirectX::XMFLOAT3 scaleFactor)
{
    XMStoreFloat3(&scale, XMVectorMultiply(XMLoadFloat3(&scale), XMLoadFloat3(&scaleFactor)));
    transformAltered = true;
}

void Transform::ScaleBy(float x, float y, float z)
{
    XMStoreFloat3(&scale, XMVectorMultiply(XMLoadFloat3(&scale), XMVectorSet(x, y, z, 0)));
    transformAltered = true;
}
#pragma endregion



void Transform::UpdateMatrices()
{
    XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
    XMMATRIX r = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
    XMMATRIX s = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

    XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(s, r), t);

    XMStoreFloat4x4(&worldMatrix, world);
    XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));

    transformAltered = false;
}

void Transform::UpdateRotation()
{
    XMStoreFloat3(&right, XMVector3Normalize(XMVector3Rotate(XMVectorSet(1, 0, 0, 0), XMLoadFloat4(&rotation))));
    XMStoreFloat3(&up, XMVector3Normalize(XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&rotation))));
    XMStoreFloat3(&forward, XMVector3Normalize(XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation))));

    XMStoreFloat(&yaw, XMVector3AngleBetweenVectors(XMVectorSet(0, 0, 1, 0), XMVectorSet(forward.x, 0, forward.z, 0)) * copysignf(1, forward.x));
    XMStoreFloat(&pitch, XMVector3AngleBetweenVectors(XMVectorSet(sinf(yaw), 0, cosf(yaw), 0), XMLoadFloat3(&forward)) * copysignf(1, -forward.y));

    rotationAltered = false;
}