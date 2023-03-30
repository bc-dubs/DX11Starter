#pragma once

// Ben Coukos-Wiley
// 1/24/2023
// A set of vertices and indices that defines an object

#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

class Mesh
{
public:
	/// <summary>
	/// Constructor that takes the raw vertex/index data
	/// </summary>
	Mesh(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
	/// <summary>
	/// Constructor that takes the name of an OBJ file to load from
	/// </summary>
	Mesh(const wchar_t* filename, Microsoft::WRL::ComPtr<ID3D11Device> device);
	~Mesh();

	/// <summary>
	/// Returns this mesh's vertex buffer
	/// </summary>
	/// <returns>This mesh's vertex buffer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	/// <summary>
	/// Returns this mesh's index buffer
	/// </summary>
	/// <returns>This mesh's index buffer</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	/// <summary>
	/// Returns the number of indices in this mesh
	/// </summary>
	/// <returns>The number of indices in this mesh</returns>
	unsigned int GetIndexCount();
	/// <summary>
	/// Draws this mesh
	/// </summary>
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	unsigned int indexCount;

	void Init(Vertex* vertices, int vertexCount, unsigned int* indices, int indexCount, Microsoft::WRL::ComPtr<ID3D11Device> device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);
};

