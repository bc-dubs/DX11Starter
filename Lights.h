// Ben Coukos-Wiley
// 03/05/2023
// Defining the structure of lights in the engine

#pragma once

#include <DirectXMath.h>

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct Light 
{
	int Type;						// 0/1/2
	DirectX::XMFLOAT3 Direction;	// Used for types 0 and 2
	float Range;					// Maximum distance a light affects (used in 1 and 2)
	DirectX::XMFLOAT3 Position;		// Location of a light in space (1 and 2)
	float Intensity;				
	DirectX::XMFLOAT3 Color;
	float SpotFalloff;				// How far light spreads from the center of a beam (type 2)
	DirectX::XMFLOAT3 Padding;		// In order to make this exactly 16 bytes wide
};
