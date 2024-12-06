#include "staticobject.h"
#include "realtimescene.h"

StaticObject::StaticObject(const RenderShapeData& data,
                           const std::shared_ptr<RealtimeScene>& scene) :
                           super(data, scene) {}

void StaticObject::translate(const glm::vec3& translation) {
    throw std::runtime_error("Can't translate static object");
}

glm::vec3 StaticObject::translateAndCollide(const glm::vec3& translation) {
    throw std::runtime_error("Can't translate static object");
}
