#pragma once
// Ben Coukos-Wiley
// 2/8/2023
// An object within the game world

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include <memory>

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material); // add constructor that takes transform or one that takes parent?
	~Entity();

	// Getters
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Material> GetMaterial();

	// Setters
	void SetMesh(std::shared_ptr<Mesh> newMesh);
	void SetMaterial(std::shared_ptr<Material> newMaterial);

private:
	std::shared_ptr <Transform> transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
};

