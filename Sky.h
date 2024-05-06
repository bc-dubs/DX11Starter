// Ben Coukos-Wiley
// 3/30/2023
// A class to hold data for a skybox

#pragma once

#include "Mesh.h"
#include "DXCore.h"
#include "SimpleShader.h"
#include "Camera.h"

#include <memory>
#include <wrl/client.h>

class Sky
{
public:
	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const wchar_t* textureFilepath, const wchar_t* vsFilepath, const wchar_t* psFilepath);
	~Sky();

	void Draw(std::shared_ptr<Camera> camera);
	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

	// Helper for creating a cubemap from 6 individual textures [Chris' Code]
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyTextureSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	Microsoft::WRL::ComPtr<ID3D11Device> device; // Holding onto these so that cube maps can be created as needed
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context; // Holding onto these so that cube maps can be created as needed

	void Init();
};

