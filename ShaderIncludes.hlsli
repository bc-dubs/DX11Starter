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
	float3 tangent			: TANGENT;		// XYZ tangent in U direction
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

// Struct representing the data sent between shaders that account for normal maps
struct VertexToPixel_NormalMap
{
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float4 shadowMapPos		: SHADOW_POSITION; // XYZW position of this pixel in the shadow map
	float3 normal			: NORMAL;		// Surface normal
	float3 tangent			: TANGENT;		// Surface tangent
	float3 worldPosition	: POSITION;		// Position in world space
	float2 uv				: TEXCOORD;     // UV position
};

// Struct representing the data sent between sky shaders
struct VertexToPixel_Sky
{
	float4 position			: SV_POSITION;	// XYZW position (System Value Position)
	float3 sampleDir		: DIRECTION;	// XYZ direction from origin
};

// Struct representing the data sent between post-processing shaders
struct VertexToPixel_Fullscreen
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
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

float3 CalculateDiffuse(float3 normal, float3 directionToLight) {
	return saturate(dot(normal, directionToLight)).rrr;
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

float3x3 CalculateTBN(float3 inputNormal, float3 inputTangent) { // Assumes input values are normalized
	inputTangent = normalize(inputTangent - inputNormal * dot(inputTangent, inputNormal));
	float3 inputBitangent = cross(inputTangent, inputNormal);
	return float3x3(inputTangent, inputBitangent, inputNormal);
}

// =========================================
// =========== PBR LIGHTING CODE ===========
// =========================================


// ===== CONSTANTS =====

// The fresnel value for non-metals (dielectrics)
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;


// ======== PBR FUNCTIONS ========

// Calculates diffuse amount based on energy conservation
//
// diffuse   - Diffuse amount
// F         - Fresnel result from microfacet BRDF
// metalness - surface metalness amount 
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
	return diffuse * (1 - F) * (1 - metalness);
}



// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
	float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
	return a2 / (PI * denomToSquare * denomToSquare);
}



// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
	float VdotH = saturate(dot(v, h));

	// Final value
	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}



// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
	return 1 / (NdotV * (1 - k) + k);
}



// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
	// Other vectors
	float3 h = normalize(v + l);

	// Run numerator functions
	float  D = D_GGX(n, h, roughness);
	float3 F = F_Schlick(v, h, f0);
	float  G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);

	// Pass F out of the function for diffuse balance
	F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
	float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
	return specularResult * max(dot(n, l), 0);
}
#endif