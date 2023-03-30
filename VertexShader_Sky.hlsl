#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0) {
	matrix view;
	matrix projection;
}

VertexToPixel_Sky main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel_Sky output;

	matrix untranslatedView = view;
	untranslatedView._14 = 0;
	untranslatedView._24 = 0;
	untranslatedView._34 = 0;

	matrix vp = mul(projection, untranslatedView);

	output.position = mul(vp, float4(input.localPosition, 1.0f));
	output.position.z = output.position.w;

	output.sampleDir = input.localPosition;

	return output;
}