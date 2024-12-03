#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include "realtimeobject.h"

/// "Abstract" class representing an object that is collidable
class CollisionObject : public RealtimeObject {
public:
    /// Translate and stop upon colliding with another collision object; returns the actual translation vector that was applied
    /// Can be overridden in subclasses to achieve custom behavior
    virtual glm::vec3 translateAndCollide(const glm::vec3& translation);
    /// Translates the object by the given vector without considering collision
    void translate(const glm::vec3& translation) override;
    /// Given some target translation, returns the vector that the object should additionally move by to avoid a collision,
    /// if a collision will happen
    std::optional<glm::vec3> collisionCorrectionVec(const glm::vec3& targetTranslation) const;
    // getters
    const AABB& aabb() const;
protected:
    CollisionObject(const RenderShapeData& data, const std::map<PrimitiveType, std::shared_ptr<PrimitiveMesh>>& meshes,
                    std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> collisionObjects);
    std::shared_ptr<std::vector<std::weak_ptr<CollisionObject>>> m_collisionObjects;
private:
    AABB m_aabb;
    // java-like super
    typedef RealtimeObject super;
};
#pragma clang diagnostic pop