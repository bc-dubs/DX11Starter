#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, float roughness)
    : tint(tint),
    vertexShader(vertexShader),
    pixelShader(pixelShader)
{
    SetRoughness(roughness);
    textureSRVs = std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>();
    samplers = std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>();
}

Material::~Material()
{
}

DirectX::XMFLOAT4 Material::GetTint()
{
    return tint;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
    return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
    return pixelShader;
}

float Material::GetRoughness()
{
    return roughness;
}

void Material::SetTint(DirectX::XMFLOAT4 newTint)
{
    tint = newTint;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader)
{
    vertexShader = newVertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader)
{
    pixelShader = newPixelShader;
}

void Material::SetRoughness(float newRoughness)
{
    roughness = max(0.0, min(newRoughness, 1.0));
}

void Material::AddTextureSRV(std::string srvName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
    textureSRVs.insert({ srvName, srv });
}

void Material::AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
    samplers.insert({ samplerName, sampler });
}

void Material::BindMaterial()
{
    for (auto& srv : textureSRVs) {
        pixelShader->SetShaderResourceView(srv.first.c_str(), srv.second);
    }
    for (auto& sampler : samplers) {
        pixelShader->SetSamplerState(sampler.first.c_str(), sampler.second);
    }
}
