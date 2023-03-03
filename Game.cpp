#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
#include "Material.h"

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
	colorTint = XMFLOAT4(0.1f, 1.0f, 0.5f, 1.0f);
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

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
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateGeometry();
	
	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.
//		context->IASetInputLayout(inputLayout.Get());

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
//		context->VSSetShader(vertexShader.Get(), 0, 0);
//		context->PSSetShader(pixelShader.Get(), 0, 0);
	}

	{
		// Create camera(s)
		cameras = vector<shared_ptr<Camera>>();
		cameras.push_back(std::make_shared<Camera>(Camera((float)this->windowWidth / this->windowHeight, XMFLOAT3(0, 0, -10))));
		cameras.push_back(std::make_shared<Camera>(Camera((float)this->windowWidth / this->windowHeight, XMFLOAT3(0, 0, 10), XMFLOAT4(1, 0, 0, 0), XM_PIDIV4, 5, 0.001f, 0.01f, 50, false, XMFLOAT3(0.4f, 0.6f, 0))));
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
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	XMFLOAT4 gold	= XMFLOAT4(1.0f, 0.84f, 0.0f, 1.0f);

	std::shared_ptr<Material> redMaterial = std::make_shared<Material>(red, vertexShader, pixelShader);
	std::shared_ptr<Material> greenMaterial = std::make_shared<Material>(green, vertexShader, pixelShader);
	std::shared_ptr<Material> goldMaterial = std::make_shared<Material>(gold, vertexShader, pixelShader);


	// Creating pointers to each mesh object
	
	shared_ptr<Mesh> cubeMesh = make_shared<Mesh>(FixPath(L"..\\Assets\\Meshes\\cube.obj").c_str(), device, context);
	shared_ptr<Mesh> sphereMesh = make_shared<Mesh>(FixPath(L"..\\Assets\\Meshes\\sphere.obj").c_str(), device, context);
	shared_ptr<Mesh> torusMesh = make_shared<Mesh>(FixPath(L"..\\Assets\\Meshes\\torus.obj").c_str(), device, context);

	entities = std::vector<std::shared_ptr<Entity>>();
	entities.push_back(std::make_shared<Entity>(cubeMesh, goldMaterial));
	entities.push_back(std::make_shared<Entity>(cubeMesh, greenMaterial));
	entities.push_back(std::make_shared<Entity>(torusMesh, redMaterial));
	entities.push_back(std::make_shared<Entity>(torusMesh, redMaterial));
	entities.push_back(std::make_shared<Entity>(torusMesh, redMaterial));

	entities[0]->GetTransform()->MoveBy(-0.125f, 0.0f, 0.0f);
	entities[0]->GetTransform()->ScaleBy(0.5f, 0.5f, 0.5f);
	entities[1]->GetTransform()->MoveBy(0.0f, 0.2f, 0.0f);
	entities[1]->GetTransform()->ScaleBy(2.0f, 2.0f, 2.0f);
	entities[2]->GetTransform()->MoveBy(0.45f, 0.45f, 0.0f);
	entities[3]->GetTransform()->MoveBy(-0.45f, -0.45f, 0.0f);
	entities[4]->GetTransform()->MoveBy(-0.45f, 0.45f, 0.0f);
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
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
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime, totalTime);

	entities[0]->GetTransform()->MoveBy(deltaTime * 0.25f * (float) sin(totalTime), deltaTime * 0.25f * (float) cos(totalTime), 0.0f);
	entities[1]->GetTransform()->RotateBy(0.0f, 0.0f, deltaTime);
	//entities[2].GetTransform()->ScaleBy(1.1 - sin(totalTime), 1.1 - sin(totalTime), 1.1 - sin(totalTime));
	entities[2]->GetTransform()->ScaleBy(1 + deltaTime * 0.25f * (float) sin(totalTime), 1.0f, 1.0f);
	entities[3]->GetTransform()->RotateBy(0.0f, 0.0f, deltaTime);
	entities[4]->GetTransform()->RotateBy(0.0f, 0.0f, -deltaTime);

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
		// Defining starting points
		std::shared_ptr<Entity> entity = entities[i];
		std::shared_ptr<Material> material = entity->GetMaterial();
		std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
		std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();

		// Setting what used to be constant buffer data, now handled by simpleShader
		vs->SetMatrix4x4("world", entity->GetTransform()->GetWorldMatrix());
		vs->SetMatrix4x4("view", cameras[cameraIndex]->GetViewMatrix());
		vs->SetMatrix4x4("projection", cameras[cameraIndex]->GetProjectionMatrix());
		vs->CopyAllBufferData();

		ps->SetFloat4("colorTint", material->GetTint());
		ps->CopyAllBufferData();

		// Material code
		vs->SetShader();
		ps->SetShader();

		entity->GetMesh()->Draw();
	}
	

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

	ImGui::Text("The current framerate is %f", ImGui::GetIO().Framerate);
	ImGui::Text("The game window is %i pixels wide and %i pixels high", windowWidth, windowHeight);
	ImGui::ColorEdit4("Geometry tint", &colorTint.x);

	// Camera GUI
	if (ImGui::CollapsingHeader("Cameras")) {
		for (unsigned int i = 0; i < cameras.size(); i++) {
			char radioLabel[] = { 'C', 'a', 'm', 'e', 'r', 'a', ' ', (char)(i+65), '\0'};
			ImGui::RadioButton(radioLabel, &cameraIndex, i); ImGui::SameLine();
			XMFLOAT3 pos = *cameras[i]->GetTransform()->GetPosition();
			char posLabel[] = { 'P', 'o', 's', 'i', 't', 'i', 'o', 'n', ' ', (char)(i + 65), '\0' };
			ImGui::DragFloat3(posLabel, &pos.x);
		}
	}

	// Entity GUI
	if (ImGui::CollapsingHeader("Entities")) {
		for (unsigned int i = 0; i < entities.size(); i++) {
			if (ImGui::TreeNode((void*)(intptr_t)i, "Entity %i", i)) {

				XMFLOAT3 pos = *entities[i]->GetTransform()->GetPosition();
				XMFLOAT4 rot = *entities[i]->GetTransform()->GetRotation(); // All I could think to do is just display quaternion data, though I know that's not super intuitive
				XMFLOAT3 sc = *entities[i]->GetTransform()->GetScale();

				ImGui::DragFloat3("Position", &pos.x); // I also couldn't figure out how to make these editable
				ImGui::DragFloat4("Rotation", &rot.x);
				ImGui::DragFloat3("Scale", &sc.x);
				ImGui::Text("Tris: %i", entities[i]->GetMesh()->GetIndexCount() / 3);
				
				ImGui::TreePop();
			}
		}
	}

	ImGui::End();
}
