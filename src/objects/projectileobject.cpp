#include "projectileobject.h"

ProjectileObject::ProjectileObject(const RenderShapeData& data,
                                   const std::shared_ptr<RealtimeScene>& scene,
                                   const glm::vec3& direction,
                                   float speed,
                                   float maxDistance)
        : super(data, scene),
          m_direction(glm::normalize(direction)),
          m_speed(speed),
          m_traveledDistance(0.f),
          m_maxDistance(maxDistance) {
    setShouldRender(true);
}

void ProjectileObject::tick(double elapsedSeconds) {
    // Calculate the translation vector for this tick
    glm::vec3 translation = m_direction * m_speed * (float)elapsedSeconds;

    // Check for collisions
    auto collisionInfoOpt = getCollisionInfo(translation);
    if (collisionInfoOpt.has_value()) {
        // On collision, destroy the projectile
        queueFree();
        return;
    }

    // Move the projectile
    translate(translation);

    // Update the distance traveled
    m_traveledDistance += glm::length(translation);

    // Destroy the projectile if it exceeds max distance
    if (m_traveledDistance >= m_maxDistance) {
        queueFree();
    }
}
