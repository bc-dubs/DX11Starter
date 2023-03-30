#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	matrix world;
	matrix worldInvTranspose;
	matrix view;
	matrix projection;
}

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel_NormalMap main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel_NormalMap output;

	// X and Y components must be between -1 and 1, Z component must be between 0 and 1.  
	// Each of these components is then automatically divided by the W component (1.0 for now)
	matrix wvp = mul(projection, mul(view, world));
	output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));

	// Transform the normal in the same way this vertex was transformed
	output.normal = mul((float3x3)worldInvTranspose, input.normal); // Why does this work again???

	output.tangent = mul((float3x3)world, input.tangent);

	// Transform local position and output as a float3
	output.worldPosition = mul(world, float4(input.localPosition, 1)).xyz;

	// Pass the uv coordinate through 
	output.uv = input.uv;

	// Send the output to the next stage (pixel shader)
	return output;
}