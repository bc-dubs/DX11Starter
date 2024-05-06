#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    int numSamples;
    float exposure;
    float weight;
    float decay;
    float4 sunPosition; // Location of the sun, in screen coordinates
}

Texture2D Screen			: register(t0);
Texture2D SunAndOcclusion	: register(t1);
SamplerState ClampSampler   : register(s0);

float4 main(VertexToPixel_Fullscreen input) : SV_TARGET
{
    float4 totalLight;
    float2 sunUV = sunPosition.xy;
    sunUV.x = (sunUV.x + 1) / 2;
    sunUV.y = (1 - sunUV.y) / 2;
    float2 distanceToSun = sunUV - input.uv;
    float2 deltaSun = distanceToSun / numSamples;
    for (int i = 0; i < numSamples; i++)
    {
        totalLight += pow(decay, i) * weight * SunAndOcclusion.Sample(ClampSampler, input.uv + deltaSun * i) / numSamples;

    }
    totalLight *= exposure;
    return Screen.Sample(ClampSampler, input.uv) + float4(totalLight.xyz, 1);
}