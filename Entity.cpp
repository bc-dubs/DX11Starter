#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) 
    : mesh(mesh), 
    material(material)
{
    transform = std::make_shared<Transform>(Transform()); // The entity starts at (0, 0, 0) by default
}

Entity::~Entity()
{
}

std::shared_ptr<Mesh> Entity::GetMesh()
{
    return mesh;
}

std::shared_ptr<Transform> Entity::GetTransform()
{
    return transform;
}

std::shared_ptr<Material> Entity::GetMaterial()
{
    return material;
}

void Entity::SetMesh(std::shared_ptr<Mesh> newMesh)
{
    mesh = newMesh;
}

void Entity::SetMaterial(std::shared_ptr<Material> newMaterial)
{
    material = newMaterial;
}
