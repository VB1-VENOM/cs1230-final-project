#pragma once

#include <unordered_map>
#include <chrono>
#include "collisionobject.h"
#include "camera.h"

// TODO play with defaults
#define DEFAULT_ENEMY_GRAVITY 15.f
#define ENEMY_SPEED 2.f
#define EPSILON 0.0001f
#define HEALTH 3
#define ON_ENEMY_HIT_FLASH_MS 300

class EnemyObject : public CollisionObject {
public:
    EnemyObject(RenderShapeData& data, const std::shared_ptr<RealtimeScene>& scene,
                 std::shared_ptr<Camera> camera, std::shared_ptr<bool> taken_damage);
    void tick(double elapsedSeconds) override;
    void onShot();
    /// Moves the enemy
    void translate(const glm::vec3& translation) override;


private:
    int health = HEALTH;
    std::shared_ptr<Camera> m_camera;
    float m_gravity = DEFAULT_ENEMY_GRAVITY;
    glm::vec3 m_velocity = glm::vec3(0.f);
    bool m_onGround = false;
    RenderShapeData& m_renderShapeData;

    std::chrono::time_point<std::chrono::steady_clock> damage_end_time;
    std::shared_ptr<bool> m_taken_damage;

    // java-like super
    typedef CollisionObject super;
};