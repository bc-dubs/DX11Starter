#include "Material.h"

Material::Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, float roughness)
    : tint(tint),
    vertexShader(vertexShader),
    pixelShader(pixelShader)
{
    SetRoughness(roughness);
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
