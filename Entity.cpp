#include "Entity.h"

Entity::Entity(std::shared_ptr<Mesh> mesh) : mesh(mesh)
{
    transform = std::make_shared<Transform>(Transform());
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
