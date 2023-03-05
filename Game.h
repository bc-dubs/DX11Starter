#pragma once

#include "DXCore.h"
#include "Entity.h"
#include "Camera.h"
#include "SimpleShader.h"
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

	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> specialPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::vector<int> specialShaderFuncs;
	std::vector<float> specialShaderVars;

	// A list of objects to draw on-screen
	std::vector<std::shared_ptr<Entity>> entities;
	DirectX::XMFLOAT4 colorTint;
	
	// A list of cameras
	std::vector<std::shared_ptr<Camera>> cameras;
	int cameraIndex;
};

