#include "ShaderIncludes.hlsli"

#define NUM_LIGHTS 5

cbuffer ExternalData : register(b0) {
	float4 colorTint;
	float3 cameraPos;		// The position of the current camera in world space
	float roughness;		// Specular constant based on mesh material
	Light lights[NUM_LIGHTS];
	float3 ambient;			// Scene-wide ambient light constant
}

Texture2D SurfaceTexture	: register(t0);	// "t" registers for textures
Texture2D SpecularTexture	: register(t1);	// "t" registers for textures
Texture2D NormalMap			: register(t2);
SamplerState BasicSampler	: register(s0);	// "s" registers for samplers


float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	// Normal map code
	float3 textureNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	textureNormal = normalize(textureNormal);

	float3 inputNormal = normalize(input.normal);
	float3 inputTangent = normalize(input.tangent);
	inputTangent = normalize(inputTangent - inputNormal * dot(inputTangent, inputNormal));
	float3 inputBitangent = cross(inputTangent, inputNormal);
	float3x3 TBN = float3x3(inputTangent, inputBitangent, inputNormal);

	input.normal = mul(textureNormal, TBN);

	// Texture code
	float3 pixelColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
	//float pixelSpecular = SpecularTexture.Sample(BasicSampler, input.uv).r;
	float pixelSpecular = 0.5f;

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

	float3 totalColor = (ambient + diffuse + specular * any(diffuse)) * colorTint.rgb * pixelColor;

	// Gamma correcting and returning
	return float4(pow(totalColor, 1 / 2.2f), 1);
}