#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Entity.h"
#include "Camera.h"
#include "Lights.h"
#include "Sky.h"

#include <memory>
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	/// <summary>
	/// Function called in Update() that handles ImGui
	/// </summary>
	void UpdateImGui(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void ShadowInit();
	void RenderTargetInit();

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> specialPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> vertexShader_NormalMap;
	std::shared_ptr<SimplePixelShader> pixelShader_NormalMap;
	std::shared_ptr<SimpleVertexShader> vertexShader_ShadowMap;
	std::shared_ptr<SimpleVertexShader> vertexShader_Fullscreen;
	std::shared_ptr<SimplePixelShader> pixelShader_Blur;

	std::vector<int> specialShaderFuncs;
	std::vector<float> specialShaderVars;

	// A list of objects to draw on-screen
	std::vector<std::shared_ptr<Entity>> entities;
	std::shared_ptr<Sky> skybox;
	DirectX::XMFLOAT4 ambientColor;
	
	// A list of cameras
	std::vector<std::shared_ptr<Camera>> cameras;
	int cameraIndex;

	// A list of lights
	std::unordered_map<int, std::shared_ptr<Light>> activeLights;
	std::vector<Light> lightsToRender;
	std::vector<std::shared_ptr<Light>> allLights;

	// Shadow mapping variables
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	int shadowMapResolution;
	float lightDisplacement; // These refer to our main directional light
	DirectX::XMFLOAT4X4 lightViewMatrix;
	DirectX::XMFLOAT4X4 lightProjectionMatrix;

	// Post-processing variables
	Microsoft::WRL::ComPtr<ID3D11SamplerState> postProcessSampler;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderRTV; // as opposed to the post-render RTV (our normal back buffer)
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> renderSRV;
	int blurRadius;

	// Volumetric Light Variables
	Microsoft::WRL::ComPtr<ID3D11Texture2D> sunAndOccludersTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> sunAndOccludersRTV; // as opposed to the post-render RTV (our normal back buffer)
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sunAndOccludersSRV;
};

