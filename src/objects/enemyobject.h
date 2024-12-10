#pragma once

#include <unordered_map>
#include "collisionobject.h"
#include "camera.h"

// TODO play with defaults
#define DEFAULT_ENEMY_GRAVITY 15.f
#define ENEMY_SPEED 2.f
#define EPSILON 0.0001f

class EnemyObject : public CollisionObject {
public:
    EnemyObject(const RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene,
                 std::shared_ptr<Camera> camera);
    void tick(double elapsedSeconds) override;
    /// Moves the enemy
    void translate(const glm::vec3& translation) override;


private:

    std::shared_ptr<Camera> m_camera;
    float m_gravity = DEFAULT_ENEMY_GRAVITY;
    glm::vec3 m_velocity = glm::vec3(0.f);
    bool m_onGround = false;

    // java-like super
    typedef CollisionObject super;
};