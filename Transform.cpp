#include "Transform.h"

using namespace DirectX;

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);
    XMMATRIX r;
    XMMATRIX s;

    XMMATRIX world = s * r * t;

    return DirectX::XMFLOAT4X4();
}
