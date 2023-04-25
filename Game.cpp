#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"
#include "Material.h"

#include <WICTextureLoader.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>


// For the DirectX Math library
using namespace DirectX;
using namespace std;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	cameraIndex = 0;
	ambientColor = XMFLOAT4(0.05f, 0.15f, 0.2f, 1.0f);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	LoadShaders();
	CreateGeometry();
	
	// Set initial graphics API state
	//  - These settings persist until we change them
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Input layout now handled by SimpleShader
	}

	// Create camera(s)
	cameras = vector<shared_ptr<Camera>>();
	cameras.push_back(std::make_shared<Camera>(Camera((float)this->windowWidth / this->windowHeight, XMFLOAT3(0, 0, -10))));
	cameras.push_back(std::make_shared<Camera>(Camera((float)this->windowWidth / this->windowHeight, XMFLOAT3(0, 0, -10), XMFLOAT4(0, 0, 0, 1), XM_PIDIV4, 5, 0.001f, 0.01f, 50, false, XMFLOAT3(0.4f, 0.6f, 0))));

	// Create lights
	allLights = vector<shared_ptr<Light>>();
	activeLights = unordered_map<int, shared_ptr<Light>>();
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = XMFLOAT3(-1, -1, 0.1f);
		directionalLight.Color = XMFLOAT3(1.0f, 0.3f, 0.3f);
		directionalLight.Intensity = 1.0f;
		allLights.push_back(make_shared<Light>(directionalLight));
		activeLights.insert({ 0, allLights[0]});
	}
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = XMFLOAT3(0, 0, -1);
		directionalLight.Color = XMFLOAT3(0.2f, 0.2f, 5.0f);
		directionalLight.Intensity = 0.5f;
		allLights.push_back(make_shared<Light>(directionalLight));
	}
	{
		Light directionalLight = {};
		directionalLight.Type = LIGHT_TYPE_DIRECTIONAL;
		directionalLight.Direction = XMFLOAT3(1, -1, -0.1f);
		directionalLight.Color = XMFLOAT3(0.5f, 0.15f, 0.15f);
		directionalLight.Intensity = 1.0f;
		allLights.push_back(make_shared<Light>(directionalLight));
	}
	// Point lights
	{
		Light pointLight = {};
		pointLight.Type = LIGHT_TYPE_POINT;
		pointLight.Position = XMFLOAT3(3, 0, 0);
		pointLight.Range = 4.0f;
		pointLight.Color = XMFLOAT3(0.5f, 0.5f, 0.0f);
		pointLight.Intensity = 1.0f;
		allLights.push_back(make_shared<Light>(pointLight));
	}
	{
		Light pointLight = {};
		pointLight.Type = LIGHT_TYPE_POINT;
		pointLight.Position = XMFLOAT3(0, 0, -5);
		pointLight.Range = 10.0f;
		pointLight.Color = XMFLOAT3(1.0f, 0.0f, 1.0f);
		pointLight.Intensity = 0.5f;
		allLights.push_back(make_shared<Light>(pointLight));
	}

	lightsToRender = vector<Light>();
	for (auto& light : activeLights) {
		lightsToRender.push_back(*light.second);
	}

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also creates the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader.cso").c_str());
	specialPixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"SpecialPixelShader.cso").c_str());
	vertexShader_NormalMap = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader_NormalMap.cso").c_str());
	pixelShader_NormalMap = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader_NormalMap.cso").c_str());

	// Below variables are only used in the specialPixelShader
	specialShaderFuncs = std::vector<int>();
	specialShaderFuncs.push_back(0);
	specialShaderFuncs.push_back(0);
	specialShaderFuncs.push_back(0);
	specialShaderFuncs.push_back(0);

	specialShaderVars = std::vector<float>();
	specialShaderVars.push_back(2);
	specialShaderVars.push_back(2);
	specialShaderVars.push_back(1);
	specialShaderVars.push_back(2.82f);
}



// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tileSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tileSpecularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSpecularSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rocksSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rocksNormalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\tiles_diffuse.png").c_str(), nullptr, tileSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\tiles_specular.png").c_str(), nullptr, tileSpecularSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\rustymetal_diffuse.png").c_str(), nullptr, metalSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\rustymetal_specular.png").c_str(), nullptr, metalSpecularSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\rock.png").c_str(), nullptr, rocksSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"..\\..\\Assets\\Textures\\rock_normals.png").c_str(), nullptr, rocksNormalSRV.GetAddressOf());


	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;

	{
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter			= D3D11_FILTER_ANISOTROPIC;
		samplerDesc.MaxAnisotropy	= 16;
		samplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

		device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	}

	// Create some temporary variables to represent colors
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 gold	= XMFLOAT4(1.0f, 0.84f, 0.0f, 1.0f);
	XMFLOAT4 white	= XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Creating materials
	
	std::shared_ptr<Material> tilesWithSpecular = std::make_shared<Material>(white, vertexShader, pixelShader, 0.5f);
	std::shared_ptr<Material> metalWithSpecular = std::make_shared<Material>(white, vertexShader, pixelShader, 0.2f);
	std::shared_ptr<Material> rocksWithNormals = std::make_shared<Material>(white, vertexShader_NormalMap, pixelShader_NormalMap, 0.9f);

	tilesWithSpecular->AddTextureSRV("SurfaceTexture", tileSRV);
	tilesWithSpecular->AddTextureSRV("SpecularTexture", tileSpecularSRV);
	tilesWithSpecular->AddSampler("BasicSampler", sampler);

	metalWithSpecular->AddTextureSRV("SurfaceTexture", metalSRV);
	metalWithSpecular->AddTextureSRV("SpecularTexture", metalSpecularSRV);
	metalWithSpecular->AddSampler("BasicSampler", sampler);

	rocksWithNormals->AddTextureSRV("SurfaceTexture", rocksSRV);
	rocksWithNormals->AddTextureSRV("NormalMap", rocksNormalSRV);
	rocksWithNormals->AddSampler("BasicSampler", sampler);

	// Creating pointers to each mesh object
	shared_ptr<Mesh> cubeMesh = make_shared<Mesh>(FixPath(L"..\\..\\Assets\\Meshes\\cube.obj").c_str(), device);
	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>(FixPath(L"..\\..\\Assets\\Meshes\\sphere.obj").c_str(), device);
	shared_ptr<Mesh> torusMesh = make_shared<Mesh>(FixPath(L"..\\..\\Assets\\Meshes\\torus.obj").c_str(), device);

	// Creating entity objects
	entities = std::vector<std::shared_ptr<Entity>>();
	entities.push_back(std::make_shared<Entity>(torusMesh, metalWithSpecular));
	entities.push_back(std::make_shared<Entity>(cubeMesh, tilesWithSpecular));
	entities.push_back(std::make_shared<Entity>(sphereMesh, rocksWithNormals));
	entities.push_back(std::make_shared<Entity>(torusMesh, tilesWithSpecular));
	entities.push_back(std::make_shared<Entity>(cubeMesh, rocksWithNormals));

	// Arranging entities regularly
	{
		int numCols = 4;
		float colSpacing = 3.5f;
		float rowSpacing = 3.2f;
		for (unsigned int i = 0; i < entities.size(); i++) {
			std::shared_ptr<Entity> entity = entities[i];
			float colIndex = (i % numCols) - numCols / 2.0f;
			float rowIndex = i / numCols - (entities.size() / numCols) / 2.0f; // Offsets to center arrangement on approximately 0, 0
			entity->GetTransform()->MoveBy(colIndex * colSpacing, -rowIndex * rowSpacing, 0);
		}
	}

	// Old entity transformations
	/*entities[0]->GetTransform()->MoveBy(-4.0f, 0.0f, 0.0f);
	entities[0]->GetTransform()->ScaleBy(0.5f, 0.5f, 0.5f);
	entities[1]->GetTransform()->MoveBy(0.0f, 0.2f, 0.0f);
	entities[1]->GetTransform()->ScaleBy(2.0f, 2.0f, 2.0f);
	entities[2]->GetTransform()->MoveBy(4.0f, 0.0f, 0.0f);
	entities[3]->GetTransform()->MoveBy(-4.0f, -2.0f, 0.0f);
	entities[4]->GetTransform()->MoveBy(-4.0f, 2.0f, 0.0f);*/

	// Create sky
	skybox = make_shared<Sky>(cubeMesh, sampler, device, context, FixPath(L"..\\..\\Assets\\Textures\\Sky_Pink").c_str(), FixPath(L"VertexShader_Sky.cso").c_str(), FixPath(L"PixelShader_Sky.cso").c_str());
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	// Update the cameras' aspect ratios
	for (unsigned int i = 0; i < cameras.size(); i++) {
		cameras[i]->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Main game loop
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime, totalTime);
	for (unsigned int i = 0; i < entities.size(); i++) {
		std::shared_ptr<Entity> entity = entities[i];
		entity->GetTransform()->RotateBy(0.0f, deltaTime/4, 0.0f);
	}

	cameras[cameraIndex]->Update(deltaTime);

	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	for (unsigned int i = 0; i < entities.size(); i++) {
		// Defining temporary variables for cleaner code (hopefully no performance cost here?)
		std::shared_ptr<Entity> entity = entities[i];
		std::shared_ptr<Material> material = entity->GetMaterial();
		std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
		std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

		// Setting what used to be constant buffer data, now handled by simpleShader
		vs->SetMatrix4x4("world", entity->GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("worldInvTranspose", entity->GetTransform()->GetWorldInverseTransposeMatrix());
		vs->SetMatrix4x4("view", cameras[cameraIndex]->GetViewMatrix());
		vs->SetMatrix4x4("projection", cameras[cameraIndex]->GetProjectionMatrix());
		vs->CopyAllBufferData();

		ps->SetFloat4("colorTint", material->GetTint());
		ps->SetFloat3("cameraPos", *cameras[cameraIndex]->GetTransform()->GetPosition());
		ps->SetFloat("roughness", material->GetRoughness());
		ps->SetData("lights", &lightsToRender[0], sizeof(Light) * (int)lightsToRender.size());
		ps->SetFloat3("ambient", XMFLOAT3(ambientColor.x, ambientColor.y, ambientColor.z));

		// If the shader is using vector field functions
		if (ps->HasVariable("functionVars")) {
			ps->SetFloat4("functionVars", XMFLOAT4(specialShaderVars[0], specialShaderVars[1], specialShaderVars[2], specialShaderVars[3]));
			ps->SetInt("xFunction", specialShaderFuncs[0] * 4 + specialShaderFuncs[1]);
			ps->SetInt("yFunction", specialShaderFuncs[2] * 4 + specialShaderFuncs[3]);
		}

		material->BindMaterial();
		ps->CopyAllBufferData();

		// Setting the current material's shaders
		vs->SetShader();
		ps->SetShader();

		entity->GetMesh()->Draw(context);
	}
	
	skybox->Draw(cameras[cameraIndex]);

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		
		// Render UI
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}

