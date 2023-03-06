// Ben Coukos-Wiley
// 3/5/2023
// Include file for the main shader functions and code

#ifndef __MAIN_SHADER_INCLUDES__
#define __MAIN_SHADER_INCLUDES__
// Struct representing a single vertex worth of data
// - Variable order must match the vertex definition in our C++ code
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic (e.g. POSITION), which defines its usage
struct VertexShaderInput
{
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;       // XYZ normal
	float2 uv				: TEXCOORD;		// UV position
};

// Struct representing the data we're sending down the pipeline
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
struct VertexToPixel
{
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal			: NORMAL;		// Surface normal
	float3 worldPosition	: POSITION;		// Position in world space
	float2 uv				: TEXCOORD;     // UV position
};

// =========================================
// ============= LIGHTING CODE =============
// =========================================
#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2
#define MAX_SPECULAR_EXPONENT 256.0f

struct Light {
	int type;
	float3 direction;
	float range;
	float3 position;
	float intensity;
	float3 color;
	float spotFalloff;
	float3 padding;
};

float3 CalculateDiffuse(Light light, float3 normal, float3 directionFromLight) {
	float3 directionToLight = normalize(-directionFromLight);
	return saturate(dot(normal, directionToLight)) * light.color * light.intensity;
}

float3 CalculateSpecular(Light light, float3 normal, float3 viewVector, float roughness, float3 directionFromLight) {
	float3 reflection = reflect(normalize(directionFromLight), normal);
	float specExponent = (1.0 - roughness) * MAX_SPECULAR_EXPONENT;
	return (specExponent < MAX_SPECULAR_EXPONENT)? pow(saturate(dot(reflection, viewVector)), specExponent) : 0; // if the roughness value was zero, just return zero for specular
}

float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
	return att * att;
}

#endif