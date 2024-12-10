#include <iostream>
#include <utility>
#include "collisionobject.h"
#include "realtimescene.h"

CollisionObject::CollisionObject(const RenderShapeData& data,
                                 const std::shared_ptr<RealtimeScene>& scene)
        : super(data, scene) {
    m_aabb = mesh()->computeAABB(CTM());
}


glm::vec3 CollisionObject::translateAndCollide(const glm::vec3& translation) {
    glm::vec3 actualTranslation;
    auto collisionInfoOpt = getCollisionInfo(translation);
    if (collisionInfoOpt.has_value()) {
        actualTranslation = translation + collisionInfoOpt->collisionCorrectionVec;
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

std::optional<CollisionInfo> CollisionObject::getCollisionInfo(const glm::vec3& targetTranslation, int passes) const {
    if (passes <= 0) {
        return std::nullopt;
    }
    std::set<std::shared_ptr<CollisionObject>> collidedObjects;
    glm::vec3 totalCorrectionVec = glm::vec3(0.f);
    AABB movedAABB = m_aabb;
    movedAABB.translate(targetTranslation);
    for (int passesLeft = passes; passesLeft > 0; passesLeft--) {
        bool collisionThisPass = false;
        for (const auto& objectWeak : scene()->collisionObjects()) {
            auto object = objectWeak.lock();
            if (!object) {
                std::cerr << "Collision object weak pointer expired! This shouldn't happen..." << std::endl;
                continue;
            }
            if (object.get() == this) {
                continue;
            }
            if (m_collisionFilter.has_value() && !(*m_collisionFilter)(object)) {
                continue;
            }
            const auto& otherAABB = object->aabb();
            if (movedAABB.collides(otherAABB)) {
                if (collidedObjects.contains(object)) {
                    std::cout << "INFO: object collided with same object twice in one call to getCollisionInfo" << std::endl;
                    continue;
                }
                auto collisionCorrectionVec = movedAABB.getCollisionMoveVec(otherAABB);
                totalCorrectionVec += collisionCorrectionVec;
                movedAABB.translate(collisionCorrectionVec);
                collidedObjects.insert(object);
                collisionThisPass = true;
            }
        }

        if (!collisionThisPass) {
            break;

        }
    }
    if (collidedObjects.empty()) {
        return std::nullopt;
    } else {
        return CollisionInfo{totalCorrectionVec, std::move(collidedObjects)};
    }
}


const AABB& CollisionObject::aabb() const {
    return m_aabb;
}


void CollisionObject::setCollisionFilter(std::function<bool(std::shared_ptr<CollisionObject>)> filter) {
    m_collisionFilter = std::move(filter);
}

std::optional<std::function<bool(std::shared_ptr<CollisionObject>)>> CollisionObject::collisionFilter() const {
    return m_collisionFilter;
}

