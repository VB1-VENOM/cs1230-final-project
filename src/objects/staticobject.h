#pragma once
#include "collisionobject.h"

class StaticObject : public CollisionObject {
public:
    StaticObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes,
                 std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> collisionObjects);
    /// can't translate a static object, so this throws an error
    void translate(const glm::vec3& translation) override;
    glm::vec3 translateAndCollide(const glm::vec3& translation) override;
private:
    // java-like super
    typedef CollisionObject super;
};