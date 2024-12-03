#include "aabb.h"

bool AABB::collides(const AABB& other) const {
    return min.x < other.max.x &&
        max.x > other.min.x &&
        min.y < other.max.y &&
        max.y > other.min.y &&
        min.z < other.max.z &&
        max.z > other.min.z;
}

// TODO this code could be cleaned up a lot
glm::vec3 AABB::getCollisionMoveVec(const AABB& other) const {
    float delta = other.min.x - max.x;
    float minMoveDist = std::abs(delta);
    glm::vec3 minMoveVec = glm::vec3(delta, 0, 0);
    delta = other.max.x - min.x;
    float abs = std::abs(delta);
    if (abs < minMoveDist) {
        minMoveDist = abs;
        minMoveVec = glm::vec3(delta, 0, 0);
    }
    delta = other.min.y - max.y;
    abs = std::abs(delta);
    if (abs < minMoveDist) {
        minMoveDist = abs;
        minMoveVec = glm::vec3(0, delta, 0);
    }
    delta = other.max.y - min.y;
    abs = std::abs(delta);
    if (abs < minMoveDist) {
        minMoveDist = abs;
        minMoveVec = glm::vec3(0, delta, 0);
    }
    delta = other.min.z - max.z;
    abs = std::abs(delta);
    if (abs < minMoveDist) {
        minMoveDist = abs;
        minMoveVec = glm::vec3(0, 0, delta);
    }
    delta = other.max.z - min.z;
    abs = std::abs(delta);
    if (abs < minMoveDist) {
        // minMoveDist = abs;
        minMoveVec = glm::vec3(0, 0, delta);
    }
    return minMoveVec;
}

void AABB::translate(const glm::vec3& translation) {
    min += translation;
    max += translation;
}
