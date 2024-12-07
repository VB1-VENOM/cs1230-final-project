#include "projectileobject.h"

#include <iostream>
#include <ostream>
#include <glm/gtx/transform.hpp>

#include "realtimescene.h"

ProjectileObject::ProjectileObject(const RenderShapeData& data,
                                   const std::shared_ptr<RealtimeScene>& scene,
                                   const glm::vec3& direction,
                                   float speed,
                                   float maxDistance,
                                   bool isBullet)
        : super(data, scene),
          m_direction(glm::normalize(direction)),
          m_speed(speed),
          m_traveledDistance(0.f),
          m_maxDistance(maxDistance),
          m_isBullet(isBullet)
{
    setShouldRender(true);
}

void ProjectileObject::tick(double elapsedSeconds) {
    // Calculate the translation vector for this tick
    glm::vec3 translation = m_direction * m_speed * (float)elapsedSeconds;

    // Check for collisions
    auto collisionInfoOpt = getCollisionInfo(translation);
    if (collisionInfoOpt.has_value() && m_isBullet) {
        // On collision, destroy the projectile

        collisionSphereEffect();

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

void ProjectileObject::collisionSphereEffect()
{
    int numProjectiles = 100; // Number of projectiles to spawn
    float speed = 10.0f;     // Speed of the additional projectiles
    float maxDistance = 10.0f; // Max distance for the spawned projectiles
    std::cout << "ENtering sphere effect" << std::endl;
    for (int i = 0; i < numProjectiles; ++i) {
        // Generate a random direction for the new projectile
        glm::vec3 randomDirection = glm::normalize(glm::vec3(
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f
        ));

        // Create render shape data for the projectile
        ScenePrimitive projectilePrimitive{PrimitiveType::PRIMITIVE_SPHERE,
            SceneMaterial{SceneColor{0.1f, 0.1f, 0.1f, 1.f}, SceneColor{1.f, 1.f, 1.f, 1.f}}};
        glm::mat4 projectileCTM = this->CTM();
        projectileCTM = glm::scale(projectileCTM, glm::vec3(0.1f)); // Smaller spheres
        projectileCTM = glm::translate(projectileCTM, randomDirection);

        RenderShapeData projectileData{projectilePrimitive, projectileCTM};

        // Add the new projectile to the scene
        scene()->addObject(std::make_unique<ProjectileObject>(
            projectileData, scene(), randomDirection, speed, maxDistance, false));
        // scene()->addObject(std::make_unique<CollisionObject>(
        //     projectileData, scene()));
    }
}

