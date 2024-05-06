#include "ShaderIncludes.hlsli"

#define MAX_LIGHTS 5

cbuffer ExternalData : register(b0) {
	float4 colorTint;
	float3 cameraPos;		// The position of the current camera in world space
	int numLights;			// Specular constant based on mesh material
	Light lights[MAX_LIGHTS];
}

TextureCube SkyTexture	: register(t0);
SamplerState SkySampler : register(s0);

struct PS_Output
{
	float4 color	: SV_TARGET0; // Render Target index 0
	float4 sun		: SV_TARGET1; // RT index 1
};

PS_Output main(VertexToPixel_Sky input)
{
	PS_Output output;
	output.color = SkyTexture.Sample(SkySampler, input.sampleDir);
	
	float sunOverlap = 0;

	if (lights[0].type == LIGHT_TYPE_DIRECTIONAL) {
		sunOverlap = clamp(dot(lights[0].direction, -input.sampleDir), 0, 1);
	}

	output.sun = sunOverlap * SkyTexture.Sample(SkySampler, input.sampleDir);
	return output;
}