void Game::UpdateImGui(float deltaTime, float totalTime)
{
	// Feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	//ImGui::ShowDemoWindow();

	ImGui::Begin("Custom GUI");

	// Basic information
	ImGui::Text("The current framerate is %f", ImGui::GetIO().Framerate);
	ImGui::Text("The game window is %i pixels wide and %i pixels high", windowWidth, windowHeight);
	ImGui::ColorEdit4("Ambient light color", &ambientColor.x);

	// Camera GUI
	if (ImGui::CollapsingHeader("Cameras")) {
		for (unsigned int i = 0; i < cameras.size(); i++) {
			char radioLabel[] = { 'C', 'a', 'm', 'e', 'r', 'a', ' ', (char)(i+65), '\0'};
			ImGui::RadioButton(radioLabel, &cameraIndex, i); ImGui::SameLine();
			XMFLOAT3 pos = *cameras[i]->GetTransform()->GetPosition();
			char posLabel[] = { 'P', 'o', 's', 'i', 't', 'i', 'o', 'n', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat3(posLabel, &pos.x);
			/*
			XMFLOAT3 rot;
			XMStoreFloat3(&rot, XMVector3Normalize(XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(cameras[i]->GetTransform()->GetRotation()))));
			char rotLabel[] = { 'D', 'i', 'r', 'e', 'c', 't', 'i', 'o', 'n', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat3(rotLabel, &rot.x);
			*/
			XMFLOAT3 fwd = *cameras[i]->GetTransform()->GetForward();
			char fwdLabel[] = { 'F', 'o', 'r', 'e', 'w', 'a', 'r', 'd', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat3(fwdLabel, &fwd.x);

			XMFLOAT3 rgt = *cameras[i]->GetTransform()->GetRight();
			char rgtLabel[] = { 'R', 'i', 'g', 'h', 't', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat3(rgtLabel, &rgt.x);

			ImGui::Text("Pitch: %f", cameras[i]->GetTransform()->GetPitch());
			ImGui::Text("Yaw: %f", cameras[i]->GetTransform()->GetYaw());

			/*
			XMFLOAT4 quat = *cameras[i]->GetTransform()->GetRotation();
			char quatLabel[] = { 'Q', 'u', 'a', 't', 'e', 'r', 'n', 'i', 'o', 'n', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat4(quatLabel, &quat.x);*/
		}
	}

	// Entity GUI
	if (ImGui::CollapsingHeader("Entities")) {
		for (unsigned int i = 0; i < entities.size(); i++) {
			if (ImGui::TreeNode((void*)(intptr_t)i, "Entity %i", i)) {

				XMFLOAT3 pos = *entities[i]->GetTransform()->GetPosition();
				XMFLOAT4 rot = *entities[i]->GetTransform()->GetRotation(); // All I could think to do is just display quaternion data, though I know that's not super intuitive
				XMFLOAT3 sc = *entities[i]->GetTransform()->GetScale();

				ImGui::DragFloat3("Position", &pos.x, 0.01f);
				ImGui::DragFloat4("Rotation", &rot.x, 0.01f);
				ImGui::DragFloat3("Scale", &sc.x, 0.01f);
				ImGui::Text("Tris: %i", entities[i]->GetMesh()->GetIndexCount() / 3);

				entities[i]->GetTransform()->SetPosition(pos);
				entities[i]->GetTransform()->SetRotation(rot);
				entities[i]->GetTransform()->SetScale(sc);
				
				ImGui::TreePop();
			}
		}
	}

	// Vector field shader GUI
	const char* xFunctions[] = { "x", "x^2", "x^(1/2)", "2^x", "ln(x)/ln(2)", "sin(x)", "cos(x)", "tan(x)" };
	const char* yFunctions[] = { "y", "y^2", "y^(1/2)", "2^y", "ln(y)/ln(2)", "sin(y)", "cos(y)", "tan(y)" };
	const char* xInputs[] = { "x", "y", "x+y", "x-y" };
	const char* yInputs[] = { "y", "x", "y+x", "y-x" };
	if (ImGui::CollapsingHeader("Vector Field Functions")) {
		ImGui::Combo("X Function", &specialShaderFuncs[0], xFunctions, IM_ARRAYSIZE(xFunctions), 5);
		ImGui::Combo("X Input", &specialShaderFuncs[1], xInputs, IM_ARRAYSIZE(xInputs), 4);
		ImGui::InputFloat("X coefficient", &specialShaderVars[0], 0.1f, 1.0f, "% .2f");
		ImGui::Separator();
		ImGui::Combo("Y Function", &specialShaderFuncs[2], yFunctions, IM_ARRAYSIZE(yFunctions), 5);
		ImGui::Combo("Y Input", &specialShaderFuncs[3], yInputs, IM_ARRAYSIZE(yInputs), 4);
		ImGui::InputFloat("Y coefficient", &specialShaderVars[1], 0.1f, 1.0f, "% .2f");
		ImGui::Separator();
		ImGui::InputFloat("Magnitude divisor", &specialShaderVars[2], 0.01f, 1.0f, "% .3f");
		ImGui::InputFloat("Minimum light divisor", &specialShaderVars[3], 0.01f, 1.0f, "% .3f");
	}

	// Lighting GUI
	const char* lightTypes[] = { "Directional", "Point", "Spot" };
	if (ImGui::CollapsingHeader("Lights")) {
		for (unsigned int i = 0; i < allLights.size(); i++) {
			if (ImGui::TreeNode((void*)(intptr_t)i, "Light %c (%s)", (char)(i + 65), lightTypes[allLights[i]->Type])) {
				const bool wasActive = activeLights.count(i);
				bool nowActive = wasActive;
				ImGui::Checkbox("Active", &nowActive);
				if (nowActive && !wasActive) {
					activeLights.insert({ i, allLights[i] });
				}
				else if (!nowActive && wasActive && activeLights.size() > 1) {
					activeLights.erase(i);
				}

				XMFLOAT3 lightColor = allLights[i]->Color;
				ImGui::ColorEdit3("Color", &lightColor.x);
				allLights[i]->Color = lightColor;


				ImGui::TreePop();
			}
		}
	}
	lightsToRender = vector<Light>();
	for (auto& light : activeLights) {
		lightsToRender.push_back(*light.second);
	}

	ImGui::End();
}
