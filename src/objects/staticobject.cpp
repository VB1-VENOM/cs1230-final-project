
#include "staticobject.h"

StaticObject::StaticObject(const RenderShapeData& data,
                           const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes,
                           std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> collisionObjects) :
                           super(data, meshes, std::move(collisionObjects)) {}

void StaticObject::translate(const glm::vec3& translation) {
    throw std::runtime_error("Can't translate static object");
}

glm::vec3 StaticObject::translateAndCollide(const glm::vec3& translation) {
    throw std::runtime_error("Can't translate static object");
}
