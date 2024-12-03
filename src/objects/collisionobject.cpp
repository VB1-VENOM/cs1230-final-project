#include <iostream>
#include <utility>
#include "collisionobject.h"

CollisionObject::CollisionObject(const RenderShapeData& data,
                                 const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes,
                                 std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> collisionObjects)
        : super(data, meshes), m_aabb(mesh()->computeAABB(CTM())), m_collisionObjects(std::move(collisionObjects)) {}


glm::vec3 CollisionObject::translateAndCollide(const glm::vec3& translation) {
    glm::vec3 actualTranslation;
    auto collisionCorrectionVecOpt = collisionCorrectionVec(translation);
    if (collisionCorrectionVecOpt.has_value()) {
        actualTranslation = translation + *collisionCorrectionVecOpt;
    } else {
        actualTranslation = translation;
    }
    translate(actualTranslation);
    return actualTranslation;
}

void CollisionObject::translate(const glm::vec3& translation) {
    super::translate(translation);
    m_aabb.translate(translation);
}

std::optional<glm::vec3> CollisionObject::collisionCorrectionVec(const glm::vec3& targetTranslation) const {
    AABB movedAABB = m_aabb;
    movedAABB.translate(targetTranslation);
    for (const auto& objectWeak : *m_collisionObjects) {
        auto object = objectWeak.lock();
        if (!object) {
            std::cerr << "Collision object weak pointer expired! This shouldn't happen..." << std::endl;
            continue;
        }
        if (object.get() == this) {
            continue;
        }
        const auto& otherAABB = object->aabb();
        if (movedAABB.collides(otherAABB)) {
            return movedAABB.getCollisionMoveVec(otherAABB);
        }
    }
    return std::nullopt;
}


const AABB& CollisionObject::aabb() const {
    return m_aabb;
}