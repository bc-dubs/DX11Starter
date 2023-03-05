// Ben Coukos-Wiley
// 3/3/2023
// Instructions for how a mesh should be shaded

#pragma once

#include "SimpleShader.h"

#include <DirectXMath.h>
#include <memory>

class Material
{
public:
	Material(DirectX::XMFLOAT4 tint, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader, float roughness);
	~Material();

	// Getters
	DirectX::XMFLOAT4 GetTint();
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();
	float GetRoughness();

	// Setters
	void SetTint(DirectX::XMFLOAT4 newTint);
	void SetVertexShader(std::shared_ptr<SimpleVertexShader> newVertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> newPixelShader);
	void SetRoughness(float newRoughness);

private:
	DirectX::XMFLOAT4 tint;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;
	float roughness; // must be between zero and one

};

