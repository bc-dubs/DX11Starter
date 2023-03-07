
#include "ShaderIncludes.hlsli"

#define NUM_LIGHTS 5

cbuffer ExternalData : register(b0) {
	float4 colorTint;		
	float3 cameraPos;		// The position of the current camera in world space
	float roughness;		// Specular constant based on mesh material
	Light lights[NUM_LIGHTS];
	float4 functionVars;	// Unused in this shader
	float3 ambient;			// Scene-wide ambient light constant
	int xFunction;			// Unused in this shader
	int yFunction;			// Unused in this shader
}

Texture2D SurfaceTexture : register(t0);	// "t" registers for textures
Texture2D SpecularTexture : register(t1);	// "t" registers for textures
SamplerState BasicSampler : register(s0);	// "s" registers for samplers

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Texture code
	float3 pixelColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
	float pixelSpecular = SpecularTexture.Sample(BasicSampler, input.uv).r;

	// Lighting code
	input.normal = normalize(input.normal);
	float3 viewVector = normalize(cameraPos - input.worldPosition);

	float3 diffuse;
	float3 specular;
	
	for (int i = 0; i < NUM_LIGHTS; i++) {

		float3 lightDirection = lights[i].direction;
		float attenuation = 1.0;
		if (lights[i].type == 1) {
			lightDirection = normalize(input.worldPosition - lights[i].position);
			attenuation = Attenuate(lights[i], input.worldPosition);
		}

		diffuse += CalculateDiffuse(lights[i], input.normal, lightDirection) * attenuation;
		specular += CalculateSpecular(lights[i], input.normal, viewVector, roughness, lightDirection) * attenuation * pixelSpecular;
	}
	
	return float4((ambient + diffuse + specular) * colorTint * pixelColor, 1);
}