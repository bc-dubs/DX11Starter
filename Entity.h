#pragma once
// Ben Coukos-Wiley
// 2/8/2023
// An object within the game world

#include "Transform.h"
#include "Mesh.h"
#include <memory>

class Entity
{
public:
	Entity(std::shared_ptr<Mesh> mesh);
	~Entity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

private:
	std::shared_ptr <Transform> transform;
	std::shared_ptr<Mesh> mesh;
};

