
#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	float4 colorTint;		
	float3 cameraPos;		// The position of the current camera in world space
	float roughness;		// Specular constant based on mesh material
	Light directionalLight1;
	float4 functionVars;	// Unused in this shader
	float3 ambient;			// Scene-wide ambient light constant
	int xFunction;			// Unused in this shader
	int yFunction;			// Unused in this shader
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float3 diffuse = CalculateDiffuse(directionalLight1, input.normal);
	
	return float4((ambient + diffuse) * colorTint, 1);
}