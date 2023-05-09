#include "ShaderIncludes.hlsli"

cbuffer externalData : register(b0)
{
	matrix world;		// 
	matrix view;		// Directional light view matrix
	matrix projection;	// Directional light projection matrix
};

float4 main(VertexShaderInput input) : SV_POSITION
{
	matrix wvp = mul(projection, mul(view, world));
	return mul(wvp, float4(input.localPosition, 1.0f));
}