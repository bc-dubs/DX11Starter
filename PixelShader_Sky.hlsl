#include "ShaderIncludes.hlsli"

TextureCube SkyTexture	: register(t0);
SamplerState SkySampler : register(s0);

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	return SkyTexture.Sample(SkySampler, input.sampleDir);
}