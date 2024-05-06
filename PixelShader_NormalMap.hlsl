#include "ShaderIncludes.hlsli"

#define MAX_LIGHTS 5

cbuffer ExternalData : register(b0) {
	float4 colorTint;
	float3 cameraPos;		// The position of the current camera in world space
	int numLights;			// Specular constant based on mesh material
	Light lights[MAX_LIGHTS];
}

struct PS_Output
{
	float4 color		: SV_TARGET0; // Render Target index 0
	float4 occluders	: SV_TARGET1; // RT index 1
};


Texture2D AlbedoTexture		: register(t0);	// "t" registers for textures
Texture2D RoughnessMap		: register(t1);
Texture2D NormalMap			: register(t2);
Texture2D MetalnessMap		: register(t3);
Texture2D ShadowMap			: register(t4);
SamplerState BasicSampler	: register(s0);	// "s" registers for samplers
SamplerComparisonState ShadowSampler : register(s1);


PS_Output main(VertexToPixel_NormalMap input)
{
	PS_Output output;
	// ========== SHADOW MAPPING CODE ==========
	// Perform the perspective divide (divide by W) ourselves as the rasterizer will not do it automatically
	input.shadowMapPos /= input.shadowMapPos.w;
	// Convert the normalized device coordinates to UVs for sampling
	float2 shadowUV = input.shadowMapPos.xy * 0.5f + 0.5f;
	shadowUV.y = 1 - shadowUV.y; // Flip the Y since UVs have an opposite Y axis
	
	float distToLight = input.shadowMapPos.z; // Distance from the light to this surface
	// Get a ratio of comparison results using SampleCmpLevelZero()
	float shadowAmount = ShadowMap.SampleCmpLevelZero(
		ShadowSampler,
		shadowUV,
		distToLight).r;
	
	// ========== NON-SHADOW CODE ==========
	// Normal map code
	float3 textureNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	textureNormal = normalize(textureNormal);

	float3x3 TBN = CalculateTBN(normalize(input.normal), normalize(input.tangent));
	input.normal = mul(textureNormal, TBN);

	// Texture code
	float3 albedo = pow(AlbedoTexture.Sample(BasicSampler, input.uv).rgb, 2.2f) * colorTint.rgb; // un-gamma correcting
	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
	float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;

	float3 fresnelAt0 = lerp(F0_NON_METAL, albedo.rgb, metalness); // Metals' f0 value is stored in albedo textures, nonmetals' is a constant

	// Vector code
	float3 viewVector = normalize(cameraPos - input.worldPosition);

	float3 totalColor;

	// Using BRDFs
	for (int i = 0; i < numLights; i++) {
		float3 directionToLight = -lights[i].direction;
		float intensity = lights[i].intensity;
		if (lights[i].type == 1) { // Values for point lights
			directionToLight = lights[i].position - input.worldPosition;
			intensity *= Attenuate(lights[i], input.worldPosition);
		}
		directionToLight = normalize(directionToLight);

		float3 diffuse = CalculateDiffuse(input.normal, directionToLight); // diffuse component for this pixel
		float3 fresnel;
		float3 specular = MicrofacetBRDF(input.normal, directionToLight, viewVector, roughness, fresnelAt0, fresnel); // specular component for this pixel
		diffuse = DiffuseEnergyConserve(diffuse, fresnel, metalness);

		float3 lightColor = (diffuse * albedo + specular) * lights[i].intensity * lights[i].color;

		// Shadow the light results only from the main directional light
		if (i == 0)
		{
			lightColor *= shadowAmount;
		}

		totalColor += lightColor;
	}

	// Gamma correcting and returning
	output.color = float4(pow(totalColor, 1 / 2.2f), 1);
	output.occluders = float4(0, 0, 0, 1);
	return output;
}