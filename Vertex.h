#pragma once

#include <DirectXMath.h>

// --------------------------------------------------------
// A custom vertex definition
// --------------------------------------------------------
struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The local position of the vertex
	DirectX::XMFLOAT3 Normal;       // The normal vector at the vertex
	DirectX::XMFLOAT3 Tangent;		
	DirectX::XMFLOAT2 UV;			// The UV coordinate at the vertex
};