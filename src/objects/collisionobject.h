#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once

#include <set>
#include <functional>
#include "realtimeobject.h"

class CollisionObject;

struct CollisionInfo {
    /// The vector that should be added to the object's translation (after the given targetTranslation) to correct the collision
    glm::vec3 collisionCorrectionVec;

    /// The objects that were collided with
    std::set<std::shared_ptr<CollisionObject>> objects;

};

/// "Abstract" class representing an object that is collidable
class CollisionObject : public RealtimeObject {
public:
    /// Translate and stop upon colliding with another collision object; returns the actual translation vector that was applied
    /// Can be overridden in subclasses to achieve custom behavior
    virtual glm::vec3 translateAndCollide(const glm::vec3& translation);
    /// Translates the object by the given vector without considering collision
    void translate(const glm::vec3& translation) override;
    /// Given some target translation, determines if that translation will cause a collision, and if so,
    /// returns info about the collision: the correction vector and the object collided with
    std::optional<CollisionInfo> getCollisionInfo(const glm::vec3& targetTranslation, int passes = 4) const;

    void setCollisionFilter(std::function<bool(std::shared_ptr<CollisionObject>)> filter);
    std::optional<std::function<bool(std::shared_ptr<CollisionObject>)>> collisionFilter() const;

    const AABB& aabb() const;
protected:
    CollisionObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene);
private:
    AABB m_aabb;
    /// Function that filters which objects this object can collide with. If empty, collides with all objects.
    /// The given function should return true if the object should collide with the given object, and false otherwise.
    std::optional<std::function<bool(std::shared_ptr<CollisionObject>)>> m_collisionFilter = std::nullopt;
    // java-like super
    typedef RealtimeObject super;
};
#pragma clang diagnostic pop
