#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-nodiscard"
#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

/// Axis-aligned bounding box for an object, in world-space coordinates. Used for collision detection
struct AABB {
    /// Corner of the box with min x, y, and z
    glm::vec3 min;
    /// Corner of the box with max x, y, and z
    glm::vec3 max;

    /// Checks if the AABB overlaps with `other`
    bool collides(const AABB& other) const;

    /// Given a colliding AABB, returns a minimal translation vector to move this AABB out of that one
    glm::vec3 getCollisionMoveVec(const AABB& other) const;

    /// Translates the AABB by the given vector
    void translate(const glm::vec3& translation);
};

#pragma clang diagnostic pop