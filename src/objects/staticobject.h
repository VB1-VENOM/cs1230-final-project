#pragma once
#include "collisionobject.h"

class StaticObject : public CollisionObject {
public:
    StaticObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene);
    /// can't translate a static object, so this throws an error
    void translate(const glm::vec3& translation) override;
    glm::vec3 translateAndCollide(const glm::vec3& translation) override;
private:
    // java-like super
    typedef CollisionObject super;
};