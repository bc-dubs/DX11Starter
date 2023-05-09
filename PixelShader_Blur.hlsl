#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	int blurRadius;		// in pixel units
	float pixelWidth;	// in UV space units
	float pixelHeight;	// in UV space units
}

Texture2D Screen			: register(t0);
SamplerState ClampSampler	: register(s0);

float4 main(VertexToPixel_Fullscreen input) : SV_TARGET
{
	float4 pixelColor = 0;
	int sampleCount = 0;

	for (int x = -blurRadius; x <= blurRadius; x++) {
		for (int y = -blurRadius; y <= blurRadius; y++) {
			float2 uv = input.uv + float2(x * pixelWidth, y * pixelHeight);

			pixelColor += Screen.Sample(ClampSampler, uv);
			sampleCount++;
		}
	}

	// Averaging color across all pixels sampled
	return pixelColor / sampleCount;
}